#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief The falling rocks on GCN DK Mountain
/// @details Rocks move along a rail when they are tangible. When they collide with a floor, an
/// inelastic bounce occurs while increasing the rock's @ref m_angSpd. Rocks become intangible (aka
/// despawn) when they reach the end of the rail path, or, if the rock's scale is less than `1.0f`,
/// the rock breaks and becomes intangible when a kart collides with it. Rocks with a scale less
/// than `1.0f` do not impart any reaction on the kart.
class ObjectRock final : public ObjectCollidable {
public:
    ObjectRock(const System::MapdataGeoObj &params);
    ~ObjectRock() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

    void calcCollisionTransform() override;
    [[nodiscard]] const EGG::Vector3f &getCollisionTranslation() const override;
    [[nodiscard]] Kart::Reaction onCollision(Kart::KartObject *kartObj,
            Kart::Reaction reactionOnKart, Kart::Reaction reactionOnObj,
            EGG::Vector3f &hitDepth) override;

private:
    /// @brief Whether the rock is collidable/visible
    enum class State {
        Tangible = 0,   ///< The rock is spawned and rolling
        Intangible = 1, ///< The rock is despawned
    };

    void enterTangible();
    void calcTangible();
    void calcIntangible();
    void calcTangibleSub();
    void breakRock();
    void checkSphereFull();

    State m_state;       ///< Current tangibility of the rock
    f32 m_startYPos;     ///< Starting y-axis position
    EGG::Vector3f m_vel; ///< Current velocity of the rock
    f32 m_angSpd;        ///< Angular speed in degrees per frame
    s32 m_cooldownTimer; ///< Cooldown timer before rock becomes tangible again
    f32 m_angRad;        ///< Rotation from accumulated angular velocity

    /// @brief Whether the transform still needs to be updated from @ref m_angRad and the rail
    /// @details OPT: The transform is only read when checking collision against the kart, so @ref
    /// calcTangibleSub() defers computing it to @ref calcCollisionTransform().
    bool m_transformPending;

    const s32 m_cooldownDuration; ///< Duration of the cooldown before becoming tangible again
    const f32 m_railSpeed;        ///< Speed at which the rock moves along the rail
    const f32 m_initVelY;         ///< Initial upwards Y velocity when the rock becomes tangible

    static constexpr f32 INITIAL_ANGULAR_SPEED = 3.0f; ///< The initial angular velocity of the rock
    static constexpr f32 ROCK_RADIUS = 240.0f;         ///< Radius of the rock
};

} // namespace Kinoko::Field
