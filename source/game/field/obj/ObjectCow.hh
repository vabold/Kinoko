#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

class RailInterpolator;

/// @brief Represents a cow on Moo Moo Meadows
/// @details The base class shared between @ref ObjectCowLeader and @ref ObjectCowFollower.
class ObjectCow : public ObjectCollidable {
public:
    ObjectCow(const System::MapdataGeoObj &params);
    ~ObjectCow() override;

    void init() override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

protected:
    virtual void calcFloor();

    void calcPos();
    void interpOrientation();
    f32 setTarget(const EGG::Vector3f &v);

    const u32 m_startFrame;      ///< The frame the cow will start moving
    EGG::Vector3f m_tangent;     ///< Smoothed forward direction, interpolated toward m_targetDir
    EGG::Vector3f m_prevTangent; ///< The previous frame's @ref m_tangent, used to smooth rotation
    EGG::Vector3f m_up;          ///< Smoothed up direction, interpolated toward the floor normal
    EGG::Vector3f m_velocity;    ///< The current velocity of the cow
    f32 m_xzSpeed;               ///< XZ plane length of m_velocity
    f32 m_tangentAccel; ///< Magnitude of acceleration applied in the direction of @ref m_tangent
    EGG::Vector3f m_floorNrm;  ///< The normal of the floor, used to orient the cow to the ground
    EGG::Vector3f m_targetPos; ///< Position the cow is walking towards
    EGG::Vector3f m_targetDir; ///< Direction from current position to @ref m_targetPos
    EGG::Vector3f m_upForce;   ///< Used by calcPos to counteract gravity
    f32 m_interpRate; ///< Rate at which @ref m_tangent is interpolated toward @ref m_targetDir

    /// @brief The force of gravity applied to the cow when it is not on the ground
    static constexpr EGG::Vector3f GRAVITY_FORCE = EGG::Vector3f(0.0f, 2.0f, 0.0f);
};

/// @brief A cow who its own rail and whose position is not influenced by the path of the others.
/// @details Walks from rail segment waypoint to waypoint, stopping to eat grass along the way. The
/// time it spends eating grass is a random number of frames in the range `[120, 240]`.
class ObjectCowLeader final : public ObjectCow, private StateManager {
    /// @brief Grants the herd class access to the leader's state
    friend class ObjectCowHerd;

public:
    ObjectCowLeader(const System::MapdataGeoObj &params);
    ~ObjectCowLeader() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    /// @brief Represents the different states of the cow's eating animation
    enum class EatAnmType {
        EatST = 0, ///< The cow is starting to eat grass
        Eat = 1,   ///< The cow is currently eating grass
        EatED = 2, ///< The cow has finished eating grass
    };

    void calcFloor() override;

    void enterWait();
    void enterEat();
    void enterRoam();

    void calcWait();
    void calcEat();
    void calcRoam();

    f32 m_railSpeed;         ///< Current speed of the cow along its rail
    bool m_endedRailSegment; ///< Set when the cow has reached a waypoint on its rail
    EatAnmType m_eatAnmType; ///< The current state of the cow's eating animation
    u16 m_eatFrames;         ///< How long the cow stays in its EatAnmType::Eat state for

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 3> STATE_ENTRIES = {{
            {StateEntry<ObjectCowLeader, &ObjectCowLeader::enterWait, &ObjectCowLeader::calcWait>(
                    0)},
            {StateEntry<ObjectCowLeader, &ObjectCowLeader::enterEat, &ObjectCowLeader::calcEat>(1)},
            {StateEntry<ObjectCowLeader, &ObjectCowLeader::enterRoam, &ObjectCowLeader::calcRoam>(
                    2)},
    }};
};

/// @brief A cow that follows a leader by sharing the same rail.
class ObjectCowFollower final : public ObjectCow, private StateManager {
    /// @brief Grants the herd class access to the follower's state
    friend class ObjectCowHerd;

public:
    ObjectCowFollower(const System::MapdataGeoObj &params, const EGG::Vector3f &pos, f32 initRot);
    ~ObjectCowFollower() override;

    void init() override;
    void calc() override;
    void loadRail() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    void enterWait();
    void enterFreeRoam();
    void enterFollowLeader();

    void calcWait();
    void calcFreeRoam();
    void calcFollowLeader();

    const EGG::Vector3f m_posOffset; ///< Fixed offset from the leader's position
    RailInterpolator *m_rail;        ///< Pointer to the leader's rail interpolator
    u16 m_waitFrames;                ///< Number of frames the cow will stand still for
    f32 m_topSpeed;                  ///< The speed the cow will accelerate up to
    bool m_bStopping;                ///< Set when the cow is coming to a stop
    f32 m_railSegThreshold; ///< The rail segmentT at which a cow will change to the follow state

    static constexpr f32 BASE_TOP_SPEED = 2.0f;            ///< The minimum top speed for a cow
    static constexpr f32 TOP_SPEED_VARIANCE = 4.0f - 2.0f; ///< The variance of possible top speeds

    /// @brief Distance at which a cow is close enough to the leader's rail to stop moving
    static constexpr f32 DIST_THRESHOLD = 200.0f;

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 3> STATE_ENTRIES = {{
            {StateEntry<ObjectCowFollower, &ObjectCowFollower::enterWait,
                    &ObjectCowFollower::calcWait>(0)},
            {StateEntry<ObjectCowFollower, &ObjectCowFollower::enterFreeRoam,
                    &ObjectCowFollower::calcFreeRoam>(1)},
            {StateEntry<ObjectCowFollower, &ObjectCowFollower::enterFollowLeader,
                    &ObjectCowFollower::calcFollowLeader>(2)},
    }};
};

/// @brief The manager class that controls a group of cows.
/// @details The herd is led by the @ref ObjectCowLeader. @ref ObjectCowFollower cows share the same
/// rail as the leader cow but have fluctuations in their position. This class also handles
/// collision checks amongst the cows to make sure that they never walk through each other.
class ObjectCowHerd final : public ObjectCollidable {
public:
    ObjectCowHerd(const System::MapdataGeoObj &params);
    ~ObjectCowHerd() override;

    void init() override;
    void calc() override;
    void createCollision() override;
    void loadRail() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    void checkIntraCollision();

    ObjectCowLeader *m_leader; ///< Pointer to the single leader that owns the rail
    owning_span<ObjectCowFollower *> m_followers; ///< All cows that follow the leader
};

} // namespace Kinoko::Field
