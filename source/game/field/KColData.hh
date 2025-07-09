#pragma once

#include "game/field/KCollisionTypes.hh"

#include <egg/math/BoundBox.hh>
#include <egg/math/Matrix.hh>

// Credit: em-eight/mkw
// Credit: stblr/Hanachan

namespace Field {

struct CollisionInfoPartial {
    EGG::BoundBox3f bbox;
    EGG::Vector3f tangentOff;

    void update(const EGG::Vector3f &offset) {
        bbox.min = bbox.min.minimize(offset);
        bbox.max = bbox.max.maximize(offset);
    }
};

struct CollisionInfo {
    EGG::BoundBox3f bbox;
    EGG::Vector3f tangentOff;
    EGG::Vector3f floorNrm;
    EGG::Vector3f wallNrm;
    EGG::Vector3f roadVelocity;
    f32 floorDist;
    f32 wallDist;
    f32 movingFloorDist;
    f32 perpendicularity;

    void updateFloor(f32 dist, const EGG::Vector3f &fnrm) {
        if (dist > floorDist) {
            floorDist = dist;
            floorNrm = fnrm;
        }
    }

    void updateWall(f32 dist, const EGG::Vector3f &fnrm) {
        if (dist > wallDist) {
            wallDist = dist;
            wallNrm = fnrm;
        }
    }

    void reset() {
        bbox.setZero();
        movingFloorDist = -std::numeric_limits<f32>::min();
        wallDist = -std::numeric_limits<f32>::min();
        floorDist = -std::numeric_limits<f32>::min();
        perpendicularity = 0.0f;
    }

    void update(f32 now_dist, const EGG::Vector3f &offset, const EGG::Vector3f &fnrm,
            u32 kclAttributeTypeBit);
    void transformInfo(CollisionInfo &rhs, const EGG::Matrix34f &mtx, const EGG::Vector3f &v);
};

/// @brief Performs lookups for KCL triangles
/// @nosubgrouping
class KColData {
public:
    enum class CollisionCheckType {
        Edge,
        Plane,
        Movement,
    };

    struct KCollisionPrism {
        KCollisionPrism();
        KCollisionPrism(f32 height, u16 posIndex, u16 faceNormIndex, u16 edge1NormIndex,
                u16 edge2NormIndex, u16 edge3NormIndex, u16 attribute);

        f32 height;
        u16 pos_i;
        u16 fnrm_i;
        u16 enrm1_i;
        u16 enrm2_i;
        u16 enrm3_i;
        u16 attribute;
    };
    STATIC_ASSERT(sizeof(KCollisionPrism) == 0x10);

    KColData(const void *file);
    ~KColData();

    void narrowScopeLocal(const EGG::Vector3f &pos, f32 radius, KCLTypeMask mask);
    void narrowPolygon_EachBlock(const u16 *prismArray);

    void computeBBox();
    [[nodiscard]] bool checkPointCollision(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);
    [[nodiscard]] bool checkSphereCollision(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);
    [[nodiscard]] bool checkSphere(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);
    [[nodiscard]] bool checkSphereSingle(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);

    void lookupPoint(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask typeMask);
    void lookupSphere(f32 radius, const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask typeMask);
    void lookupSphereCached(const EGG::Vector3f &p1, const EGG::Vector3f &p2, u32 typeMask,
            f32 radius);

    [[nodiscard]] const u16 *searchBlock(const EGG::Vector3f &pos);

    /// @beginGetters
    [[nodiscard]] const EGG::BoundBox3f &bbox() const {
        return m_bbox;
    }

    [[nodiscard]] u16 prismCache(u32 idx) const {
        return m_prismCache[idx];
    }
    /// @endGetters

    [[nodiscard]] static EGG::Vector3f GetVertex(f32 height, const EGG::Vector3f &vertex1,
            const EGG::Vector3f &fnrm, const EGG::Vector3f &enrm3, const EGG::Vector3f &enrm);

private:
    void preloadPrisms();
    void preloadNormals();
    void preloadVertices();

    [[nodiscard]] bool checkCollision(const KCollisionPrism &prism, f32 *distOut,
            EGG::Vector3f *fnrmOut, u16 *flagsOut, CollisionCheckType type);
    [[nodiscard]] bool checkPointCollision(const KCollisionPrism &prism, f32 *distOut,
            EGG::Vector3f *fnrmOut, u16 *flagsOut, bool movement);
    [[nodiscard]] bool checkSphereMovement(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *attributeOut);
    [[nodiscard]] bool checkPointMovement(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *attributeOut);
    [[nodiscard]] bool checkPoint(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *attributeOut);

    const void *m_posData;
    const void *m_nrmData;
    const void *m_prismData;
    const void *m_blockData;
    f32 m_prismThickness;
    EGG::Vector3f m_areaMinPos;
    u32 m_areaXWidthMask;    ///< The x dimension of the octree's bounding box. @see searchBlock.
    u32 m_areaYWidthMask;    ///< The y dimension of the octree's bounding box. @see searchBlock.
    u32 m_areaZWidthMask;    ///< The z dimension of the octree's bounding box. @see searchBlock.
    u32 m_blockWidthShift;   ///< Used to initialize octree navigation. @see searchBlock.
    u32 m_areaXBlocksShift;  ///< Used to initialize octree navigation. @see searchBlock.
    u32 m_areaXYBlocksShift; ///< Used to initialize octree navigation. @see searchBlock.
    f32 m_sphereRadius;      ///< Clamps the sphere we check collision against. @see searchBlock.
    EGG::Vector3f m_pos;
    EGG::Vector3f m_prevPos;
    EGG::Vector3f m_movement;
    f32 m_radius;
    KCLTypeMask m_typeMask;
    const u16 *m_prismIter;
    EGG::BoundBox3f m_bbox;
    std::array<u16, 256> m_prismCache;
    u16 *m_prismCacheTop;
    u16 *m_cachedPrismArray;
    EGG::Vector3f m_cachedPos;
    f32 m_cachedRadius;

    /// @brief Optimizes for time by avoiding unnecessary byteswapping.
    /// The Wii doesn't have this problem because big endian is always assumed.
    std::span<KCollisionPrism> m_prisms;
    std::span<EGG::Vector3f> m_nrms;
    std::span<EGG::Vector3f> m_vertices;
};

} // namespace Field
