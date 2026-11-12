#pragma once

#include "game/field/CollisionInfo.hh"

#include <egg/math/BoundBox.hh>

// Credit: em-eight/mkw
// Credit: stblr/Hanachan

namespace Kinoko::Field {

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

    /// @brief Stores the vertex, height and normals of a collision prism
    /// @details We introduce this struct in Kinoko in order to pre-fetch a prism's geometry rather
    /// than accessing these fields from the various different arrays. This is a small performance
    /// improvement due to cache locality.
    struct alignas(0x40) KCollisionPrismGeometry {
        EGG::Vector3f vtx;   ///< The first vertex of the tri
        EGG::Vector3f enrm1; ///< The first edge's normal
        EGG::Vector3f enrm2; ///< The second edge's normal
        EGG::Vector3f enrm3; ///< The third edge's normal
        EGG::Vector3f fnrm;  ///< The face normal
        f32 height;          ///< The height of the tri
    };

    STATIC_ASSERT(sizeof(KCollisionPrismGeometry) == 0x40);

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
    void preloadPrismGeometry();

    template <CollisionCheckType Type>
    [[nodiscard]] bool checkCollision(u16 prismIdx, f32 *distOut, EGG::Vector3f *fnrmOut,
            u16 *flagsOut);

    [[nodiscard]] bool checkPointCollision(u16 prismIdx, f32 *distOut, EGG::Vector3f *fnrmOut,
            u16 *flagsOut, bool movement);
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

    /// @brief One-indexed array of each prism's geometry
    /// @details Kinoko-specific optimization for cache locality. See @ref KCollisionPrismGeometry.
    owning_span<KCollisionPrismGeometry> m_prismGeometry;

    /// @brief One-indexed array of each prism's KCL attribute
    /// @details Kept separate from @ref m_prismGeometry so that @ref KCollisionPrismGeometry can
    /// have better cache alignment.
    owning_span<u16> m_prismAttributes;
};

} // namespace Kinoko::Field
