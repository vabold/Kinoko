#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class RailInterpolator;

/// @brief The base class shared between @ref ObjectCowLeader and @ref ObjectCowFollower.
class ObjectCow : public ObjectCollidable {
public:
    ObjectCow(const System::MapdataGeoObj &params);
    ~ObjectCow() override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

protected:
    virtual void calcFloor();

    void setup();
    void calcPos();
    f32 setTarget(const EGG::Vector3f &v);

    /// @addr{0x806BCF4C}
    [[nodiscard]] static EGG::Vector3f Interpolate(f32 t, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1) {
        return v0 + (v1 - v0) * t;
    }

    u32 m_startFrame; ///< The frame the cow will start moving
    EGG::Vector3f m_tangent;
    EGG::Vector3f m_prevTangent;
    EGG::Vector3f m_up;
    EGG::Vector3f m_velocity;
    f32 m_xzSpeed; ///< XZ plane length of m_velocity
    f32 m_tangentFactor;
    EGG::Vector3f m_floorNrm;
    EGG::Vector3f m_state1TargetPos; ///< Calculated in enterState1
    EGG::Vector3f m_targetDir;
    EGG::Vector3f m_upForce; ///< Used by calcPos to counteract gravity
    f32 m_interpRate;

    static constexpr EGG::Vector3f GRAVITY_FORCE = EGG::Vector3f(0.0f, 2.0f, 0.0f);
};

class ObjectCowLeader;

/// @brief A cow who its own rail and whose position is not influenced by the path of the others.
class ObjectCowLeader final : public ObjectCow, public StateManager<ObjectCowLeader> {
    friend class ObjectCowHerd;
    friend class StateManager<ObjectCowLeader>;

public:
    ObjectCowLeader(const System::MapdataGeoObj &params);
    ~ObjectCowLeader() override;

    void init() override;
    void calc() override;

    /// @addr{0x806BF42C}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

private:
    enum class AnmType {
        EatST = 0,
        Eat = 1,
        EatED = 2,
    };

    void calcFloor() override;

    void enterWait();
    void enterEat();
    void enterRoam();

    void calcWait();
    void calcEat();
    void calcRoam();

    f32 m_railSpeed;
    bool m_endedRailSegment;
    AnmType m_state1AnmType;
    u16 m_eatFrames; ///< Length of the state 1 eat animation

    static constexpr std::array<StateManagerEntry<ObjectCowLeader>, 3> STATE_ENTRIES = {{
            {0, &ObjectCowLeader::enterWait, &ObjectCowLeader::calcWait},
            {1, &ObjectCowLeader::enterEat, &ObjectCowLeader::calcEat},
            {2, &ObjectCowLeader::enterRoam, &ObjectCowLeader::calcRoam},
    }};
};

class ObjectCowFollower;

/// @brief A cow that follows a leader by sharing the same rail.
class ObjectCowFollower final : public ObjectCow, public StateManager<ObjectCowFollower> {
    friend class ObjectCowHerd;
    friend class StateManager<ObjectCowFollower>;

public:
    ObjectCowFollower(const System::MapdataGeoObj &params, const EGG::Vector3f &pos, f32 rot);
    ~ObjectCowFollower() override;

    void init() override;
    void calc() override;

    /// @addr{0x806BF424}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806BF420}
    void loadRail() override {}

private:
    void enterWait();
    void enterFreeRoam();
    void enterFollowLeader();

    void calcWait();
    void calcFreeRoam();
    void calcFollowLeader();

    const EGG::Vector3f m_posOffset;
    RailInterpolator *m_rail;
    u16 m_waitFrames;       ///< Number of frames the cow will stand still for
    f32 m_topSpeed;         ///< The speed the cow will accelerate up to
    bool m_bStopping;       ///< Set when the cow is coming to a stop
    f32 m_railSegThreshold; ///< The rail segmentT at which a cow will change to state 2

    static constexpr f32 BASE_TOP_SPEED = 2.0f;
    static constexpr f32 TOP_SPEED_VARIANCE = 4.0f - 2.0f;

    /// @brief Distance at which a cow is considered close enough to the rail to stop moving.
    static constexpr f32 DIST_THRESHOLD = 200.0f;

    static constexpr std::array<StateManagerEntry<ObjectCowFollower>, 3> STATE_ENTRIES = {{
            {0, &ObjectCowFollower::enterWait, &ObjectCowFollower::calcWait},
            {1, &ObjectCowFollower::enterFreeRoam, &ObjectCowFollower::calcFreeRoam},
            {2, &ObjectCowFollower::enterFollowLeader, &ObjectCowFollower::calcFollowLeader},
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

    /// @addr{0x806BF42C}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806BF348}
    void createCollision() override {}

    /// @addr{0x806BF34C}
    void loadRail() override {}

private:
    void checkCollision();

    ObjectCowLeader *m_leader;
    std::span<ObjectCowFollower *> m_followers;
};

} // namespace Field
