#pragma once

#include "game/field/KCollisionTypes.hh"

#include <egg/math/BoundBox.hh>

// Credit: em-eight/mkw
// Credit: stblr/Hanachan

namespace Field {

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

    void narrowScopeLocal(const EGG::Vector3f &pos, f32 radius, KCLTypeMask mask);
    void narrowPolygon_EachBlock(const u16 *prismArray);

    void computeBBox();
    [[nodiscard]] bool checkSphereCollision(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);
    [[nodiscard]] bool checkSphere(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);
    [[nodiscard]] bool checkSphereSingle(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);

    void lookupSphere(f32 radius, const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask typeMask);
    void lookupSphereCached(const EGG::Vector3f &p1, const EGG::Vector3f &p2, u32 typeMask,
            f32 radius);

    [[nodiscard]] const u16 *searchBlock(const EGG::Vector3f &pos);

    [[nodiscard]] EGG::Vector3f getPos(u16 posIdx) const;
    [[nodiscard]] EGG::Vector3f getNrm(u16 nrmIdx) const;
    [[nodiscard]] KCollisionPrism getPrism(u16 prismIdx) const;

    /// @beginGetters
    [[nodiscard]] u16 prismCache(u32 idx) const;
    /// @endGetters

    [[nodiscard]] static EGG::Vector3f GetVertex(f32 height, const EGG::Vector3f &vertex1,
            const EGG::Vector3f &fnrm, const EGG::Vector3f &enrm3, const EGG::Vector3f &enrm);

private:
    [[nodiscard]] bool checkCollision(const KCollisionPrism &prism, f32 *distOut,
            EGG::Vector3f *fnrmOut, u16 *flagsOut, CollisionCheckType type);
    [[nodiscard]] bool checkSphereMovement(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *attributeOut);

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
    u32 m_prismCount;
    const KCollisionPrism *m_prisms;
    EGG::BoundBox3f m_bbox;
    std::array<u16, 256> m_prismCache;
    u16 *m_prismCacheTop;
    u16 *m_cachedPrismArray;
    EGG::Vector3f m_cachedPos;
    f32 m_cachedRadius;
};

} // namespace Field
