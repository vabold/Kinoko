#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectProjectile.hh"

namespace Field {

/// @brief The cannonball projectiles on GBA Shy Guy Beach.
class ObjectHeyhoBall final : public ObjectProjectile, public StateManager {
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

    /// @addr{0x806D0A00}
    void enterIntangible() {}

    void enterFalling();

    /// @addr{0x806D0C0C}
    void enterBlinking() {
        m_workingPos = m_initPos + EGG::Vector3f::ey * -BALL_RADIUS;
    }

    /// @addr{0x806D0D84}
    void enterExploding() {
        m_scaleChangeRate = (1.2f * m_blastRadiusRatio - 1.0f) / 40.0f / 40.0f;
    }

    /// @addr{0x806D0A14}
    void calcIntangible() {
        m_workingPos = m_shipPos;
    }

    void calcFalling();

    /// @addr{0x806D0CD8}
    void calcBlinking() {
        constexpr u32 REST_FRAMES = 180;

        if (m_currentFrame >= REST_FRAMES) {
            m_nextStateId = 3;
        }
    }

    void calcExploding();

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

    static constexpr f32 BALL_RADIUS = 50.0f;
    static constexpr f32 BLAST_RADIUS = 1500.0f;

    static constexpr std::array<StateManagerEntry, 4> STATE_ENTRIES = {{
            {StateEntry<ObjectHeyhoBall, &ObjectHeyhoBall::enterIntangible,
                    &ObjectHeyhoBall::calcIntangible>(0)},
            {StateEntry<ObjectHeyhoBall, &ObjectHeyhoBall::enterFalling,
                    &ObjectHeyhoBall::calcFalling>(1)},
            {StateEntry<ObjectHeyhoBall, &ObjectHeyhoBall::enterBlinking,
                    &ObjectHeyhoBall::calcBlinking>(2)},
            {StateEntry<ObjectHeyhoBall, &ObjectHeyhoBall::enterExploding,
                    &ObjectHeyhoBall::calcExploding>(3)},
    }};
};

} // namespace Field
