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
        EGG::BoundBox3f m_bbox;
        EGG::Vector3f m_minPlusMax;
        EGG::Vector3f m_floorNrm;
        EGG::Vector3f m_wallNrm;
        EGG::Vector3f _3c;
        f32 m_floorDist;
        f32 m_wallDist;
        f32 _50;
        f32 m_perpendicularity;
        void *astruct_7; // TODO

        void updateFloor(f32 dist, const EGG::Vector3f &fnrm);
        void updateWall(f32 dist, const EGG::Vector3f &fnrm);
        void update(f32 now_dist, const EGG::Vector3f &offset, const EGG::Vector3f &fnrm,
                u32 kclAttributeTypeBit);
    };

    struct NoBounceWallColInfo {
        EGG::Vector3f m_bboxLow;
        EGG::Vector3f m_bboxHigh;
        EGG::Vector3f m_lowPlusHigh; // bboxLow + bboxHigh, see 0x805998c0
        f32 m_dist;
        EGG::Vector3f m_fnrm;
    };
    static_assert(sizeof(NoBounceWallColInfo) == 0x34);

    void init();

    bool checkSphereFullPush(f32 scalar, f32 radius, KColData *data, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *info,
            KCLTypeMask *kcl_flags_out);

    static void *LoadFile(const char *filename);

    static CourseColMgr *CreateInstance();
    static void DestroyInstance();
    static CourseColMgr *Instance();

private:
    CourseColMgr();
    ~CourseColMgr();

    bool doCheckWithFullInfoPush(KColData *colMgr, CollisionCheckFunc collisionCheckFunc,
            CollisionInfo *colInfo, u32 *flagsOut);

    KColData *m_data;
    f32 m_kclScale;
    NoBounceWallColInfo *m_noBounceWallInfo;
    EGG::Matrix34f *m_localMtx;

    static CourseColMgr *s_instance;
};

} // namespace Field
