#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectProjectile.hh"

namespace Kinoko::Field {

/// @brief Represents the smaller fireballs that trail behind a @ref ObjectFireSnake
class ObjectFireSnakeKid final : public ObjectCollidable {
public:
    ObjectFireSnakeKid(const System::MapdataGeoObj &params);
    ~ObjectFireSnakeKid() override;

    [[nodiscard]] LoadFlags loadFlags() const override;
};

/// @brief Represents the bouncing fire snakes on DS Desert Hills and is the base class for the @ref
/// ObjectFireSnakeV objects on Grumble Volcano
/// @details Each firesnake has two children that follow behind. The children's position is set by
/// copying the transformation matrix of the parent snake from previous frames (10 frames ago for
/// the first child and 20 frames ago for the second child). @ref ObjectFireSnakeV manages its own
/// state lifecycle, whereas @ref ObjectFireSnake instances rely on @ref ObjectSunDS to advance the
/// state cycle by launching the fire snake and are registered as managed objects so that @ref
/// ObjectSunManager can fetch the FireSnakes.
class ObjectFireSnake : public ObjectProjectile, virtual protected StateManager {
public:
    ObjectFireSnake(const System::MapdataGeoObj &params);
    ~ObjectFireSnake() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

    void initProjectile(const EGG::Vector3f &pos) override;
    void onLaunch() override;

protected:
    void enterDespawned();
    void enterFalling();
    void enterHighBounce();
    void enterRest();

    void calcFalling();
    void calcHighBounce();
    void calcRest();
    void calcBounce();

    void calcChildren();

    EGG::Vector3f m_spawnPos;      ///< Starting position when a fire snake begins falling
    EGG::Vector3f m_initPos;       ///< Landing position after spawning
    EGG::Vector3f m_initRot;       ///< Initial rotation
    EGG::Vector3f m_trajectoryPos; ///< True position used for physics, not affected by spiral fall
    EGG::Vector3f m_bounceDir;     ///< Direction of current bounce
    u16 m_age;                     ///< How long the firesnake has been spawned

private:
    void calcBounce(f32 initialVel);
    [[nodiscard]] bool isCollisionEnabled() const;

    std::array<ObjectFireSnakeKid *, 2> m_kids; ///< Pointers to the smaller fireball followers
    const s16 m_maxAge;                         ///< FireSnake despawns after this number of frames
    EGG::Vector3f m_xzSunDist;                  ///< Distance between m_spawnPos and m_initPos
    EGG::Vector3f m_fallDir;                    ///< Direction of fall from the sun
    f32 m_xzFallSpeed;                          ///< Speed along the xz-plane while falling
    u16 m_fallDuration;                         ///< How long the firesnake falls from the sun
    std::array<EGG::Matrix34f, 21> m_prevTransforms; ///< The last 21 transformation matrices

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 6> STATE_ENTRIES = {{
            {StateEntry<ObjectFireSnake, &ObjectFireSnake::enterDespawned, nullptr>(0)},
            {StateEntry<ObjectFireSnake, &ObjectFireSnake::enterFalling,
                    &ObjectFireSnake::calcFalling>(1)},
            {StateEntry<ObjectFireSnake, &ObjectFireSnake::enterHighBounce,
                    &ObjectFireSnake::calcHighBounce>(2)},
            {StateEntry<ObjectFireSnake, &ObjectFireSnake::enterRest, &ObjectFireSnake::calcRest>(
                    3)},
            {StateEntry<ObjectFireSnake, nullptr, &ObjectFireSnake::calcBounce>(4)},
            {StateEntry<ObjectFireSnake, nullptr, nullptr>(5)},
    }};

    static constexpr f32 GRAVITY = 3.0f;          ///< Gravitational constant of acceleration
    static constexpr f32 BOUNCE_VELOCITY = 60.0f; ///< Initial upwards speed of a bounce
};

} // namespace Kinoko::Field
