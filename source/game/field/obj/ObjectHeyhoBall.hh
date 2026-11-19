#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectProjectile.hh"

namespace Kinoko::Field {

/// @brief The cannonball projectiles on GBA Shy Guy Beach
/// @details Cannonballs are fired from a @ref ObjectHeyhoShip and have a parabolic flight path.
/// When they land, they blink for a few seconds before exploding. Before exploding, they act as
/// wall collision. The explosion can either knock the player into the air and make them lose their
/// items or just spin them out without losing items, depending on how much time has elapsed since
/// the explosion. The synchronization between a cannonball and the ship is managed by @ref
/// ObjectHeyhoShipManager.
class ObjectHeyhoBall final : public ObjectProjectile, private StateManager {
public:
    ObjectHeyhoBall(const System::MapdataGeoObj &params);
    ~ObjectHeyhoBall() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

    [[nodiscard]] Kart::Reaction onCollision(Kart::KartObject *kartObj,
            Kart::Reaction reactionOnKart, Kart::Reaction reactionOnObj,
            EGG::Vector3f &hitDepth) override;

    void initProjectile(const EGG::Vector3f &pos) override;
    void onLaunch() override;

private:
    /// @brief Describes the intensity of the explosion when a kart collides with the explosion
    enum class ExplosionIntensity {
        ExplosionLoseItem = 0, ///< The kart loses its items and is knocked upwards
        SpinSomeSpeed = 1,     ///< The kart spins out but keeps its items
    };

    void enterFalling();
    void enterBlinking();

    void calcIntangible();
    void calcFalling();
    void calcBlinking();
    void calcExploding();

    void calcFinishedExplodingScale();
    void calcExplodingScale();

    const f32 m_airtime;            ///< Number of frames between shooting and landing
    EGG::Vector3f m_shipPos;        ///< Position of the ship firing the ball
    const EGG::Vector3f m_initPos;  ///< Target landing position
    EGG::Vector3f m_xzDir;          ///< XZ direction of the cannonball's flight path
    f32 m_xzSpeed;                  ///< XZ speed of the cannonball's flight path
    f32 m_initYSpeed;               ///< Initial vertical projectile speed
    f32 m_blastRadiusRatio;         ///< Ratio between the blast radius and the shell's radius
    f32 m_scaleChangeRate;          ///< Coefficient of the explosion scale's parabola
    EGG::Vector3f m_workingPos;     ///< Intermediate position of the cannonball
    ExplosionIntensity m_intensity; ///< Collision reaction strength

    static constexpr f32 BALL_RADIUS = 50.0f;            ///< Radius of the cannonball
    static constexpr f32 INIT_BLAST_RADIUS = 1500.0f;    ///< Initial radius of the explosion
    static constexpr f32 EXPLOSION_VERTEX_FRAME = 40.0f; ///< Frame of the explosion scale's peak

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 4> STATE_ENTRIES = {{
            {StateEntry<ObjectHeyhoBall, nullptr, &ObjectHeyhoBall::calcIntangible>(0)},
            {StateEntry<ObjectHeyhoBall, &ObjectHeyhoBall::enterFalling,
                    &ObjectHeyhoBall::calcFalling>(1)},
            {StateEntry<ObjectHeyhoBall, &ObjectHeyhoBall::enterBlinking,
                    &ObjectHeyhoBall::calcBlinking>(2)},
            {StateEntry<ObjectHeyhoBall, nullptr, &ObjectHeyhoBall::calcExploding>(3)},
    }};
};

} // namespace Kinoko::Field
