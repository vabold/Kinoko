#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectCarA : public ObjectCollidable, public StateManager {
public:
    ObjectCarA(const System::MapdataGeoObj &params);
    ~ObjectCarA() override;

    void init() override;
    void calc() override;

    /// @addr{0x806B8F44}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    void createCollision() override;
    void calcCollisionTransform() override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

private:
    // Trapezoidal motion profile
    enum class MotionState {
        Accelerating = 0,
        Cruising = 1,
        Decelerating = 2,
    };

    void calcPos();

    void enterStop();
    void enterAccel() {}
    void enterCruising();

    void calcStop();
    void calcAccel();
    void calcCruising();

    const f32 m_finalVel;
    const f32 m_accel;
    const s32 m_stopTime;
    f32 m_cruiseTime;            ///< How long to spend at cruising speed before decelerating.
    EGG::Vector3f m_currTangent; ///< It's @ref EGG::Vector3f::ey unless it flies up in the air.
    EGG::Vector3f m_currUp;      ///< It's @ref EGG::Vector3f::ey unless it flies up in the air.
    f32 m_currVel;
    MotionState m_motionState;
    bool m_changingDir;

    static constexpr std::array<StateManagerEntry, 3> STATE_ENTRIES = {{
            {StateEntry<ObjectCarA, &ObjectCarA::enterStop, &ObjectCarA::calcStop>(0)},
            {StateEntry<ObjectCarA, &ObjectCarA::enterAccel, &ObjectCarA::calcAccel>(1)},
            {StateEntry<ObjectCarA, &ObjectCarA::enterCruising, &ObjectCarA::calcCruising>(2)},
    }};
};

} // namespace Field
