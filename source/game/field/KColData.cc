#include "KColData.hh"

#include <egg/geom/Sphere.hh>
#include <egg/math/Math.hh>

#include <cmath>

// Credit: em-eight/mkw
// Credit: stblr/Hanachan

namespace Kinoko::Field {

/// @addr{0x807BDC5C}
/// @brief Constructor that parses KCL data from the provided file pointer
/// @param file Pointer to the `.kcl` file in memory
/// @details Reads the file header to identify the file offsets for the position, normal, prism, and
/// block data sections. Reads the prism thickness and various octree navigation parameters. Because
/// Kinoko may be run on systems not sharing the same endianness as the `.kcl` file, Kinoko adds
/// @ref preloadPrismGeometry() to ensure all data is correctly byte-swapped upfront and not done
/// on-the-fly for performance reasons. Finally, determines the minimum required bounding box that
/// contains all KCL tris.
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

    // NOTE: Collision is expensive on the CPU, so we preload all of the prism data to ensure we're
    // not constantly handling endianness.
    preloadPrismGeometry();

    computeBBox();
}

/// @brief Default destructor
KColData::~KColData() = default;

/// @addr{0x807C24C0}
/// @brief Prepares the prism cache for a sphere query and performs the query
/// @param pos The position of the sphere
/// @param radius The radius of the sphere
/// @param mask The type mask to filter which prisms are considered for the query
void KColData::narrowScopeLocal(const EGG::Vector3f &pos, f32 radius, KCLTypeMask mask) {
    m_prismCacheIter = m_prismCache.data();
    m_pos = pos;
    m_radius = radius;
    m_typeMask = mask;
    m_cachedPos = pos;
    m_cachedRadius = radius;

    if (radius <= m_sphereRadius) {
        narrowPolygon_EachBlock(searchBlock(pos));
    }

    *m_prismCacheIter = 0;
}

/// @addr{0x807C1F80}
/// @brief Checks for a collision at a specific point. If the previous position is valid, then
/// it filters only to prisms for which the kart is traveling in the direction of the prism's
/// face normal.
/// @param distOut Output parameter for the distance to the collision point
/// @param fnrmOut Output parameter for the face normal at the collision point
/// @param flagsOut Output parameter for the collision flags
/// @return `true` if a collision was detected, `false` otherwise
bool KColData::checkPointCollision(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut) {
    return checkPoint(distOut, fnrmOut, flagsOut, std::isfinite(m_prevPos.y));
}

/// @addr{0x807C2410}
/// @brief Checks for a collision with a sphere. If the previous position is valid, then
/// it filters only to prisms for which the kart is traveling in the direction of the prism's
/// face normal.
/// @param distOut Output parameter for the distance to the collision point
/// @param fnrmOut Output parameter for the face normal at the collision point
/// @param flagsOut Output parameter for the collision flags
/// @return `true` if a collision was detected, `false` otherwise
bool KColData::checkSphereCollision(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut) {
    return std::isfinite(m_prevPos.y) ?
            checkSphereImpl<CollisionCheckType::Movement>(distOut, fnrmOut, flagsOut) :
            checkSphereImpl<CollisionCheckType::Plane>(distOut, fnrmOut, flagsOut);
}

/// @addr{0x807C1B0C}
/// @brief Sets members in preparation of a subsequent point collision check call
/// @param pos The position of the point to search around
/// @param prevPos The previous position of the point
/// @param typeMask The type mask to filter which prisms to consider for collision
void KColData::lookupPoint(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask typeMask) {
    m_prismIter = searchBlock(pos);
    m_pos = pos;
    m_prevPos = prevPos;
    m_movement = pos - prevPos;
    m_typeMask = typeMask;
}

/// @addr{0x807C1BB4}
/// @brief Sets members in preparation of a subsequent sphere collision check call
/// @param radius The radius of the sphere to check for collisions
/// @param pos The position of the sphere to search around
/// @param prevPos The previous position of the sphere
/// @param typeMask The type mask to filter which prisms to consider for collision
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
/// @brief Performs a sphere collision query using the cached prism list if possible
/// @param radius The radius of the sphere
/// @param pos The current position of the sphere
/// @param prevPos The previous position of the sphere
/// @param typeMask The KCL types to filter the collision query to
/// @details The cache is used if the provided sphere (defined by `radius` and `pos`) lies within
/// the cached query's sphere. Otherwise, a new octree lookup is performed.
void KColData::lookupSphereCached(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, u32 typeMask) {
    EGG::Sphere3f sphere1(pos, radius);
    EGG::Sphere3f sphere2(m_cachedPos, m_cachedRadius);

    if (!sphere1.isInsideOtherSphere(sphere2)) {
        m_prismIter = searchBlock(pos);
        m_radius = std::min(m_sphereRadius, radius);
    } else {
        m_radius = radius;
        m_prismIter = m_prismCache.data() - 1;
    }

    m_pos = pos;
    m_prevPos = prevPos;
    m_movement = pos - prevPos;
    m_typeMask = typeMask;
}

/// @brief Resolves each prism's vertex, height, and normal indices into a contiguous struct
/// @details OPT: The use of this function avoids having to byteswap and index several different
/// arrays to fetch a prism's geometry. Memory cost is 64 bytes per prism for the geometry plus 2
/// bytes per prism for the attribute, which is at worst ~690KB (for Maple Treeway).
void KColData::preloadPrismGeometry() {
    size_t prismCount =
            (reinterpret_cast<uintptr_t>(m_blockData) - reinterpret_cast<uintptr_t>(m_prismData)) /
            sizeof(KCollisionPrism);

    // Prisms are one-indexed, so the normal data runs up to the (unused) zeroth prism
    size_t nrmSize = reinterpret_cast<uintptr_t>(m_prismData) + sizeof(KCollisionPrism) -
            reinterpret_cast<uintptr_t>(m_nrmData);
    size_t posSize =
            reinterpret_cast<uintptr_t>(m_nrmData) - reinterpret_cast<uintptr_t>(m_posData);

    EGG::RamStream prismStream = EGG::RamStream(m_prismData, sizeof(KCollisionPrism) * prismCount);
    EGG::RamStream nrmStream = EGG::RamStream(m_nrmData, nrmSize);
    EGG::RamStream posStream = EGG::RamStream(m_posData, posSize);

    auto readVec = [](EGG::RamStream &stream, u16 idx) {
        EGG::Vector3f vec;
        stream.jump(idx * sizeof(EGG::Vector3f));
        vec.read(stream);
        return vec;
    };

    m_prismGeometry = owning_span<KCollisionPrismGeometry>(prismCount);
    m_prismAttributes = owning_span<u16>(prismCount);

    // Prisms are one-indexed, so the first entry is left empty
    prismStream.skip(sizeof(KCollisionPrism));

    for (size_t i = 1; i < prismCount; ++i) {
        KCollisionPrism prism;
        prism.read(prismStream);
        m_prismAttributes[i] = prism.attribute;

        KCollisionPrismGeometry &geom = m_prismGeometry[i];
        geom.height = prism.height;
        geom.vtx = readVec(posStream, prism.pos_i);
        geom.fnrm = readVec(nrmStream, prism.fnrm_i);
        geom.enrm1 = readVec(nrmStream, prism.enrm1_i);
        geom.enrm2 = readVec(nrmStream, prism.enrm2_i);
        geom.enrm3 = readVec(nrmStream, prism.enrm3_i);
    }
}

/// @addr{0x807BDDFC}
/// @brief Calculates a @ref EGG::BoundBox3f describing the boundary of all prisms in the KCL file
/// @details The bounding box is constructed by iterating every prism and expanding the bounding box
/// to include all vertices of each prism.
void KColData::computeBBox() {
    m_bbox.max.set(-999999.0f);
    m_bbox.min.set(999999.0f);

    for (size_t i = 1; i < m_prismGeometry.size(); ++i) {
        const KCollisionPrismGeometry &geom = m_prismGeometry[i];
        const EGG::Vector3f &vtx1 = geom.vtx;

        const EGG::Vector3f vtx2 = GetVertex(geom.height, vtx1, geom.fnrm, geom.enrm3, geom.enrm1);
        const EGG::Vector3f vtx3 = GetVertex(geom.height, vtx1, geom.fnrm, geom.enrm3, geom.enrm2);

        m_bbox.min = m_bbox.min.minimize(vtx1);
        m_bbox.min = m_bbox.min.minimize(vtx2);
        m_bbox.min = m_bbox.min.minimize(vtx3);
        m_bbox.max = m_bbox.max.maximize(vtx1);
        m_bbox.max = m_bbox.max.maximize(vtx2);
        m_bbox.max = m_bbox.max.maximize(vtx3);
    }
}

/// @addr{0x807C243C}
/// @brief Iteratively performs collision checks against each prism in the provided array, caching
/// the colliding prisms' indices to @ref m_prismCache
/// @param prismArray The array of prism indices in the octree to check for collisions
void KColData::narrowPolygon_EachBlock(const u16 *prismArray) {
    m_prismIter = prismArray;

    while (checkSphereSingle(nullptr, nullptr, nullptr)) {
        // We assume the cache has same endianness as the archive file,
        // so do not parse out the prism index and directly store it in the cache.
        *(m_prismCacheIter++) = *m_prismIter;

        if (m_prismCacheIter == m_prismCache.data() + m_prismCache.size()) {
            --m_prismCacheIter;
            return;
        }
    }
}

/// @addr{0x807BE030}
/// @brief Traverses the octree to find the leaf node containing the provided position
/// @param pos The position to search for within the octree
/// @return A pointer to the leaf node containing the input position
const u16 *KColData::searchBlock(const EGG::Vector3f &pos) {
    // Calculate the x, y, and z offsets of the point from the minimum
    // corner of the tree's bounding box.
    const int x = pos.x - m_areaMinPos.x;
    const int y = pos.y - m_areaMinPos.y;
    const int z = pos.z - m_areaMinPos.z;

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

/// @addr{0x807C0F00}
/// @brief Iterates the list of nearby triangles, skipping prisms that are in the cache, to see if
/// we are colliding with any edge
/// @param distOut If colliding, outputs the distance between the player and the triangle
/// @param fnrmOut If colliding, outputs the floor normal of the triangle
/// @param flagsOut If colliding, outputs the KCL attributes for that triangle
/// @return `true` if the player is colliding with an edge of a triangle, `false` otherwise
bool KColData::checkSphereSingle(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut) {
    if (!m_prismIter) {
        return false;
    }

    while (*++m_prismIter != 0) {
        if (m_prismCacheIter != m_prismCache.data()) {
            u16 *puVar10 = m_prismCacheIter - 1;
            while (*m_prismIter != *puVar10) {
                if (puVar10-- < m_prismCache.data()) {
                    break;
                }
            }

            if (puVar10 >= m_prismCache.data()) {
                continue;
            }
        }

        if (checkSphereCollision<CollisionCheckType::Edge>(parse<u16>(*m_prismIter), distOut,
                    fnrmOut, flagsOut)) {
            return true;
        }
    }

    m_prismIter = nullptr;
    return false;
}

/// @addr{0x807C21F4} @addr{0x807C1F80}
/// @brief Combination of two point collision check functions. Iterates the nearby triangles to see
/// if a point is colliding with any prism.
/// @param distOut If colliding, outputs the distance between the player and the triangle
/// @param fnrmOut If colliding, outputs the floor normal of the triangle
/// @param attributeOut If colliding, outputs the KCL attributes for that triangle
/// @param movement Whether to gate the collision on whether the point is driving in the direction
/// of the prism's face normal
/// @return `true` if the player is colliding with any triangle, `false` otherwise
bool KColData::checkPoint(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *attributeOut, bool movement) {
    // If there's no list of triangles to check, there's no collision
    if (!m_prismIter) {
        return false;
    }

    // Check collision for all triangles, and continuously call the function until we're out
    while (*++m_prismIter != 0) {
        if (checkPointCollision(parse<u16>(*m_prismIter), distOut, fnrmOut, attributeOut,
                    movement)) {
            return true;
        }
    }

    // We're out of triangles to check - another list must be prepared for subsequent calls
    m_prismIter = nullptr;
    return false;
}

/// @brief This is a combination of two point collision check functions that check whether a point
/// is colliding with any prism. They only vary based on whether we are checking movement.
/// @param prismIdx The index of the collision prism to check against
/// @param distOut Output parameter for the distance to the collision point, if colliding
/// @param fnrmOut Output parameter for the prism's face normal, if colliding
/// @param flagsOut Output parameter for the KCL flags of the prism, if colliding
/// @param movement Whether to gate the collision on whether the point is driving in the direction
/// of the prism's face normal
/// @return `true` if a collision occurred, `false` otherwise
/// @details First checks to see whether the prism at `prismIdx` has any flags matching the query
/// type mask @ref m_typeMask. If not, early returns `false`. This function uses a pseudo-radius of
/// `0.01f` for determining whether a given point has collided with the prism. Thus, if the point is
/// not within `0.01f` of any edge or the face of the prism, returns `false`. If the point (with its
/// pseudo-radius) lies beyond @ref m_prismThickness from the prism's face, returns `false`.
/// Finally, if the point is moving and the prism has directional flags, checks whether the point's
/// movement is against the prism's face normal and returns `false` if so. Otherwise, returns `true`
/// and updates the output parameters with the collision information.
bool KColData::checkPointCollision(u16 prismIdx, f32 *distOut, EGG::Vector3f *fnrmOut,
        u16 *flagsOut, bool movement) {
    const u16 attribute = m_prismAttributes[prismIdx];
    KCLTypeMask attrMask = KCL_ATTRIBUTE_TYPE_BIT(attribute);
    if (!(attrMask & m_typeMask)) {
        return false;
    }

    const KCollisionPrismGeometry &geom = m_prismGeometry[prismIdx];
    const EGG::Vector3f relativePos = m_pos - geom.vtx;

    // For Kinoko, we check the plane distance first since this is a more common early return than
    // the edge normal checks.
    const EGG::Vector3f &fnrm = geom.fnrm;
    f32 plane_dist = relativePos.ps_dot(fnrm);
    f32 dist_in_plane = 0.01f - plane_dist;
    if (dist_in_plane <= 0.0f) {
        return false;
    }

    const EGG::Vector3f &enrm1 = geom.enrm1;
    f32 dist_ca = relativePos.ps_dot(enrm1);
    if (dist_ca >= 0.01f) {
        return false;
    }

    const EGG::Vector3f &enrm2 = geom.enrm2;
    f32 dist_ab = relativePos.ps_dot(enrm2);
    if (dist_ab >= 0.01f) {
        return false;
    }

    const EGG::Vector3f &enrm3 = geom.enrm3;
    f32 dist_bc = relativePos.ps_dot(enrm3) - geom.height;
    if (dist_bc >= 0.01f) {
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
        *flagsOut = attribute;
    }

    return true;
}

/// @addr{0x807C1514} @addr{0x807C0884}
/// @brief Combination of two sphere collision check functions. Iterates the list of nearby
/// triangles to see if a sphere is colliding with any plane, optionally gated on whether we are
/// moving towards the face of the prism.
/// @tparam Type Either @ref CollisionCheckType::Plane or @ref CollisionCheckType::Movement
/// @param distOut If colliding, outputs the distance between the player and the triangle
/// @param fnrmOut If colliding, outputs the floor normal of the triangle
/// @param flagsOut If colliding, outputs the KCL attributes for that triangle
/// @return `true` if the player is colliding with the plane of a triangle, `false` otherwise
template <KColData::CollisionCheckType Type>
    requires(Type == KColData::CollisionCheckType::Plane ||
            Type == KColData::CollisionCheckType::Movement)
bool KColData::checkSphereImpl(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut) {
    // If there's no list of triangles to check, there's no collision
    if (!m_prismIter) {
        return false;
    }

    // Check collision for all triangles, and continuously call the function until we're out
    while (*++m_prismIter != 0) {
        if (checkSphereCollision<Type>(parse<u16>(*m_prismIter), distOut, fnrmOut, flagsOut)) {
            return true;
        }
    }

    // We're out of triangles to check - another list must be prepared for subsequent calls
    m_prismIter = nullptr;
    return false;
}

/// @brief This is a combination of the three low-level sphere collision checks in the base game
/// @tparam Type The type of collision check to perform (@ref CollisionCheckType::Edge,
/// @ref CollisionCheckType::Plane, or @ref CollisionCheckType::Movement)
/// @param prismIdx The index of the prism to check for collision against
/// @param distOut Output parameter for the distance to the collision point, if colliding
/// @param fnrmOut Output parameter for the prism's face normal, if colliding
/// @param flagsOut Output parameter for the KCL flags of the prism, if colliding
/// @return `true` if a collision is detected, `false` otherwise
/// @details The checks vary only by a few if-statements, related to whether we are checking for:
/// 1. A collision with at least the triangle edge (0x807C0F00)
/// 2. A collision with the triangle plane (0x807C1514)
/// 3. A collision such that we are inside the triangle (0x807C0884)
///
/// First checks to see if the prism at `prismIdx` has any flags enabled that match the query mask
/// @ref m_typeMask. If not, early returns `false`. This check normally occurs later in the
/// function's codeflow, but we take creative liberty to move this check earlier as a performance
/// optimization.
///
/// If the query's @ref m_radius and @ref m_pos lie outside of the prism's bounding sphere, early
/// returns `false`. If `Type` is `CollisionCheckType::Edge`, then checks to see if the sphere is at
/// least colliding with the edge of the prism, otherwise checks to see if the sphere is colliding
/// with the inside of the prism; if these checks fail, returns `false`. If `Type` is
/// `CollisionCheckType::Movement`, the prism has any @ref KCL_TYPE_DIRECTIONAL flags set, and the
/// sphere is moving away from the direction of the prism's face normal, then returns `false`.
///
/// After computing the distance between the sphere and each edge of the prism, first checks to see
/// if each distance is negative, which indicates that the sphere is entirely inside the prism. If
/// this is the case, then there is an additional check performed when `Type` is
/// `CollisionCheckType::Movement` which requires that the sphere is moving towards the prism's
/// face. If so, then returns `true`, setting the output parameters accordingly. This check normally
/// occurs later in the base game's function, but we move it earlier for Kinoko as a performance
/// improvement.
///
/// Next, finds the two edges of the prism that the sphere has passed the furthest through. Then,
/// checks to see whether the sphere is closest to one of these edges or the corner of these edges
/// and computes the squared distance to the respective point. Along the way, if `Type` is
/// `CollisionCheckType::Plane` and the distance to the edge/corner exceeds the distance between the
/// sphere and the prism's face plane, then returns `false`.
///
/// With the computed distance to the edge/corner, checks to see if the distance exceeds the
/// sphere's radius. If it does, returns `false`. Lastly, if `Type` is
/// `CollisionCheckType::Movement`, requires that the sphere is moving towards the prism's face,
/// otherwise returns `false`. With all checks passed, returns `true`, updating the output
/// parameters accordingly.
template <KColData::CollisionCheckType Type>
bool KColData::checkSphereCollision(u16 prismIdx, f32 *distOut, EGG::Vector3f *fnrmOut,
        u16 *flagsOut) {
    const u16 attribute = m_prismAttributes[prismIdx];
    const KCollisionPrismGeometry &geom = m_prismGeometry[prismIdx];

    // Responsible for updating the output params
    auto out = [&](f32 dist) {
        if (distOut) {
            *distOut = dist;
        }
        if (fnrmOut) {
            *fnrmOut = geom.fnrm;
        }
        if (flagsOut) {
            *flagsOut = attribute;
        }
        return true;
    };

    // The flag check occurs earlier than in the base game here. We don't want to do math if the tri
    // we're checking doesn't have matching flags.
    u32 attributeMask = KCL_ATTRIBUTE_TYPE_BIT(attribute);
    if (!(attributeMask & m_typeMask)) {
        return false;
    }

    const EGG::Vector3f relativePos = m_pos - geom.vtx;

    // For Kinoko, we check the plane distance first since this is a more common early return than
    // the edge normal checks.
    const EGG::Vector3f &fnrm = geom.fnrm;
    f32 plane_dist = relativePos.ps_dot(fnrm);
    f32 dist_in_plane = m_radius - plane_dist;
    if (dist_in_plane <= 0.0f) {
        return false;
    }

    // Edge normals point outside the triangle
    const EGG::Vector3f &enrm1 = geom.enrm1;
    f32 dist_ca = relativePos.ps_dot(enrm1);
    if (m_radius <= dist_ca) {
        return false;
    }

    const EGG::Vector3f &enrm2 = geom.enrm2;
    f32 dist_ab = relativePos.ps_dot(enrm2);
    if (m_radius <= dist_ab) {
        return false;
    }

    const EGG::Vector3f &enrm3 = geom.enrm3;
    f32 dist_bc = relativePos.ps_dot(enrm3) - geom.height;
    if (m_radius <= dist_bc) {
        return false;
    }

    f32 typeDistance = m_prismThickness;
    if constexpr (Type == CollisionCheckType::Edge) {
        typeDistance += m_radius;
    }

    if (dist_in_plane >= typeDistance) {
        return false;
    }

    if constexpr (Type == CollisionCheckType::Movement) {
        if (attributeMask & KCL_TYPE_DIRECTIONAL && m_movement.dot(fnrm) > 0.0f) {
            return false;
        }
    }

    // Originally part of the edge searching, but moved out for simplicity
    // If these are all zero, then we're inside the triangle
    if (dist_ab <= 0.0f && dist_bc <= 0.0f && dist_ca <= 0.0f) {
        if constexpr (Type == CollisionCheckType::Movement) {
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
        if constexpr (Type == CollisionCheckType::Plane) {
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
        if constexpr (Type == CollisionCheckType::Plane) {
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

    if constexpr (Type == CollisionCheckType::Movement) {
        EGG::Vector3f lastPos = relativePos - m_movement;
        // We're only colliding if we are moving towards the face
        if (lastPos.ps_dot(fnrm) < 0.0f) {
            return false;
        }
    }

    return out(dist);
}

/// @addr{0x807BDF54}
/// @brief Computes a prism vertex based off of the triangle's normal vectors
/// @param height The height of the prism
/// @param vertex1 The first vertex of the triangle
/// @param fnrm The face normal of the triangle
/// @param enrm3 The edge normal opposite to the third vertex
/// @param enrm The edge normal opposite to the second vertex
/// @return The computed vertex position based on the given parameters
/// @details @par Triangle Vertices Formula
/// Given a triangle with vertices \f$\vec{A}, \vec{B}, \vec{C}\f$, face normal \f$\hat{f}\f$,
/// and height \f$h\f$, label the edge normals by: \f{aligned}{\hat{en}_1 := e_{AB}, \,\,
/// \hat{en}_2:= e_{AC}, \,\,\hat{en}_3:=e_{BC}}\f} We can recover \f$\vec{B},
/// \vec{C}\f$ via: \f{aligned}{ \vec{B} = \vec{A} + \dfrac{h}{(\hat{en}_2 \times \hat{f})
/// \cdot \hat{en}_3}\left(\hat{en}_2 \times \hat{f}\right), \, \, \vec{C} = \vec{A} +
/// \dfrac{h}{(\hat{en}_1 \times \hat{f}) \cdot \hat{en}_3}(\hat{en}_1 \times \hat{f}) \, .
/// }\f}
EGG::Vector3f KColData::GetVertex(f32 height, const EGG::Vector3f &vertex1,
        const EGG::Vector3f &fnrm, const EGG::Vector3f &enrm3, const EGG::Vector3f &enrm) {
    EGG::Vector3f cross = fnrm.cross(enrm);
    f32 dp = cross.ps_dot(enrm3);
    cross *= (height / dp);

    return cross + vertex1;
}

} // namespace Kinoko::Field
