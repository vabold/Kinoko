#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief The traffic cones on Daisy Circuit.
class ObjectPylon final : public ObjectCollidable {
public:
    ObjectPylon(const System::MapdataGeoObj &params);
    ~ObjectPylon() override;

    void init() override;
    void calc() override;

    /// @addr{0x8082E4F8}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

private:
    enum State {
        Idle = 0,
        Moving = 1,
        Hide = 2,
        ComeBack = 3,
        Hiding = 4,
        Hit = 5,
    };

    void checkIntraCollision(const EGG::Vector3f &hitDepth);

    void startHit(f32 velFactor, EGG::Vector3f &hitDepth);

    void calcHit();
    void calcHiding();
    void calcHide();
    void calcComeBack();

    State m_state;
    std::array<ObjectPylon *, 2> m_neighbors;
    const EGG::Vector3f m_initPos;
    const EGG::Vector3f m_initScale;
    const EGG::Vector3f m_initRot;
    u32 m_stateStartFrame;  ///< Frame when pylon entered the current m_state
    u32 m_numBounces;       ///< Number of floor collisions while in Hit state
    EGG::Vector3f m_vel;    ///< Velocity used in Hit and ComeBack states
    EGG::Vector3f m_angVel; ///< Added to rotation while in Hit state

    static constexpr f32 RADIUS = 120.0f;
    static constexpr f32 FALL_VEL = 20.0f;

    /// Minimum frames before a state change can occur
    static constexpr u32 STATE_COOLDOWN_FRAMES = 5;
};

} // namespace Field
