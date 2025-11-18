#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectProjectile.hh"

namespace Field {

class ObjectHeyhoBall;

/// @brief The cannonball projectiles on GBA Shy Guy Beach.
class ObjectHeyhoBall final : public ObjectProjectile, public StateManager<ObjectHeyhoBall> {
    friend StateManager<ObjectHeyhoBall>;

public:
    ObjectHeyhoBall(const System::MapdataGeoObj &params);
    ~ObjectHeyhoBall() override;

    void init() override;
    void calc() override;

    [[nodiscard]] Kart::Reaction onCollision(Kart::KartObject *kartObj,
            Kart::Reaction reactionOnKart, Kart::Reaction reactionOnObj,
            EGG::Vector3f &hitDepth) override;

    /// @addr{0x806D18EC}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    void initProjectile(const EGG::Vector3f &pos) override;

    /// @addr{0x806D188C}
    void onLaunch() override {
        m_nextStateId = 1;
    }

    [[nodiscard]] f32 yDist() const {
        return m_yDist;
    }

    [[nodiscard]] f32 initYSpeed() const {
        return m_initYSpeed;
    }

private:
    enum class ExplosionIntensity {
        ExplosionLoseItem = 0,
        SpinSomeSpeed = 1,
    };

    void enterState0();
    void enterState1();
    void enterState2();
    void enterState3();

    void calcState0();
    void calcState1();
    void calcState2();
    void calcState3();

    const f32 m_airtime; ///< Number of frames between shooting and landing
    EGG::Vector3f m_shipPos;
    const EGG::Vector3f m_initPos;
    EGG::Vector3f m_xzDir;
    f32 m_yDist;
    f32 m_xzSpeed;
    f32 m_initYSpeed;
    f32 m_blastRadiusRatio; ///< Ratio between the blast radius and the shell's radius
    f32 m_scaleChangeRate;
    EGG::Vector3f m_workingPos;
    ExplosionIntensity m_intensity;

    static constexpr std::array<StateManagerEntry<ObjectHeyhoBall>, 4> STATE_ENTRIES = {{
            {0, &ObjectHeyhoBall::enterState0, &ObjectHeyhoBall::calcState0},
            {1, &ObjectHeyhoBall::enterState1, &ObjectHeyhoBall::calcState1},
            {2, &ObjectHeyhoBall::enterState2, &ObjectHeyhoBall::calcState2},
            {3, &ObjectHeyhoBall::enterState3, &ObjectHeyhoBall::calcState3},
    }};
};

} // namespace Field
