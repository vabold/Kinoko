#pragma once

#include "game/field/KColData.hh"

#include <egg/math/BoundBox.hh>
#include <egg/math/Matrix.hh>

// Credit: em-eight/mkw

class SavestateManager;

namespace Field {

typedef bool (
        KColData::*CollisionCheckFunc)(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *attributeOut);

/// @brief Manager for course KCL interactions.
/// @addr{0x809C3C10}
/// @nosubgrouping
class CourseColMgr : EGG::Disposer {
    friend class ::SavestateManager;

public:
    struct NoBounceWallColInfo {
        EGG::BoundBox3f bbox;
        EGG::Vector3f tangentOff;
        f32 dist;
        EGG::Vector3f fnrm;
    };
    STATIC_ASSERT(sizeof(NoBounceWallColInfo) == 0x34);

    void init();

    void scaledNarrowScopeLocal(f32 scale, f32 radius, KColData *data, const EGG::Vector3f &pos,
            KCLTypeMask mask);

    [[nodiscard]] bool checkPointPartial(f32 scale, KColData *data, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut);
    [[nodiscard]] bool checkPointPartialPush(f32 scale, KColData *data, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut);
    [[nodiscard]] bool checkPointFull(f32 kclScale, KColData *data, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut);
    [[nodiscard]] bool checkPointFullPush(f32 kclScale, KColData *data, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut);

    [[nodiscard]] bool checkSpherePartial(f32 scale, f32 radius, KColData *data,
            const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask,
            CollisionInfoPartial *info, KCLTypeMask *maskOut);
    [[nodiscard]] bool checkSpherePartialPush(f32 scale, f32 radius, KColData *data,
            const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask,
            CollisionInfoPartial *info, KCLTypeMask *maskOut);
    [[nodiscard]] bool checkSphereFull(f32 scale, f32 radius, KColData *data,
            const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask,
            CollisionInfo *info, KCLTypeMask *maskOut);
    [[nodiscard]] bool checkSphereFullPush(f32 scale, f32 radius, KColData *data,
            const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask,
            CollisionInfo *info, KCLTypeMask *maskOut);

    [[nodiscard]] bool checkPointCachedPartial(f32 scale, KColData *data, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut);
    [[nodiscard]] bool checkPointCachedPartialPush(f32 scale, KColData *data,
            const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask,
            CollisionInfoPartial *info, KCLTypeMask *maskOut);
    [[nodiscard]] bool checkPointCachedFull(f32 scale, KColData *data, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut);
    [[nodiscard]] bool checkPointCachedFullPush(f32 scale, KColData *data, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut);

    [[nodiscard]] bool checkSphereCachedPartial(f32 scale, f32 radius, KColData *data,
            const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask typeMask,
            CollisionInfoPartial *info, KCLTypeMask *maskOut);
    [[nodiscard]] bool checkSphereCachedPartialPush(f32 scale, f32 radius, KColData *data,
            const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask typeMask,
            CollisionInfoPartial *info, KCLTypeMask *maskOut);
    [[nodiscard]] bool checkSphereCachedFull(f32 scale, f32 radius, KColData *data,
            const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask typeMask,
            CollisionInfo *colInfo, KCLTypeMask *maskOut);
    [[nodiscard]] bool checkSphereCachedFullPush(f32 scale, f32 radius, KColData *data,
            const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask typeMask,
            CollisionInfo *colInfo, KCLTypeMask *maskOut);

    /// @beginSetters
    void setNoBounceWallInfo(NoBounceWallColInfo *info) {
        m_noBounceWallInfo = info;
    }

    void clearNoBounceWallInfo() {
        m_noBounceWallInfo = nullptr;
    }

    void setLocalMtx(EGG::Matrix34f *mtx) {
        m_localMtx = mtx;
    }
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] NoBounceWallColInfo *noBounceWallInfo() const {
        return m_noBounceWallInfo;
    }
    /// @endGetters

    static void *LoadFile(const char *filename);

    static CourseColMgr *CreateInstance();
    static void DestroyInstance();

    [[nodiscard]] static CourseColMgr *Instance() {
        return s_instance;
    }

private:
    CourseColMgr();
    ~CourseColMgr() override;

    [[nodiscard]] bool doCheckWithPartialInfo(KColData *data, CollisionCheckFunc collisionCheckFunc,
            CollisionInfoPartial *info, KCLTypeMask *typeMask);
    [[nodiscard]] bool doCheckWithPartialInfoPush(KColData *data,
            CollisionCheckFunc collisionCheckFunc, CollisionInfoPartial *info,
            KCLTypeMask *typeMask);
    [[nodiscard]] bool doCheckWithFullInfo(KColData *data, CollisionCheckFunc collisionCheckFunc,
            CollisionInfo *colInfo, KCLTypeMask *flagsOut);
    [[nodiscard]] bool doCheckWithFullInfoPush(KColData *data,
            CollisionCheckFunc collisionCheckFunc, CollisionInfo *colInfo, KCLTypeMask *flagsOut);
    [[nodiscard]] bool doCheckMaskOnly(KColData *data, CollisionCheckFunc collisionCheckFunc,
            KCLTypeMask *maskOut);
    [[nodiscard]] bool doCheckMaskOnlyPush(KColData *data, CollisionCheckFunc collisionCheckFunc,
            KCLTypeMask *maskOut);

    KColData *m_data;
    f32 m_kclScale;
    NoBounceWallColInfo *m_noBounceWallInfo;
    EGG::Matrix34f *m_localMtx;

    static CourseColMgr *s_instance; ///< @addr{0x809C3C10}
};

} // namespace Field
