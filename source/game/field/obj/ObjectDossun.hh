#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief Base class for the various different Thwomp types in the game.
/// @details On initialization, the game pre-calculates how many frames it takes for the Thwomp
/// to stomp down and hit the floor. If a Thwomp is positioned such that there is no floor beneath
/// it, then the init function will be stuck in an infinite loop, causing the race to never start.
class ObjectDossun : public ObjectCollidable {
public:
    ObjectDossun(const System::MapdataGeoObj &params);
    ~ObjectDossun() override;

    void init() override;

    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    void calcCollisionTransform() override;
    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    void initState();
    void calcStomp();

    /// @addr{0x8075FB50}
    virtual void startStill() {
        m_anmState = AnmState::Still;
        m_shakePhase = 0;
        m_vel = 0.0f;
        setRot(EGG::Vector3f(rot().x, m_currRot, rot().z));
    }

    /// @addr{0x8075FE8C}
    void startGrounded() {
        m_anmState = AnmState::Grounded;
        m_groundedTimer = GROUND_DURATION;
    }

protected:
    /// @brief Represents the current movement state of the Thwomp in terms of its stomp
    enum class AnmState {
        Still = 0,
        BeforeFall = 1, ///< The quick upwards motion before slamming down
        Falling = 2,    ///< Slamming down
        Grounded = 3,   ///< Contacting the floor
        Rising = 4,     ///< Resetting
    };

    enum class StompState {
        Inactive = 0,
        Active = 1,
    };

    StompState m_stompState;
    AnmState m_anmState;
    s32 m_beforeFallTimer; ///< Number of frames until the Thwomp will stomp down
    s32 m_stillTimer;      ///< Number of frames the Thwomp will remain stationary (or shakes)
    s32 m_groundedTimer;   ///< Number of frames the Thwomp remains on the ground
    s32 m_shakePhase; ///< Causes the Thwomp's position to shake at the end of the "still" state
    f32 m_vel;        ///< Vector3f in the base game, but only the y-component is used
    f32 m_initialPosY;
    u32 m_fullDuration; ///< Framecount of an entire animation loop
    s32 m_cycleTimer;
    f32 m_currRot;
    bool m_touchingGround;

    /// The total number of frames the thwomp rises before crashing down
    static constexpr u32 BEFORE_FALL_DURATION = 10;

private:
    void calcBeforeFall();

    /// @addr{0x8075F430}
    void calcFalling() {
        m_vel -= STOMP_ACCEL;
        setPos(EGG::Vector3f(pos().x, m_vel + pos().y, pos().z));
        checkFloorCollision();
    }

    /// @addr{0x8075F460}
    void calcGrounded() {
        if (--m_groundedTimer == 0) {
            m_anmState = AnmState::Rising;
        }
    }

    /// @addr{0x8075F4D8}
    void calcRising() {
        f32 posY = std::min(RISING_VEL + pos().y, m_initialPosY);
        setPos(EGG::Vector3f(pos().x, posY, pos().z));
    }

    void checkFloorCollision();

    static constexpr u32 GROUND_DURATION = 60;
    static constexpr u32 STOMP_ACCEL = 17.0f;
    static constexpr f32 STOMP_RADIUS = 20.0f;
    static constexpr EGG::Vector3f STOMP_POS_OFFSET = EGG::Vector3f(0.0f, STOMP_RADIUS, 0.0f);
    static constexpr f32 RISING_VEL = 10.0f;
};

} // namespace Field
