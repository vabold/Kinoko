#pragma once

#include "game/field/KColData.hh"
#include "game/field/obj/ObjectDrivable.hh"

#include <egg/math/BoundBox.hh>
#include <egg/math/Matrix.hh>

// Credit: em-eight/mkw

namespace Field {

struct CollisionInfo;

typedef bool (
        KColData::*CollisionCheckFunc)(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *attributeOut);

/// @brief Manager for course KCL interactions.
/// @addr{0x809C3C10}
/// @nosubgrouping
class CourseColMgr : EGG::Disposer {
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

    [[nodiscard]] bool checkSphereFull(f32 scalar, f32 radius, KColData *data,
            const EGG::Vector3f &v0, const EGG::Vector3f &v1, KCLTypeMask flags,
            CollisionInfo *info, KCLTypeMask *kcl_flags_out);
    [[nodiscard]] bool checkSphereFullPush(f32 scalar, f32 radius, KColData *data,
            const EGG::Vector3f &v0, const EGG::Vector3f &v1, KCLTypeMask flags,
            CollisionInfo *info, KCLTypeMask *kcl_flags_out);

    [[nodiscard]] bool checkSphereCachedPartial(KColData *data, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfo *colInfo,
            KCLTypeMask *typeMaskOut, f32 scale, f32 radius);
    [[nodiscard]] bool checkSphereCachedPartialPush(KColData *data, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfo *colInfo,
            KCLTypeMask *typeMaskOut, f32 scale, f32 radius);
    [[nodiscard]] bool checkSphereCachedFullPush(KColData *data, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfo *colInfo,
            KCLTypeMask *typeMaskOut, f32 scale, f32 radius);

    /// @beginSetters
    void setNoBounceWallInfo(NoBounceWallColInfo *info);
    void clearNoBounceWallInfo();
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] NoBounceWallColInfo *noBounceWallInfo() const;
    /// @endGetters

    static void *LoadFile(const char *filename);

    static CourseColMgr *CreateInstance();
    static void DestroyInstance();
    [[nodiscard]] static CourseColMgr *Instance();

private:
    CourseColMgr();
    ~CourseColMgr() override;

    [[nodiscard]] bool doCheckWithPartialInfo(KColData *data, CollisionCheckFunc collisionCheckFunc,
            CollisionInfo *colInfo, KCLTypeMask *typeMask);
    [[nodiscard]] bool doCheckWithPartialInfoPush(KColData *data,
            CollisionCheckFunc collisionCheckFunc, CollisionInfo *colInfo, KCLTypeMask *typeMask);
    [[nodiscard]] bool doCheckWithFullInfo(KColData *data, CollisionCheckFunc collisionCheckFunc,
            CollisionInfo *colInfo, KCLTypeMask *flagsOut);
    [[nodiscard]] bool doCheckWithFullInfoPush(KColData *data,
            CollisionCheckFunc collisionCheckFunc, CollisionInfo *colInfo, KCLTypeMask *flagsOut);
    [[nodiscard]] bool doCheckMaskOnlyPush(KColData *data, CollisionCheckFunc collisionCheckFunc,
            KCLTypeMask *typeMaskOut);

    KColData *m_data;
    f32 m_kclScale;
    NoBounceWallColInfo *m_noBounceWallInfo;
    EGG::Matrix34f *m_localMtx;

    static CourseColMgr *s_instance; ///< @addr{0x809C3C10}
};

} // namespace Field
