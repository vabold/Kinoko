#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectCarA;

class ObjectCarA : public ObjectCollidable, public StateManager<ObjectCarA> {
    friend StateManager<ObjectCarA>;

public:
    ObjectCarA(const System::MapdataGeoObj &params);
    ~ObjectCarA() override;

    void init() override;
    void calc() override;

    /// @addr{0x806B8F44}
    [[nodiscard]] virtual u32 loadFlags() const override {
        return 1;
    }

    void createCollision() override;
    void calcCollisionTransform() override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    void enterStateStub();
    void enterStandstill();
    void enterFullSpeed();
    void calcStandstill();
    void calcAccelerating();
    void calcFullSpeed();

private:
    void calcPos();

    /// @addr{0x806B8E54}
    EGG::Vector3f interpolate(f32 t, const EGG::Vector3f &v0, const EGG::Vector3f &v1) {
        return v0 + (v1 - v0) * t;
    }

    const f32 m_speed;
    const f32 m_accel;
    const u32 m_pauseTime;
    f32 m_railDuration;
    EGG::Vector3f m_currTangent; ///< It's @ref EGG::Vectof3f::ey unless it flies up in the air.
    EGG::Vector3f m_currUp;      ///< It's @ref EGG::Vectof3f::ey unless it flies up in the air.
    f32 m_currVel;
    s32 m_state;
    bool m_changingDir;

    static constexpr std::array<StateManagerEntry<ObjectCarA>, 3> STATE_ENTRIES = {{
            {0, &ObjectCarA::enterStandstill, &ObjectCarA::calcStandstill},
            {1, &ObjectCarA::enterStateStub, &ObjectCarA::calcAccelerating},
            {2, &ObjectCarA::enterFullSpeed, &ObjectCarA::calcFullSpeed},
    }};
};

} // namespace Field
