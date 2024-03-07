#pragma once

#include "game/field/KCollisionTypes.hh"

#include <egg/math/BoundBox.hh>

// Credit: em-eight/mkw
// Credit: stblr/Hanachan

namespace Field {

class KColData {
public:
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
    static_assert(sizeof(KCollisionPrism) == 0x10);

    KColData();
    KColData(const void *file);

    void narrowScopeLocal(const EGG::Vector3f &pos, f32 radius, KCLTypeMask mask);
    void narrowPolygon_EachBlock(const u16 *prismArray);

    void computeBBox();
    bool checkSphereCollision(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);
    bool checkSphere(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);
    bool checkSphereSingle(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);

    void lookupSphere(f32 radius, const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask typeMask);
    void lookupSphereCached(const EGG::Vector3f &p1, const EGG::Vector3f &p2, u32 typeMask,
            f32 radius);

    const u16 *searchBlock(const EGG::Vector3f &pos);

    EGG::Vector3f getPos(u16 posIdx) const;
    EGG::Vector3f getNrm(u16 nrmIdx) const;
    KCollisionPrism getPrism(u16 prismIdx) const;

    static EGG::Vector3f GetVertex(f32 height, const EGG::Vector3f &vertex1,
            const EGG::Vector3f &fnrm, const EGG::Vector3f &enrm3, const EGG::Vector3f &enrm);

    u16 prismCache(u32 idx) const;

private:
    bool checkSphereTriCollision(const KCollisionPrism &prism, f32 *distOut, EGG::Vector3f *fnrmOut,
            u16 *flagsOut);
    bool checkSphereMovementCollision(const KCollisionPrism &prism, f32 *distOut,
            EGG::Vector3f *fnrmOut, u16 *flagsOut);
    bool checkSphereMovement(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *attributeOut);

    const void *m_posData;
    const void *m_nrmData;
    const void *m_prismData;
    const void *m_blockData;
    f32 m_prismThickness;
    EGG::Vector3f m_areaMinPos;
    u32 m_areaXWidthMask;
    u32 m_areaYWidthMask;
    u32 m_areaZWidthMask;
    u32 m_blockWidthShift;
    u32 m_areaXBlocksShift;
    u32 m_areaXYBlocksShift;
    f32 m_sphereRadius;
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
