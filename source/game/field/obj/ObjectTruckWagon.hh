#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief The individual minecarts that spawn from the @ref ObjectTruckWagon spawner
/// @details Carts are toggled active or inactive by the @ref ObjectTruckWagon spawner object. When
/// they are active, they move along a rail. In the base game, they start by rolling along the
/// ground. When they are between rail nodes 3 and 4, they diverge from the rail's vertical position
/// in order to simulate falling near the spawner on Wario's Gold Mine. Once they reach the fork at
/// the end of the mine on Wario's Gold Mine, they get picked up and move along the rail while
/// suspended mid-air.
class ObjectTruckWagonCart final : public ObjectCollidable, private StateManager {
public:
    ObjectTruckWagonCart(const System::MapdataGeoObj &params);
    ~ObjectTruckWagonCart() override;

    void calc() override;
    void calcCollisionTransform() override;
    [[nodiscard]] const char *getName() const override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getKclName() const override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    void activate();
    void deactivate();
    void reset(u32 idx);

    /// @beginGetters

    [[nodiscard]] bool isActive() const;

    /// @endGetters

    /// @beginSetters

    void setActive(bool isSet);

    /// @endSetters

private:
    void calcRolling();
    void calcSuspended();
    void calcState2();

    void checkRailPointState();
    void calcRailAndVel();

    bool m_active;           ///< Whether or not the minecart is spawned and has collision
    EGG::Vector3f m_vel;     ///< Current velocity along the rail, with gravity applied
    f32 m_speed;             ///< Speed of the current segment of the rail interpolator
    EGG::Vector3f m_lastVel; ///< Last frame's velocity, used during the suspended state
    EGG::Vector3f m_up;      ///< Up direction vector
    EGG::Vector3f m_tangent; ///< Forward direction vector
    f32 m_pitch;             ///< Current rocking angle of the suspended minecart
    f32 m_angVel;            ///< Angular velocity of the swing of the suspended minecart

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 4> STATE_ENTRIES = {{
            {StateEntry<ObjectTruckWagonCart, nullptr, &ObjectTruckWagonCart::calcRolling>(0)},
            {StateEntry<ObjectTruckWagonCart, nullptr, &ObjectTruckWagonCart::calcSuspended>(1)},
            {StateEntry<ObjectTruckWagonCart, nullptr, &ObjectTruckWagonCart::calcState2>(2)},
            {StateEntry<ObjectTruckWagonCart, nullptr, nullptr>(3)},
    }};
};

/// @brief The "spawner" for minecarts on Wario's Gold Mine
/// @details Each cycle represents two minecart spawns. The first cart spawns at the start of the
/// cycle, the second cart spawns at @ref m_spawn2Frame, and the cycle ends at @ref m_spawn1Frame.
/// @ref m_curCartIdx keeps track of which ObjectTruckWagonCart object is spawning next.
class ObjectTruckWagon final : public ObjectCollidable {
public:
    ObjectTruckWagon(const System::MapdataGeoObj &params);
    ~ObjectTruckWagon() override;

    void init() override;
    void calc() override;

    void loadGraphics() override;
    void createCollision() override;
    void loadRail() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    owning_span<ObjectTruckWagonCart *> m_carts; ///< Pointers to each cart that spawns
    const s32 m_spawn2Frame; ///< Frame delay before the second minecart in a cycle spawns
    const s32 m_spawn1Frame; ///< Frame delay before the first minecart spawns, ending the cycle
    s32 m_cycleFrame;        ///< Current frame modulo cycle duration
    s32 m_curCartIdx;        ///< Index into @ref m_carts representing the next cart to spawn
};

} // namespace Kinoko::Field
