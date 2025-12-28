#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief Represents a fireball that is launched from the volcanoes on Grumble Volcano.
class ObjectVolcanoBall final : public ObjectCollidable, public StateManager {
    friend class ObjectVolcanoBallLauncher;

public:
    ObjectVolcanoBall(f32 accel, f32 finalVel, f32 endPosY, const System::MapdataGeoObj &params,
            const EGG::Vector3f &vel);
    ~ObjectVolcanoBall() override;

    void init() override;

    /// @addr{0x806E2E08}
    void calc() override {
        StateManager::calc();
    }

    /// @addr{0x806E3A7C}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806E2CF0}
    Kart::Reaction onCollision(Kart::KartObject * /*kartObj*/, Kart::Reaction reactionOnKart,
            Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) override {
        return reactionOnKart;
    }

private:
    /// @addr{0x806E2F24}
    void enterState0() {
        init();
    }

    /// @addr{0x806E2F38}
    void enterState1() {
        init();
    }

    /// @addr{0x806E327C}
    void enterState2() {}

    /// @addr{0x806E2F34}
    void calcDormant() {}

    void calcFalling();

    /// @addr{0x806E3324}
    void calcBurning() {
        if (m_currentFrame >= static_cast<u32>(m_burnDuration)) {
            m_nextStateId = 0;
        }
    }

    const s16 m_burnDuration; ///< How long the ball burns for before disappearing
    const f32 m_accel;
    const f32 m_finalVel; ///< Velocity of the ball at the moment of impact
    const f32 m_endPosY;  ///< Height of the ball at the end of its rail
    const f32 m_sqVelXZ;  ///< Squared X-Z plane velocity

    static constexpr std::array<StateManagerEntry, 3> STATE_ENTRIES = {{
            {StateEntry<ObjectVolcanoBall, &ObjectVolcanoBall::enterState0,
                    &ObjectVolcanoBall::calcDormant>(0)},
            {StateEntry<ObjectVolcanoBall, &ObjectVolcanoBall::enterState1,
                    &ObjectVolcanoBall::calcFalling>(1)},
            {StateEntry<ObjectVolcanoBall, &ObjectVolcanoBall::enterState2,
                    &ObjectVolcanoBall::calcBurning>(2)},
    }};
};

} // namespace Field
