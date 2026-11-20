#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko {

namespace Kart {

class KartObject;
enum class Reaction;

} // namespace Kart

namespace Field {

/// @brief Represents the walking Chain Chomps on DS Peach Gardens
/// @details Chain Chomps move along a rail and bounce upwards whenever they collide with the
/// floor.
class ObjectHwanwan final : public ObjectCollidable, private StateManager {
    /// @brief Grants the manager class access to the Chain Chomp's state
    friend class ObjectHwanwanManager;

public:
    ObjectHwanwan(const System::MapdataGeoObj &params);
    ~ObjectHwanwan() override;

    void init() override;
    void calc() override;
    void loadRail() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getKclName() const override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) override;

private:
    void calcBounce();
    void checkFloorCollision();
    void calcUp();

    const EGG::Vector3f m_initPos; ///< Spawn position
    EGG::Vector3f m_workPos;       ///< Staged position for this frame
    EGG::Vector3f m_extVel;        ///< External velocity (gravity and bounce velocity)
    EGG::Vector3f m_bounceVel;     ///< Upwards velocity impulse when Chain Chomp contacts the floor
    EGG::Vector3f m_tangent;       ///< Forward direction along the rail
    EGG::Vector3f m_up;            ///< Smoothed up vector
    EGG::Vector3f m_targetUp;      ///< Target up vector, which is the normal of the colliding floor
    bool m_touchingGround;         ///< Whether the Chain Chomp is currently touching the floor
    f32 m_targetY; ///< Rail height, used to skip collision checks when far enough in the air

    static constexpr f32 DIAMETER = 400.0f; ///< The diameter of the Chain Chomp's collision sphere

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 2> STATE_ENTRIES = {{
            {StateEntry<ObjectHwanwan, nullptr, &ObjectHwanwan::calcBounce>(0)},
            {StateEntry<ObjectHwanwan, nullptr, nullptr>(1)},
    }};
};

/// @brief Manager class for @ref ObjectHwanwan
/// @details For Kinoko, this is effectively a very simple wrapper over a single @ref ObjectHwanwan.
/// However, in the base game, this manager class is responsible for managing a trail of item boxes
/// behind the Chain Chomp.
class ObjectHwanwanManager final : public ObjectCollidable {
public:
    ObjectHwanwanManager(const System::MapdataGeoObj &params);
    ~ObjectHwanwanManager() override;

    void init() override;
    void calc() override;
    void loadGraphics() override;
    void createCollision() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    void calcState();
    void calcPosAndTangent();

    ObjectHwanwan *m_hwanwan; ///< Pointer to the underlying Chain Chomp object
};

} // namespace Field

} // namespace Kinoko
