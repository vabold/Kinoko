#pragma once

#include "game/field/obj/ObjectKCL.hh"

namespace Kinoko::Field {

/// @brief Represents an individual escalator on Coconut Mall.
/// @details Escalators have two settings which are used to determine two framecounts at which the
/// escalator comes to a standstill and changes to the next speed setting. This does not always
/// indicate a direction change, as can be demonstrated by the escalator right before the first
/// fountain whose steps always move downwards. Escalators use a piecewise function to mimic the
/// position offset induced by the individual steps. For Kinoko, creative liberty was taken to
/// create a few const members for expressions that are commonly re-used in this piecewise function
/// that do not need to be recomputed every frame.
class ObjectEscalator final : public ObjectKCL {
    /// @brief Grants the escalator group class access to the escalator's state
    friend class ObjectEscalatorGroup;

public:
    ObjectEscalator(const System::MapdataGeoObj &params, bool reverse = false);
    ~ObjectEscalator() override;

    void calc() override;
    [[nodiscard]] ObjectId id() const override;
    [[nodiscard]] LoadFlags loadFlags() const override;

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

    [[nodiscard]] const EGG::Matrix34f &getUpdatedMatrix(u32 timeOffset) override;
    [[nodiscard]] f32 colRadiusAdditionalLength() const override;

    [[nodiscard]] bool checkCollision(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;

private:
    /// @brief Alias for a member function pointer type used for point collision checks.
    /// @tparam T The type of collision info, either CollisionInfo or CollisionInfoPartial.
    template <typename T>
        requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
    using CheckPointFunc = bool (ObjColMgr::*)(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, T *info, KCLTypeMask *maskOut);

    /// @brief Alias for a member function pointer type used for sphere collision checks.
    /// @tparam T The type of collision info, either CollisionInfo or CollisionInfoPartial.
    template <typename T>
        requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
    using CheckSphereFunc = bool (ObjColMgr::*)(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, T *info, KCLTypeMask *maskOut);

    template <typename T>
        requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
    [[nodiscard]] bool checkPointImpl(CheckPointFunc<T> checkFunc, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, T *info, KCLTypeMask *maskOut);

    template <typename T>
        requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
    [[nodiscard]] bool checkSphereImpl(CheckSphereFunc<T> checkFunc, f32 radius,
            const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask, T *info,
            KCLTypeMask *maskOut, u32 timeOffset);

    [[nodiscard]] f32 calcWrappedStepCount(s32 t);
    [[nodiscard]] f32 calcSpeed(s32 t);

    EGG::Vector3f m_initialPos; ///< Initial position of the escalator
    f32 m_wrappedStepCount;     ///< Step count modulo 20
    EGG::Vector3f m_stepDims; ///< Length and height of steps but aligned with its facing direction
    EGG::Matrix34f m_workMatrix;            ///< Represents the current object collision matrix
    const std::array<s32, 2> m_stillFrames; ///< When escalators change direction (two times)
    const std::array<f32, 3> m_speed;       ///< Velocity for each of the three directions it moves
    const f32 m_checkColYPosMax; ///< Collision checks early return if you are above this height
    const f32 m_checkColYPosMin; ///< Collision checks early return if you are below this height

    // Helper members that only need to be computed once
    const std::array<f32, 2> m_stopFrames;      ///< When the escalator starts slowing down
    const std::array<f32, 2> m_startFrames;     ///< When the escalator starts accelerating
    const std::array<f32, 2> m_fullSpeedFrames; ///< When escalator reaches max speed
    const f32 m_midDuration; ///< Duration of full speed in between the dir changes

    /// @brief Helper function to compute the speeds of the escalator at each stage of its movement
    /// @param params The parameters used to initialize the object
    /// @param reverse Whether the escalator should move in reverse
    /// @return An array containing the speeds of the escalator at each stage of its movement
    [[nodiscard]] std::array<f32, 3> static InitSpeed(const System::MapdataGeoObj &params,
            bool reverse);

    static constexpr f32 STEP_HEIGHT = 10.0f;         ///< Height in units of a single step
    static constexpr f32 REVERSE_FRAMES_F32 = 240.0f; ///< from full speed to still or vice versa
    static constexpr f32 STILL_FRAMES = 50.0f; ///< Frames the escalator waits before speeding up

    /// @brief Maximum height offset to still perform collision checks
    static constexpr f32 MAX_HEIGHT_OFFSET = STEP_HEIGHT * (17.5f * STEP_HEIGHT);

    /// @brief Minimum height offset to still perform collision checks
    static constexpr f32 MIN_HEIGHT_OFFSET = 0.5f * (17.5f * STEP_HEIGHT);
};

} // namespace Kinoko::Field
