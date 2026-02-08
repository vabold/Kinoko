#pragma once

#include "game/field/obj/ObjectCollidable.hh"

#include "game/field/StateManager.hh"

namespace Field {

/// @brief The individual minecarts that spawn from the @ref ObjectTruckWagon spawner.
class ObjectTruckWagonCart final : public ObjectCollidable, public StateManager {
public:
    ObjectTruckWagonCart(const System::MapdataGeoObj &params);
    ~ObjectTruckWagonCart() override;

    /// @addr{0x806E0160}
    void init() override {}

    void calc() override;

    /// @addr{0x806E260C}
    [[nodiscard]] const char *getName() const override {
        return "TruckWagon";
    }

    /// @addr{0x806E2744}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806E2618}
    [[nodiscard]] const char *getKclName() const override {
        return "TruckWagon";
    }

    void calcCollisionTransform() override;
    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    /// @addr{0x806E2060}
    void setActive(bool isSet) {
        m_active = isSet;
    }

    /// @addr{0x806E24FC}
    void activate() {
        reset(0);
        setActive(true);
        loadAABB(0.0f);
    }

    /// @addr{0x806E1F20}
    void deactivate() {
        reset(0);
        setActive(false);
        unregisterCollision();
        m_nextStateId = 0;
    }

    void reset(u32 idx);

    [[nodiscard]] bool isActive() const {
        return m_active;
    }

private:
    /// @addr{0x806E0D48}
    void enterState0() {}

    /// @addr{0x806E12D0}
    void enterState1() {}

    /// @addr{0x806E17F4}
    void enterState2() {}

    /// @addr{0x806E1898}
    void enterState3() {}

    void calcState0();
    void calcState1();

    /// @addr{0x806E1894}
    void calcState2() {
        calcState0();
    }

    /// @addr{0x806E1940}
    void calcState3() {}

    bool m_active; ///< Whether or not the minecart is spawned and has collision
    EGG::Vector3f m_vel;
    f32 m_speed;
    EGG::Vector3f m_lastVel;
    EGG::Vector3f m_up;
    EGG::Vector3f m_tangent;
    f32 m_pitch;
    f32 m_angVel;

    static constexpr std::array<StateManagerEntry, 4> STATE_ENTRIES = {{
            {StateEntry<ObjectTruckWagonCart, &ObjectTruckWagonCart::enterState0,
                    &ObjectTruckWagonCart::calcState0>(0)},
            {StateEntry<ObjectTruckWagonCart, &ObjectTruckWagonCart::enterState1,
                    &ObjectTruckWagonCart::calcState1>(1)},
            {StateEntry<ObjectTruckWagonCart, &ObjectTruckWagonCart::enterState2,
                    &ObjectTruckWagonCart::calcState2>(2)},
            {StateEntry<ObjectTruckWagonCart, &ObjectTruckWagonCart::enterState3,
                    &ObjectTruckWagonCart::calcState3>(3)},
    }};
};

/// @brief The "spawner" for minecarts on Wario's Gold Mine.
/// @details Each cycle represents two minecart spawns. The first cart spawns at the start of the
/// cycle, the second cart spawns at @ref m_spawn2Frame, and the cycle ends at @ref m_cycleDuration.
/// @ref m_curCartIdx keeps track of which ObjectTruckWagonCart object is spawning next.
class ObjectTruckWagon final : public ObjectCollidable {
public:
    ObjectTruckWagon(const System::MapdataGeoObj &params);
    ~ObjectTruckWagon() override;

    void init() override;
    void calc() override;

    /// @addr{0x806E24EC}
    [[nodiscard]] u32 loadFlags() const override {
        return 3;
    }

    /// @addr{0x806E24E8}
    void loadGraphics() override {}

    /// @addr{0x806E24E0}
    void createCollision() override {}

    /// @addr{0x806E24E4}
    void loadRail() override {}

private:
    std::span<ObjectTruckWagonCart *> m_carts;
    const s32 m_spawn2Frame;   ///< Frame that the second minecart in a cycle spawns
    const s32 m_cycleDuration; ///< Total duration of a cycle
    s32 m_cycleFrame;          ///< Current frame modulo cycle duration
    s32 m_curCartIdx;          ///< Index into @ref m_carts representing the next cart to spawn
};

} // namespace Field
