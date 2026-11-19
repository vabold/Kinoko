#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Snowboarding Shy Guys on DK Summit
/// @details The Shy Guys move along a rail and jump over the zipper. When they go off a zipper, all
/// color variants perform a 180, except for red Shy Guys which perform a 720. The gravitational
/// acceleration is calculated based on the apex, the midpoint (lowest point), and the maximum
/// velocity param setting.
class ObjectHeyho final : public ObjectCollidable, private StateManager {
public:
    ObjectHeyho(const System::MapdataGeoObj &params);
    ~ObjectHeyho() override;

    void init() override;
    void calc() override;
    void loadAnims() override;
    void calcCollisionTransform() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    /// @brief Current animation of the Shy Guy
    enum class Animation {
        Move = 1,   ///< On the ground
        Jump = 2,   ///< In the air
        Jumped = 3, ///< Landed after a jump
    };

    /// @brief The color of the Shy Guy
    enum class Color {
        Red = 0,    ///< Red Shy Guy that performs a 720
        Yellow = 1, ///< Yellow Shy Guy that performs a 180
        Green = 2,  ///< Green Shy Guy that performs a 180
    };

    void enterJump();
    void calcMove();
    void calcJump();

    void changeAnimation(Animation anim);
    void calcStateTransition();
    void calcMotion();
    void calcInterp();

    const Color m_color;      ///< Color of the Shy Guy, used to determine if it spins
    f32 m_apex;               ///< Highest Y position between the rail endpoints
    EGG::Vector3f m_midpoint; ///< Middle point (and lowest Y position) of the rail
    EGG::Vector3f m_initVel;  ///< Initial velocity vector
    f32 m_currentSpeed;       ///< Current speed along the rail
    f32 m_accel;              ///< Gravity based off apex, midpoint, and max velocity
    f32 m_maxVelSq;           ///< Square of the maximum velocity specified by param setting 1
    EGG::Vector3f m_up;       ///< Smoothed up vector
    EGG::Vector3f m_forward;  ///< Facing direction
    EGG::Vector3f m_floorNrm; ///< Floor normal from the most recent collision
    bool m_floorCollision;    ///< Whether the Shy Guy is currently colliding with the floor
    Animation m_currentAnim;  ///< Currently playing animation
    bool m_freeFall;          ///< Shy Guy has left the rail due to asymmetric endpoint heights
    f32 m_launchVel; ///< Speed at moment of rail direction change, used to snap back on landing
    s16 m_spinFrame; ///< Frame counter that ticks up while the Shy Guy is spinning mid-air

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 2> STATE_ENTRIES = {{
            {StateEntry<ObjectHeyho, nullptr, &ObjectHeyho::calcMove>(0)},
            {StateEntry<ObjectHeyho, &ObjectHeyho::enterJump, &ObjectHeyho::calcJump>(1)},
    }};

    /// @brief Height offset applied to the position passed into collision checks
    static constexpr EGG::Vector3f COLLISION_OFFSET = EGG::Vector3f(0.0f, 10.0f, 0.0f);

    static constexpr f32 COLLISION_RADIUS = 100.0f; ///< Radius of the collision sphere
};

} // namespace Kinoko::Field
