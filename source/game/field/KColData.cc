#include "KColData.hh"

#include <egg/geom/Sphere.hh>
#include <egg/math/Math.hh>

#include <cmath>

// Credit: em-eight/mkw
// Credit: stblr/Hanachan

namespace Field {

/// @addr{0x807BDC5C}
KColData::KColData(const void *file) {
    auto addOffset = [](const void *file, u32 offset) -> const void * {
        return reinterpret_cast<const void *>(reinterpret_cast<const u8 *>(file) + offset);
    };
    EGG::RamStream stream = EGG::RamStream(file, sizeof(KColHeader));

    u32 posOffset = stream.read_u32();
    u32 nrmOffset = stream.read_u32();
    u32 prismOffset = stream.read_u32();
    u32 blockOffset = stream.read_u32();

    m_posData = addOffset(file, posOffset);
    m_nrmData = addOffset(file, nrmOffset);
    m_prismData = addOffset(file, prismOffset);
    m_blockData = addOffset(file, blockOffset);

    m_prismThickness = stream.read_f32();
    m_areaMinPos.read(stream);
    m_areaXWidthMask = stream.read_u32();
    m_areaYWidthMask = stream.read_u32();
    m_areaZWidthMask = stream.read_u32();
    m_blockWidthShift = stream.read_u32();
    m_areaXBlocksShift = stream.read_u32();
    m_areaXYBlocksShift = stream.read_u32();
    m_sphereRadius = stream.read_f32();

    m_pos.setZero();
    m_prevPos.setZero();
    m_movement.setZero();
    m_radius = 0.0f;
    m_prismIter = nullptr;
    m_cachedPrismArray = m_prismCache.data() - 1;

    // NOTE: Collision is expensive on the CPU, so we preload all of the prism data to ensure we're
    // not constantly handling endianness.
    preloadPrisms();
    preloadNormals();
    preloadVertices();

    computeBBox();
}

KColData::~KColData() {
    delete[] m_prisms.data();
    delete[] m_nrms.data();
    delete[] m_vertices.data();
}

/// @addr{0x807C24C0}
void KColData::narrowScopeLocal(const EGG::Vector3f &pos, f32 radius, KCLTypeMask mask) {
    m_prismCacheTop = m_prismCache.data();
    m_pos = pos;
    m_radius = radius;
    m_typeMask = mask;
    m_cachedPos = pos;
    m_cachedRadius = radius;

    if (radius <= m_sphereRadius) {
        narrowPolygon_EachBlock(searchBlock(pos));
    }

    *m_prismCacheTop = 0;
}

/// @addr{0x807C243C}
void KColData::narrowPolygon_EachBlock(const u16 *prismArray) {
    m_prismIter = prismArray;

    while (checkSphereSingle(nullptr, nullptr, nullptr)) {
        /// We assume the cache has same endianness as the archive file,
        /// so do not parse out the prism index and directly store it in the cache.
        *(m_prismCacheTop++) = *m_prismIter;

        if (m_prismCacheTop == m_prismCache.end()) {
            --m_prismCacheTop;
            return;
        }
    }
}

/// @brief Calculates a EGG::BoundBox3f that describes the boundary of the track's KCL
/// @addr{0x807BDDFC}
void KColData::computeBBox() {
    m_bbox.max.set(-999999.0f);
    m_bbox.min.set(999999.0f);

    for (size_t i = 1; i < m_prisms.size(); ++i) {
        const auto &prism = m_prisms[i];
        const EGG::Vector3f &fnrm = m_nrms[prism.fnrm_i];
        const EGG::Vector3f &enrm1 = m_nrms[prism.enrm1_i];
        const EGG::Vector3f &enrm2 = m_nrms[prism.enrm2_i];
        const EGG::Vector3f &enrm3 = m_nrms[prism.enrm3_i];
        const EGG::Vector3f &vtx1 = m_vertices[prism.pos_i];

        const EGG::Vector3f vtx2 = GetVertex(prism.height, vtx1, fnrm, enrm3, enrm1);
        const EGG::Vector3f vtx3 = GetVertex(prism.height, vtx1, fnrm, enrm3, enrm2);

        m_bbox.min = m_bbox.min.minimize(vtx1);
        m_bbox.min = m_bbox.min.minimize(vtx2);
        m_bbox.min = m_bbox.min.minimize(vtx3);
        m_bbox.max = m_bbox.max.maximize(vtx1);
        m_bbox.max = m_bbox.max.maximize(vtx2);
        m_bbox.max = m_bbox.max.maximize(vtx3);
    }
}

/// @addr{0x807C1F80}
bool KColData::checkPointCollision(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut) {
    return std::isfinite(m_prevPos.y) ? checkPointMovement(distOut, fnrmOut, flagsOut) :
                                        checkPoint(distOut, fnrmOut, flagsOut);
}

/// @addr{0x807C2410}
bool KColData::checkSphereCollision(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut) {
    return std::isfinite(m_prevPos.y) ? checkSphereMovement(distOut, fnrmOut, flagsOut) :
                                        checkSphere(distOut, fnrmOut, flagsOut);
}

/// @brief Iterates the list of looked-up triangles to see if we are colliding
/// @addr{0x807C1514}
/// @param distOut If colliding, returns the distance between the player and the triangle
/// @param fnrmOut If colliding, returns the floor normal of the triangle
/// @param flagsOut If colliding, returns the KCL attributes for that triangle
/// @return whether or not the player is colliding with the triangle
bool KColData::checkSphere(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut) {
    // If there's no list of triangles to check, there's no collision
    if (!m_prismIter) {
        return false;
    }

    // Check collision for all triangles, and continuously call the function until we're out
    while (*++m_prismIter != 0) {
        const KCollisionPrism &prism = m_prisms[parse<u16>(*m_prismIter)];
        if (checkCollision(prism, distOut, fnrmOut, flagsOut, CollisionCheckType::Plane)) {
            return true;
        }
    }

    // We're out of triangles to check - another list must be prepared for subsequent calls
    m_prismIter = nullptr;
    return false;
}

/// @addr{0x807C0F00}
bool KColData::checkSphereSingle(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut) {
    if (!m_prismIter) {
        return false;
    }

    while (*++m_prismIter != 0) {
        if (m_prismCacheTop != m_prismCache.begin()) {
            u16 *puVar10 = m_prismCacheTop - 1;
            while (*m_prismIter != *puVar10) {
                if (puVar10-- < m_prismCache.begin()) {
                    break;
                }
            }

            if (puVar10 >= m_prismCache.begin()) {
                continue;
            }
        }

        const KCollisionPrism &prism = m_prisms[parse<u16>(*m_prismIter)];
        if (checkCollision(prism, distOut, fnrmOut, flagsOut, CollisionCheckType::Edge)) {
            return true;
        }
    }

    m_prismIter = nullptr;
    return false;
}

/// @brief Sets members in preparation of a subsequent point collision check call
/// @addr{0x807C1B0C}
void KColData::lookupPoint(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask typeMask) {
    m_prismIter = searchBlock(pos);
    m_pos = pos;
    m_prevPos = prevPos;
    m_movement = pos - prevPos;
    m_typeMask = typeMask;
}

/// @brief Sets members in preparation of a subsequent sphere collision check call
/// @addr{0x807C1BB4}
void KColData::lookupSphere(f32 radius, const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask typeMask) {
    m_prismIter = searchBlock(pos);
    m_pos = pos;
    m_prevPos = prevPos;
    m_movement = pos - prevPos;
    m_radius = std::min(radius, m_sphereRadius);
    m_typeMask = typeMask;
}

/// @addr{0x807C1DE8}
void KColData::lookupSphereCached(const EGG::Vector3f &p1, const EGG::Vector3f &p2, u32 typeMask,
        f32 radius) {
    EGG::Sphere3f sphere1(p1, radius);
    EGG::Sphere3f sphere2(m_cachedPos, m_cachedRadius);

    if (!sphere1.isInsideOtherSphere(sphere2)) {
        m_prismIter = searchBlock(p1);
        m_radius = std::min(m_sphereRadius, radius);
    } else {
        m_radius = radius;
        m_prismIter = m_cachedPrismArray;
    }

    m_pos = p1;
    m_prevPos = p2;
    m_movement = p1 - p2;
    m_typeMask = typeMask;
}

/// @brief Finds the data block corresponding to the provided position
/// @addr{0x807BE030}
/// @param point The player's position
/// @return the address of the leaf node containing the input point.
const u16 *KColData::searchBlock(const EGG::Vector3f &point) {
    // Calculate the x, y, and z offsets of the point from the minimum
    // corner of the tree's bounding box.
    const int x = point.x - m_areaMinPos.x;
    const int y = point.y - m_areaMinPos.y;
    const int z = point.z - m_areaMinPos.z;

    // Check if the point is outside the tree's bounding box in the x, y,
    // or z dimensions. If it is, return 0.
    if (x & m_areaXWidthMask || y & m_areaYWidthMask || z & m_areaZWidthMask) {
        return nullptr;
    }

    // Initialize the current tree node to the root node of the tree.
    u32 shift = m_blockWidthShift;
    const u8 *curBlock = reinterpret_cast<const u8 *>(m_blockData);
    s32 offset;

    // Traverse the tree to find the leaf node containing the input point.
    u32 index = 4 *
            (((u32)z >> shift) << m_areaXYBlocksShift | ((u32)y >> shift) << m_areaXBlocksShift |
                    (u32)x >> shift);

    while (true) {
        // Get the offset of the current node's child node.
        offset = parse<u32>(*reinterpret_cast<const u32 *>(curBlock + index));

        // If the offset is negative, the current node is a leaf node.
        if ((offset & 0x80000000) != 0) {
            break;
        }

        // If the offset is non-negative, update the current node to be
        // the child node and continue traversing the tree.
        shift--;
        curBlock += offset;

        u32 x_shift = ((1 * ((u32)x >> shift)) & 1);
        u32 y_shift = ((2 * ((u32)y >> shift)) & 2);
        u32 z_shift = ((4 * ((u32)z >> shift)) & 4);

        index = 4 * (x_shift | y_shift | z_shift);
    }

    // We have to remove the MSB since it's solely used to identify leaves.
    return reinterpret_cast<const u16 *>(curBlock + (offset & ~0x80000000));
}

/// @brief Computes a prism vertex based off of the triangle's normal vectors
/// @addr{0x807BDF54}
/// @par Triangle Vertices Formula
/// Given a triangle with vertices \f$\vec{A}, \vec{B}, \vec{C}\f$, face normal \f$\hat{f}\f$, and
/// height \f$h\f$, label the edge normals by: \begin{aligned}\hat{en}_1 := e_{AB}, \,\,
/// \hat{en}_2:= e_{AC}, \,\,\hat{en}_3:=e_{BC} \end{aligned} We can recover \f$\vec{B}, \vec{C}\f$
/// via: \begin{aligned} \vec{B} = \vec{A} + \dfrac{h}{(\hat{en}_2 \times \hat{f}) \cdot
/// \hat{en}_3}\left(\hat{en}_2 \times \hat{f}\right), \, \, \vec{C} = \vec{A} +
/// \dfrac{h}{(\hat{en}_1 \times \hat{f}) \cdot \hat{en}_3}(\hat{en}_1 \times \hat{f}) \, .
/// \end{aligned}
EGG::Vector3f KColData::GetVertex(f32 height, const EGG::Vector3f &vertex1,
        const EGG::Vector3f &fnrm, const EGG::Vector3f &enrm3, const EGG::Vector3f &enrm) {
    EGG::Vector3f cross = fnrm.cross(enrm);
    f32 dp = cross.ps_dot(enrm3);
    cross *= (height / dp);

    return cross + vertex1;
}

/// @brief Creates a copy of the prisms in memory.
/// @details Optimizes for time by copying all of the prisms to avoid constant byteswapping.
/// Memory cost is typically upwards of a few hundred KB, with the worst case being ~1MB.
void KColData::preloadPrisms() {
    size_t prismCount =
            (reinterpret_cast<uintptr_t>(m_blockData) - reinterpret_cast<uintptr_t>(m_prismData)) /
            sizeof(KCollisionPrism);

    EGG::RamStream stream = EGG::RamStream(m_prismData, sizeof(KCollisionPrism) * prismCount);

    m_prisms = std::span<KCollisionPrism>(new KCollisionPrism[prismCount], prismCount);

    // Because the prisms are one-indexed, we insert an empty prism
    stream.skip(sizeof(KCollisionPrism));

    for (size_t i = 1; i < prismCount; ++i) {
        KCollisionPrism &prism = m_prisms[i];
        prism.height = stream.read_f32();
        prism.pos_i = stream.read_u16();
        prism.fnrm_i = stream.read_u16();
        prism.enrm1_i = stream.read_u16();
        prism.enrm2_i = stream.read_u16();
        prism.enrm3_i = stream.read_u16();
        prism.attribute = stream.read_u16();
    }
}

/// @brief Creates a copy of the normals in memory.
/// @details Optimizes for time by copying all of the normals to avoid constant byteswapping.
/// Memory cost is typically upwards of a few hundred KB, with the worst case being ~750KB.
void KColData::preloadNormals() {
    size_t normalCount = (reinterpret_cast<uintptr_t>(m_prismData) + sizeof(KCollisionPrism) -
                                 reinterpret_cast<uintptr_t>(m_nrmData)) /
            sizeof(EGG::Vector3f);

    m_nrms = std::span<EGG::Vector3f>(new EGG::Vector3f[normalCount], normalCount);
    EGG::RamStream stream = EGG::RamStream(m_nrmData, sizeof(EGG::Vector3f) * normalCount);

    for (auto &nrm : m_nrms) {
        nrm.read(stream);
    }
}

/// @brief Creates a copy of the vertices in memory.
/// @details Optimizes for time by copying all of the vertices to avoid constant byteswapping.
/// Memory cost is typically upwards of a few hundred KB, with the worst case being ~750KB.
void KColData::preloadVertices() {
    size_t vertexCount =
            (reinterpret_cast<uintptr_t>(m_nrmData) - reinterpret_cast<uintptr_t>(m_posData)) /
            sizeof(EGG::Vector3f);

    m_vertices = std::span<EGG::Vector3f>(new EGG::Vector3f[vertexCount], vertexCount);
    EGG::RamStream stream = EGG::RamStream(m_posData, sizeof(EGG::Vector3f) * vertexCount);

    for (auto &vert : m_vertices) {
        vert.read(stream);
    }
}

/// @brief This is a combination of the three collision checks in the base game.
/// @details The checks vary only by a few if-statements, related to whether we are checking for:
/// 1. A collision with at least the triangle edge (0x807C0F00)
/// 2. A collision with the triangle plane (0x807C1514)
/// 3. A collision such that we are inside the triangle (0x807C0884)
bool KColData::checkCollision(const KCollisionPrism &prism, f32 *distOut, EGG::Vector3f *fnrmOut,
        u16 *flagsOut, CollisionCheckType type) {
    // Responsible for updating the output params
    auto out = [&](f32 dist) {
        if (distOut) {
            *distOut = dist;
        }
        if (fnrmOut) {
            *fnrmOut = m_nrms[prism.fnrm_i];
        }
        if (flagsOut) {
            *flagsOut = prism.attribute;
        }
        return true;
    };

    // The flag check occurs earlier than in the base game here. We don't want to do math if the tri
    // we're checking doesn't have matching flags.
    u32 attributeMask = KCL_ATTRIBUTE_TYPE_BIT(prism.attribute);
    if (!(attributeMask & m_typeMask)) {
        return false;
    }

    const EGG::Vector3f relativePos = m_pos - m_vertices[prism.pos_i];

    // Edge normals point outside the triangle
    const EGG::Vector3f &enrm1 = m_nrms[prism.enrm1_i];
    f32 dist_ca = relativePos.ps_dot(enrm1);
    if (m_radius <= dist_ca) {
        return false;
    }

    const EGG::Vector3f &enrm2 = m_nrms[prism.enrm2_i];
    f32 dist_ab = relativePos.ps_dot(enrm2);
    if (m_radius <= dist_ab) {
        return false;
    }

    const EGG::Vector3f &enrm3 = m_nrms[prism.enrm3_i];
    f32 dist_bc = relativePos.ps_dot(enrm3) - prism.height;
    if (m_radius <= dist_bc) {
        return false;
    }

    const EGG::Vector3f &fnrm = m_nrms[prism.fnrm_i];
    f32 plane_dist = relativePos.ps_dot(fnrm);
    f32 dist_in_plane = m_radius - plane_dist;
    if (dist_in_plane <= 0.0f) {
        return false;
    }

    f32 typeDistance = m_prismThickness;
    if (type == CollisionCheckType::Edge) {
        typeDistance += m_radius;
    }

    if (dist_in_plane >= typeDistance) {
        return false;
    }

    if (type == CollisionCheckType::Movement) {
        if (attributeMask & KCL_TYPE_DIRECTIONAL && m_movement.dot(fnrm) > 0.0f) {
            return false;
        }
    }

    // Originally part of the edge searching, but moved out for simplicity
    // If these are all zero, then we're inside the triangle
    if (dist_ab <= 0.0f && dist_bc <= 0.0f && dist_ca <= 0.0f) {
        if (type == CollisionCheckType::Movement) {
            EGG::Vector3f lastPos = relativePos - m_movement;
            // We're only colliding if we are moving towards the face
            if (plane_dist < 0.0f && lastPos.ps_dot(fnrm) < 0.0f) {
                return false;
            }
        }
        return out(dist_in_plane);
    }

    EGG::Vector3f edge_nor, other_edge_nor;
    f32 edge_dist, other_edge_dist;
    bool swap = false;
    bool swapNorms = false;
    // > means further, < means closer, = means same distance
    if (dist_ab >= dist_ca && dist_ab > dist_bc) {
        // AB is the furthest edge
        edge_nor = enrm2;
        edge_dist = dist_ab;
        if (dist_ca >= dist_bc) {
            // CA is the second furthest edge
            other_edge_nor = enrm1;
            other_edge_dist = dist_ca;
            swapNorms = true;
        } else {
            // BC is the second furthest edge
            other_edge_nor = enrm3;
            other_edge_dist = dist_bc;
            swap = true;
        }
    } else if (dist_bc >= dist_ca) {
        // BC is the furthest edge
        edge_nor = enrm3;
        edge_dist = dist_bc;
        if (dist_ab >= dist_ca) {
            // AB is the second furthest edge
            other_edge_nor = enrm2;
            other_edge_dist = dist_ab;
            swapNorms = true;
        } else {
            // CA is the second furthest edge
            other_edge_nor = enrm1;
            other_edge_dist = dist_ca;
            swap = true;
        }
    } else {
        // CA is the furthest edge
        edge_nor = enrm1;
        edge_dist = dist_ca;
        if (dist_bc >= dist_ab) {
            // BC is the second furthest edge
            other_edge_nor = enrm3;
            other_edge_dist = dist_bc;
            swapNorms = true;
        } else {
            // AB is the second furthest edge
            other_edge_nor = enrm2;
            other_edge_dist = dist_ab;
            swap = true;
        }
    }

    f32 cos = edge_nor.ps_dot(other_edge_nor);
    f32 sq_dist;
    if (cos * edge_dist > other_edge_dist) {
        if (type == CollisionCheckType::Plane) {
            if (edge_dist > plane_dist) {
                return false;
            }
        }
        sq_dist = m_radius * m_radius - edge_dist * edge_dist;
    } else {
        f32 sq_sin = cos * cos - 1.0f;

        if (swap) {
            std::swap(edge_dist, other_edge_dist);
        }

        if (swapNorms) {
            std::swap(edge_nor, other_edge_nor);
        }

        f32 t = (cos * edge_dist - other_edge_dist) / sq_sin;
        f32 s = edge_dist - t * cos;
        const EGG::Vector3f corner_pos = edge_nor * t + other_edge_nor * s;

        f32 cornerDot = corner_pos.ps_squareMag();
        if (type == CollisionCheckType::Plane) {
            if (cornerDot > plane_dist * plane_dist) {
                return false;
            }
        }

        sq_dist = m_radius * m_radius - cornerDot;
    }

    if (sq_dist < plane_dist * plane_dist || sq_dist <= 0.0f) {
        return false;
    }

    f32 dist = EGG::Mathf::sqrt(sq_dist) - plane_dist;
    if (dist <= 0.0f) {
        return false;
    }

    if (type == CollisionCheckType::Movement) {
        EGG::Vector3f lastPos = relativePos - m_movement;
        // We're only colliding if we are moving towards the face
        if (lastPos.ps_dot(fnrm) < 0.0f) {
            return false;
        }
    }

    return out(dist);
}

/// @brief This is a combination of two point collision check functions. They only vary based on
/// whether we are checking movement.
bool KColData::checkPointCollision(const KCollisionPrism &prism, f32 *distOut,
        EGG::Vector3f *fnrmOut, u16 *flagsOut, bool movement) {
    KCLTypeMask attrMask = KCL_ATTRIBUTE_TYPE_BIT(prism.attribute);
    if (!(attrMask & m_typeMask)) {
        return false;
    }

    const EGG::Vector3f relativePos = m_pos - m_vertices[prism.pos_i];

    const EGG::Vector3f &enrm1 = m_nrms[prism.enrm1_i];
    f32 dist_ca = relativePos.ps_dot(enrm1);
    if (dist_ca >= 0.01f) {
        return false;
    }

    const EGG::Vector3f &enrm2 = m_nrms[prism.enrm2_i];
    f32 dist_ab = relativePos.ps_dot(enrm2);
    if (dist_ab >= 0.01f) {
        return false;
    }

    const EGG::Vector3f &enrm3 = m_nrms[prism.enrm3_i];
    f32 dist_bc = relativePos.ps_dot(enrm3) - prism.height;
    if (dist_bc >= 0.01f) {
        return false;
    }

    const EGG::Vector3f &fnrm = m_nrms[prism.fnrm_i];
    f32 plane_dist = relativePos.ps_dot(fnrm);
    f32 dist_in_plane = 0.01f - plane_dist;
    if (dist_in_plane <= 0.0f) {
        return false;
    }

    if (m_prismThickness <= dist_in_plane && 0.02f + m_prismThickness <= dist_in_plane) {
        return false;
    }

    if (movement && (attrMask & KCL_TYPE_DIRECTIONAL) && m_movement.dot(fnrm) < 0.0f) {
        return false;
    }

    if (distOut) {
        *distOut = dist_in_plane;
    }

    if (fnrmOut) {
        *fnrmOut = fnrm;
    }

    if (flagsOut) {
        *flagsOut = prism.attribute;
    }

    return true;
}

/// @brief Iterates the local data block to check for directional collision
/// @addr{0x807C0884}
/// @param distOut If colliding, returns the distance between the player and the tri
/// @param fnrmOut If colliding, returns the floor normal of the triangle
/// @param attributeOut If colliding, returns the KCL attributes for that triangle
/// @return Whether or not a collision has occurred
bool KColData::checkSphereMovement(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *attributeOut) {
    // If there's no list of triangles to check, there's no collision
    if (!m_prismIter) {
        return false;
    }

    // Check collision for all triangles, and continuously call the function until we're out
    while (*++m_prismIter != 0) {
        const KCollisionPrism &prism = m_prisms[parse<u16>(*m_prismIter)];
        if (checkCollision(prism, distOut, fnrmOut, attributeOut, CollisionCheckType::Movement)) {
            return true;
        }
    }

    // We're out of triangles to check - another list must be prepared for subsequent calls
    m_prismIter = nullptr;
    return false;
}

/// @addr{0x807C21F4}
bool KColData::checkPoint(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *attributeOut) {
    // If there's no list of triangles to check, there's no collision
    if (!m_prismIter) {
        return false;
    }

    // Check collision for all triangles, and continuously call the function until we're out
    while (*++m_prismIter != 0) {
        const KCollisionPrism &prism = m_prisms[parse<u16>(*m_prismIter)];
        if (checkPointCollision(prism, distOut, fnrmOut, attributeOut, false)) {
            return true;
        }
    }

    // We're out of triangles to check - another list must be prepared for subsequent calls
    m_prismIter = nullptr;
    return false;
}

/// @addr{0x807C1F80}
bool KColData::checkPointMovement(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *attributeOut) {
    // If there's no list of triangles to check, there's no collision
    if (!m_prismIter) {
        return false;
    }

    // Check collision for all triangles, and continuously call the function until we're out
    while (*++m_prismIter != 0) {
        const KCollisionPrism &prism = m_prisms[parse<u16>(*m_prismIter)];
        if (checkPointCollision(prism, distOut, fnrmOut, attributeOut, true)) {
            return true;
        }
    }

    // We're out of triangles to check - another list must be prepared for subsequent calls
    m_prismIter = nullptr;
    return false;
}

KColData::KCollisionPrism::KCollisionPrism() = default;

KColData::KCollisionPrism::KCollisionPrism(f32 height, u16 posIndex, u16 faceNormIndex,
        u16 edge1NormIndex, u16 edge2NormIndex, u16 edge3NormIndex, u16 attribute)
    : height(height), pos_i(posIndex), fnrm_i(faceNormIndex), enrm1_i(edge1NormIndex),
      enrm2_i(edge2NormIndex), enrm3_i(edge3NormIndex), attribute(attribute) {}

void CollisionInfo::update(f32 now_dist, const EGG::Vector3f &offset, const EGG::Vector3f &fnrm,
        u32 kclAttributeTypeBit) {
    bbox.min = bbox.min.minimize(offset);
    bbox.max = bbox.max.maximize(offset);

    if (kclAttributeTypeBit & KCL_TYPE_FLOOR) {
        updateFloor(now_dist, fnrm);
    } else if (kclAttributeTypeBit & KCL_TYPE_WALL) {
        if (wallDist > -std::numeric_limits<f32>::min()) {
            f32 dot = 1.0f - wallNrm.ps_dot(fnrm);
            if (dot > perpendicularity) {
                perpendicularity = std::min(dot, 1.0f);
            }
        }

        updateWall(now_dist, fnrm);
    }
}

/// @addr{0x807C26AC}
void CollisionInfo::transformInfo(CollisionInfo &rhs, const EGG::Matrix34f &mtx,
        const EGG::Vector3f &v) {
    rhs.bbox.min = mtx.ps_multVector33(rhs.bbox.min);
    rhs.bbox.max = mtx.ps_multVector33(rhs.bbox.max);

    EGG::Vector3f min = rhs.bbox.min;

    rhs.bbox.min = min.minimize(rhs.bbox.max);
    rhs.bbox.max = min.maximize(rhs.bbox.max);

    bbox.min = bbox.min.minimize(rhs.bbox.min);
    bbox.max = bbox.max.maximize(rhs.bbox.max);

    if (floorDist < rhs.floorDist) {
        floorDist = rhs.floorDist;
        floorNrm = mtx.ps_multVector33(rhs.floorNrm);
    }

    if (wallDist < rhs.wallDist) {
        wallDist = rhs.wallDist;
        wallNrm = mtx.ps_multVector33(rhs.wallNrm);
    }

    if (movingFloorDist < rhs.floorDist) {
        movingFloorDist = rhs.floorDist;
        roadVelocity = v;
    }

    perpendicularity = std::max(perpendicularity, rhs.perpendicularity);
}

} // namespace Field
