#pragma once

#include "game/field/obj/ObjectKCL.hh"

namespace Field {

/// @brief Represents an object that eventually shakes and then falls, e.g. the ultra rock on GV.
class ObjectVolcanoPiece final : public ObjectKCL {
public:
    ObjectVolcanoPiece(const System::MapdataGeoObj &params);
    ~ObjectVolcanoPiece() override;

    void calc() override;

    /// @addr{0x80805974}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x80803F6C}
    [[nodiscard]] const char *getKclName() const override {
        return m_modelName;
    }

    typedef bool (ObjColMgr::*CheckPointPartialFunc)(const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
            KCLTypeMask *pFlagsOut);

    typedef bool (ObjColMgr::*CheckPointFullFunc)(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut);

    typedef bool (ObjColMgr::*CheckSpherePartialFunc)(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
            KCLTypeMask *pFlagsOut);

    typedef bool (ObjColMgr::*CheckSphereFullFunc)(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *pFlagsOut);

    void createCollision() override;

    template <typename T, typename U>
        requires(std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>) &&
            (std::is_same_v<U, CheckPointPartialFunc> || std::is_same_v<U, CheckPointFullFunc>)
    [[nodiscard]] bool checkPointImpl(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, T *pInfo, KCLTypeMask *pFlagsOut, U checkFunc);

    template <typename T, typename U>
        requires(std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>) &&
            (std::is_same_v<U, CheckSpherePartialFunc> || std::is_same_v<U, CheckSphereFullFunc>)
    [[nodiscard]] bool checkSphereImpl(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, T *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset, U checkFunc);

    [[nodiscard]] bool checkCollisionImpl(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset, CheckSphereFullFunc checkFunc);

    /// @addr{0x80805404}
    [[nodiscard]] bool checkPointPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) override {
        return checkPointImpl(v0, v1, flags, pInfo, pFlagsOut, &ObjColMgr::checkPointPartial);
    }

    /// @addr{0x8080554C}
    [[nodiscard]] bool checkPointPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) override {
        return checkPointImpl(v0, v1, flags, pInfo, pFlagsOut, &ObjColMgr::checkPointPartialPush);
    }

    /// @addr{0x80805694}
    [[nodiscard]] bool checkPointFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) override {
        return checkPointImpl(v0, v1, flags, pInfo, pFlagsOut, &ObjColMgr::checkPointFull);
    }

    /// @addr{0x808057DC}
    [[nodiscard]] bool checkPointFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) override {
        return checkPointImpl(v0, v1, flags, pInfo, pFlagsOut, &ObjColMgr::checkPointFullPush);
    }

    /// @addr{0x80804F68}
    [[nodiscard]] bool checkSpherePartial(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override {
        return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset,
                &ObjColMgr::checkSpherePartial);
    }

    /// @addr{0x808050EC}
    [[nodiscard]] bool checkSpherePartialPush(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override {
        return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset,
                &ObjColMgr::checkSpherePartialPush);
    }

    /// @addr{0x80805270}
    [[nodiscard]] bool checkSphereFull(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
            u32 timeOffset) override {
        return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset,
                &ObjColMgr::checkSphereFull);
    }

    /// @addr{0x808053F4}
    [[nodiscard]] bool checkSphereFullPush(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override {
        return checkCollision(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
    }

    void narrScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
            u32 timeOffset) override;

    /// @addr{0x80804A48}
    [[nodiscard]] bool checkPointCachedPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) override {
        return checkPointImpl(v0, v1, flags, pInfo, pFlagsOut, &ObjColMgr::checkPointCachedPartial);
    }

    /// @addr{0x80804B90}
    [[nodiscard]] bool checkPointCachedPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) override {
        return checkPointImpl(v0, v1, flags, pInfo, pFlagsOut,
                &ObjColMgr::checkPointCachedPartialPush);
    }

    /// @addr{0x80804CD8}
    [[nodiscard]] bool checkPointCachedFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) override {
        return checkPointImpl(v0, v1, flags, pInfo, pFlagsOut, &ObjColMgr::checkPointCachedFull);
    }

    /// @addr{0x80804E20}
    [[nodiscard]] bool checkPointCachedFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) override {
        return checkPointImpl(v0, v1, flags, pInfo, pFlagsOut,
                &ObjColMgr::checkPointCachedFullPush);
    }

    /// @addr{0x808045AC}
    [[nodiscard]] bool checkSphereCachedPartial(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override {
        return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset,
                &ObjColMgr::checkSphereCachedPartial);
    }

    /// @addr{0x80804730}
    [[nodiscard]] bool checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override {
        return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset,
                &ObjColMgr::checkSphereCachedPartialPush);
    }

    /// @addr{0x808048B4}
    [[nodiscard]] bool checkSphereCachedFull(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override {
        return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset,
                &ObjColMgr::checkSphereCachedFull);
    }

    /// @addr{0x80804A38}
    [[nodiscard]] bool checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override {
        return checkCollisionCached(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
    }

    void update(u32 timeOffset) override;
    void calcScale(u32 timeOffset) override;
    void setMovingObjVel(const EGG::Vector3f &v) override;

    /// @addr{0x8080595C}
    [[nodiscard]] const EGG::Matrix34f &getUpdatedMatrix(u32 timeOffset) override {
        return calcShakeAndFall(nullptr, timeOffset);
    }

    /// @addr{0x808199A8}
    [[nodiscard]] bool checkCollision(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override {
        return checkCollisionImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset,
                &ObjColMgr::checkSphereFullPush);
    }

    /// @addr{0x80819DA0}
    [[nodiscard]] bool checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override {
        return checkCollisionImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset,
                &ObjColMgr::checkSphereCachedFullPush);
    }

private:
    enum class State {
        Rest = 0,
        Shake = 1,
        Quake = 2,
        Fall = 3,
        Gone = 4,
    };

    const EGG::Matrix34f &calcShakeAndFall(EGG::Vector3f *vel, u32 timeOffset);
    State calcState(s32 frame) const;
    f32 calcT(s32 frame) const;

    char m_modelName[16];
    const EGG::Vector3f m_initialPos;
    const EGG::Vector3f m_initialRot;
    const s32 m_restDuration;
    const s32 m_shakeDuration;
    const s32 m_quakeDuration;
    EGG::Matrix34f m_rtMat;
    ObjColMgr *m_colMgrB;
    ObjColMgr *m_colMgrC;

    static constexpr s32 FALL_DURATION = 900;
};

} // namespace Field
