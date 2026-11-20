#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief The fireball shot by the giant Bowser statue in the half-pipe section of Bowser's Castle
/// @details The fireball is shot out with an initial speed of @ref INITIAL_SPEED with a vertical
/// speed of @ref INITIAL_Y_SPEED. Once it reaches the end of the rail, it begins exploding.
/// The explosion lasts a duration of @ref m_animDuration, where collision is disabled for the
/// last 20 frames of the explosion.
class ObjectKoopaBall final : public ObjectCollidable {
public:
    ObjectKoopaBall(const System::MapdataGeoObj &params);
    ~ObjectKoopaBall() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;
    [[nodiscard]] const EGG::Vector3f &getCollisionTranslation() const override;

private:
    /// @brief Describes the current collision state of the fireball
    enum class State {
        Tangible = 0,   ///< The fireball is active and can collide with karts
        Intangible = 1, ///< The fireball is inactive and cannot collide with karts
        Exploding = 2,  ///< The fireball is exploding and will throw the player upwards
    };

    void initCooldownTimer();
    void calcTangible();
    void calcExploding();
    void calcIntangible();
    void calcRot();
    void calcSlowdown();
    void setExplosionScale();
    void resetScale();
    void checkSphereFull();

    State m_state;                      ///< Current collision state of the fireball
    f32 m_initPosY;                     ///< Starting height
    EGG::Vector3f m_vel;                ///< Current velocity of the fireball
    f32 m_angSpeed;                     ///< Angular speed of the fireball in degrees per frame
    s32 m_cooldownTimer;                ///< Frames until the fireball will be fired
    Render::DrawMdl *m_bombCoreDrawMdl; ///< DrawMdl for the bomb animation
    s32 m_explodeTimer;                 ///< Remaining explosion duration
    u32 m_animDuration;                 ///< Explosion animation duration defined in bombCore.brres
    f32 m_angleRad;                     ///< Total angle of rotation in radians per frame
    f32 m_curScale;                     ///< Current scale of the fireball

    static constexpr f32 RADIUS_AABB = 870.0f * 2.0f; ///< Radius of the BoxColUnit

    /// @brief Scale when the fireball starts exploding (slightly smaller than before explosion)
    static constexpr f32 INITIAL_EXPLOSION_SCALE = RADIUS_AABB / 940.0f * 0.5f;

    /// @brief Rate at which the explosion scale increases per frame
    static constexpr f32 SCALE_DELTA = INITIAL_EXPLOSION_SCALE / 25.0f;

    static constexpr f32 INITIAL_SPEED = 400.0f;       ///< Initial speed of the fireball
    static constexpr f32 INITIAL_ANGULAR_SPEED = 3.0f; ///< Initial angular speed in degrees
    static constexpr f32 INITIAL_Y_SPEED = -30.0f;     ///< Initial vertical speed upon firing
};

} // namespace Kinoko::Field
