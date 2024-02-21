#pragma once

#include "game/field/KColData.hh"

#include <egg/math/BoundBox.hh>
#include <egg/math/Matrix.hh>

// Credit: em-eight/mkw

namespace Field {

typedef bool (
        KColData::*CollisionCheckFunc)(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *attributeOut);

class CourseColMgr {
public:
    struct CollisionInfo {
        EGG::BoundBox3f bbox;
        EGG::Vector3f tangentOff;
        EGG::Vector3f floorNrm;
        EGG::Vector3f wallNrm;
        EGG::Vector3f _3c;
        f32 floorDist;
        f32 wallDist;
        f32 _50;
        f32 perpendicularity;

        void updateFloor(f32 dist, const EGG::Vector3f &fnrm);
        void updateWall(f32 dist, const EGG::Vector3f &fnrm);
        void update(f32 now_dist, const EGG::Vector3f &offset, const EGG::Vector3f &fnrm,
                u32 kclAttributeTypeBit);
    };

    struct NoBounceWallColInfo {
        EGG::Vector3f bboxLow;
        EGG::Vector3f bboxHigh;
        EGG::Vector3f lowPlusHigh; // bboxLow + bboxHigh, see 0x805998c0
        f32 dist;
        EGG::Vector3f fnrm;
    };
    static_assert(sizeof(NoBounceWallColInfo) == 0x34);

    void init();

    void scaledNarrowScopeLocal(f32 scale, f32 radius, KColData *colMgr, const EGG::Vector3f &pos,
            KCLTypeMask mask);

    bool checkSphereFullPush(f32 scalar, f32 radius, KColData *colMgr, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *info,
            KCLTypeMask *kcl_flags_out);

    bool checkSphereCachedPartialPush(KColData *colMgr, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfo *colInfo,
            KCLTypeMask *typeMaskOut, f32 scale, f32 radius);
    bool checkSphereCachedFullPush(KColData *colMgr, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfo *colInfo,
            KCLTypeMask *typeMaskOut, f32 scale, f32 radius);

    static void *LoadFile(const char *filename);

    static CourseColMgr *CreateInstance();
    static void DestroyInstance();
    static CourseColMgr *Instance();

private:
    CourseColMgr();
    ~CourseColMgr();

    bool doCheckWithPartialInfoPush(KColData *colMgr, CollisionCheckFunc collisionCheckFunc,
            CollisionInfo *colInfo, KCLTypeMask *typeMask);
    bool doCheckWithFullInfoPush(KColData *colMgr, CollisionCheckFunc collisionCheckFunc,
            CollisionInfo *colInfo, KCLTypeMask *flagsOut);
    bool doCheckMaskOnlyPush(KColData *colMgr, CollisionCheckFunc collisionCheckFunc,
            KCLTypeMask *typeMaskOut);

    KColData *m_data;
    f32 m_kclScale;
    NoBounceWallColInfo *m_noBounceWallInfo;
    EGG::Matrix34f *m_localMtx;

    static CourseColMgr *s_instance;
};

} // namespace Field
