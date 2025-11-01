#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief Base class
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
    virtual void startStill();
    void startGrounded();

protected:
    enum class MoveState {
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
    MoveState m_anmState;  ///< Offset 0xb0
    u32 m_beforeFallTimer; ///< Number of frames the Thwomp travels upwards before crashing down
    u32 m_stillTimer;      ///< Number of frames the Thwomp remains stationary (or shakes)
    s32 m_groundedTimer;   ///< Number of frames the Thwomp remains on the ground
    s32 m_shakePhase; ///< Causes the Thwomp's position to shake at the end of the "still" state
    f32 m_vel;        ///< Vector3f in the base game, but only the y-component is used
    f32 m_initialPosY;
    u32 m_fallDuration; ///< Frames the Thwomp falls for
    u32 m_riseDuration; ///< Frames the Thwomp rises for
    u32 m_fullDuration; ///< Framecount of an entire animation loop
    s32 m_cycleTimer;
    f32 m_currRot;
    bool m_touchingGround;

private:
    void calcBeforeFall();
    void calcFalling();
    void calcGrounded();
    void calcRising();
    void checkFloorCollision();

    static constexpr u32 GROUND_DURATION = 60;
    static constexpr u32 RISE_DURATION = 10;
    static constexpr u32 STOMP_ACCEL = 17.0f;

    ///< Used to index the hit table when the Thwomp stomps down and crushes.
    static constexpr ObjectId SOKO_ID = ObjectId::DossuncSoko;
};

} // namespace Field
