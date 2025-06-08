#pragma once

#include "game/field/KColData.hh"

namespace Field {

/// @brief Manager for an object's KCL interactions.
/// @nosubgrouping
class ObjColMgr {
public:
    ObjColMgr(const void *file);
    ~ObjColMgr();

    void narrScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask flags);

    [[nodiscard]] EGG::Vector3f kclLowWorld() const;
    [[nodiscard]] EGG::Vector3f kclHighWorld() const;

    [[nodiscard]] bool checkPointPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask flags, CollisionInfoPartial *infoOut, KCLTypeMask *typeMaskOut);
    [[nodiscard]] bool checkPointPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask flags, CollisionInfoPartial *info, KCLTypeMask *typeMaskOut);
    [[nodiscard]] bool checkPointFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *typeMaskOut);
    [[nodiscard]] bool checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *typeMaskOut);

    [[nodiscard]] bool checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfoPartial *info,
            KCLTypeMask *typeMaskOut);
    [[nodiscard]] bool checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfoPartial *info,
            KCLTypeMask *typeMaskOut);
    [[nodiscard]] bool checkSphereFull(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfo *info,
            KCLTypeMask *typeMaskOut);
    [[nodiscard]] bool checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfo *info,
            KCLTypeMask *typeMaskOut);

    [[nodiscard]] bool checkPointCachedPartial(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfoPartial *info,
            KCLTypeMask *typeMaskOut);
    [[nodiscard]] bool checkPointCachedPartialPush(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfoPartial *info,
            KCLTypeMask *typeMaskOut);
    [[nodiscard]] bool checkPointCachedFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask mask, CollisionInfo *pInfo, KCLTypeMask *typeMaskOut);
    [[nodiscard]] bool checkPointCachedFullPush(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *typeMaskOut);

    [[nodiscard]] bool checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfoPartial *info,
            KCLTypeMask *typeMaskOut);
    [[nodiscard]] bool checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfoPartial *info,
            KCLTypeMask *typeMaskOut);
    [[nodiscard]] bool checkSphereCachedFull(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *typeMaskOut);
    [[nodiscard]] bool checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *typeMaskOut);

    /// @beginSetters
    void setMtx(const EGG::Matrix34f &mtx) {
        m_mtx = mtx;
    }

    void setInvMtx(const EGG::Matrix34f &mtx) {
        m_mtxInv = mtx;
    }

    void setScale(f32 val) {
        m_kclScale = val;
    }

    void setMovingObjVel(const EGG::Vector3f &v) {
        m_movingObjVel = v;
    }
    /// @endSetters

private:
    KColData *m_data;
    EGG::Matrix34f m_mtx;
    EGG::Matrix34f m_mtxInv;
    f32 m_kclScale;
    EGG::Vector3f m_movingObjVel;
};

} // namespace Field
