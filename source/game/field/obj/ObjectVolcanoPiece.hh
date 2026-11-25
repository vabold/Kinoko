#pragma once

#include "game/field/obj/ObjectKCL.hh"

namespace Kinoko::Field {

/// @brief Represents an drivable object that eventually shakes and then falls, e.g. the ultra rock
/// on Grumble Volcano
/// @details Supports up to three separate collision managers for different parts of the volcano
/// piece. The piece initially is at rest. After @ref m_restEndFrame frames, it will start to shake.
/// Once @ref m_shakeEndFrame frames have elapsed, the piece will enter the quake state, in which
/// the piece remains at rest temporarily. Finally, the piece will begin falling for @ref
/// FALL_DURATION frames.
class ObjectVolcanoPiece final : public ObjectKCL {
public:
    ObjectVolcanoPiece(const System::MapdataGeoObj &params);
    ~ObjectVolcanoPiece() override;

    void calc() override;
    void createCollision() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getKclName() const override;

    /// @beginPublicColCheckFuncs

    [[nodiscard]] bool checkPointPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) override;
    [[nodiscard]] bool checkPointPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) override;
    [[nodiscard]] bool checkPointFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) override;
    [[nodiscard]] bool checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) override;

    [[nodiscard]] bool checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereFull(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;

    void narrScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
            u32 timeOffset) override;

    [[nodiscard]] bool checkPointCachedPartial(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut) override;
    [[nodiscard]] bool checkPointCachedPartialPush(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut) override;
    [[nodiscard]] bool checkPointCachedFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) override;
    [[nodiscard]] bool checkPointCachedFullPush(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut) override;

    [[nodiscard]] bool checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereCachedFull(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;

    /// @endPublicColCheckFuncs

    void update(u32 timeOffset) override;
    void calcScale(u32 timeOffset) override;
    void setMovingObjVel(const EGG::Vector3f &v) override;
    [[nodiscard]] const EGG::Matrix34f &getUpdatedMatrix(u32 timeOffset) override;

    /// @beginPublicColCheckFuncs

    [[nodiscard]] bool checkCollision(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;

    /// @endPublicColCheckFuncs

private:
    /// @brief Represents the different motion states of the volcano piece
    enum class State {
        Rest = 0,  ///< The volcano piece is at rest and not moving
        Shake = 1, ///< The volcano piece is shaking
        Quake = 2, ///< The volcano piece is about to fall
        Fall = 3,  ///< The volcano piece is falling
        Gone = 4,  ///< The volcano piece has disappeared below
    };

    /// @beginPrivateColCheckFuncs

    /// @brief Function pointer type for a function that represents a partial point collision check
    /// @param pos The current position of the point to check
    /// @param prevPos The previous position of the point to check
    /// @param mask The KCL flags to check collision against
    /// @param info Pointer to the structure to store partial collision information
    /// @param maskOut Pointer to a variable to store the resulting KCL flags after the check
    typedef bool (ObjColMgr::*CheckPointPartialFunc)(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut);

    /// @brief Function pointer type for a function that represents a full point collision check
    /// @param pos The current position of the point to check
    /// @param prevPos The previous position of the point to check
    /// @param mask The KCL flags to check collision against
    /// @param info Pointer to the structure to store full collision information
    /// @param maskOut Pointer to a variable to store the resulting KCL flags after the check
    typedef bool (ObjColMgr::*CheckPointFullFunc)(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut);

    /// @brief Function pointer type for a function that represents a partial sphere collision check
    /// @param radius The radius of the sphere to check
    /// @param pos The current position of the point to check
    /// @param prevPos The previous position of the point to check
    /// @param mask The KCL flags to check collision against
    /// @param info Pointer to the structure to store partial collision information
    /// @param maskOut Pointer to a variable to store the resulting KCL flags after the check
    typedef bool (ObjColMgr::*CheckSpherePartialFunc)(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut);

    /// @brief Function pointer type for a function that represents a full sphere collision check
    /// @param radius The radius of the sphere to check
    /// @param pos The current position of the point to check
    /// @param prevPos The previous position of the point to check
    /// @param mask The KCL flags to check collision against
    /// @param info Pointer to the structure to store collision information
    /// @param maskOut Pointer to a variable to store the resulting KCL flags after the check
    typedef bool (ObjColMgr::*CheckSphereFullFunc)(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut);

    template <typename T, typename U>
        requires(std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>) &&
            (std::is_same_v<U, CheckPointPartialFunc> || std::is_same_v<U, CheckPointFullFunc>)
    [[nodiscard]] bool checkPointImpl(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask mask, T *info, KCLTypeMask *maskOut, U checkFunc);

    template <typename T, typename U>
        requires(std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>) &&
            (std::is_same_v<U, CheckSpherePartialFunc> || std::is_same_v<U, CheckSphereFullFunc>)
    [[nodiscard]] bool checkSphereImpl(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, T *info, KCLTypeMask *maskOut,
            u32 timeOffset, U checkFunc);

    [[nodiscard]] bool checkCollisionImpl(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset, CheckSphereFullFunc checkFunc);

    /// @endPrivateColCheckFuncs

    const EGG::Matrix34f &calcShakeAndFall(EGG::Vector3f *vel, u32 timeOffset);
    [[nodiscard]] State calcState(u32 frame) const;
    [[nodiscard]] f32 calcT(u32 frame) const;
    [[nodiscard]] f32 getShakePosY(u32 t) const;

    char m_modelName[16];             ///< Name of the KCL file for this volcano piece
    const EGG::Vector3f m_initialPos; ///< Initial position of the volcano piece
    const u32 m_restEndFrame;         ///< Frame at which the rest state ends
    const u32 m_shakeEndFrame;        ///< Frame at which the shake state ends
    const u32 m_quakeEndFrame;        ///< Frame at which the quake state ends
    EGG::Matrix34f m_rtMat;           ///< Current frame's rotation/translation matrix
    ObjColMgr *m_colMgrB; ///< Collision manager for KCL when the rock starts shaking and falling
    ObjColMgr *m_colMgrC; ///< Collision manager for stationary KCL when the rock starts to fall

    /// @brief How long the piece will fall for before transitioning to the Gone state
    static constexpr u32 FALL_DURATION = 900;
};

} // namespace Kinoko::Field
