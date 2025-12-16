#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief Represents a single bat. It's owned and managed by @ref ObjectBasabasa.
class ObjectBasabasaDummy final : public ObjectCollidable, public StateManager {
public:
    ObjectBasabasaDummy(const System::MapdataGeoObj &params);
    ~ObjectBasabasaDummy() override;

    void init() override;

    /// @addr{0x806B602C}
    void calc() override {
        StateManager::calc();
    }

    /// @addr{0x806B7700}
    [[nodiscard]] u32 loadFlags() const override {
        return 3;
    }

    /// @addr{0x806B76F4}
    [[nodiscard]] virtual const char *getKclName() const override {
        return "basabasa";
    }

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    void setActive(bool isSet) {
        m_active = isSet;
    }

    [[nodiscard]] bool active() const {
        return m_active;
    }

private:
    /// @addr{0x806B5C80}
    void enterState0() {}

    /// @addr{0x806B6288}
    /// @brief This is run when a bat is hit with a start or other item. We can ignore for Kinoko.
    void enterState1() {}

    void calcState0();

    /// @addr{0x806B652C}
    /// @brief This is run when a bat is hit with a start or other item. We can ignore for Kinoko.
    void calcState1() {}

    const bool m_bigBump;       ///< Affects the severity of the "push" when colliding with bat
    EGG::Vector3f m_initialPos; ///< RNG-based starting position for the bat
    bool m_active;              ///< Whether or not this bat is currently spawned

    static constexpr std::array<StateManagerEntry, 2> STATE_ENTRIES = {{
            {StateEntry<ObjectBasabasaDummy, &ObjectBasabasaDummy::enterState0,
                    &ObjectBasabasaDummy::calcState0>(0)},
            {StateEntry<ObjectBasabasaDummy, &ObjectBasabasaDummy::enterState1,
                    &ObjectBasabasaDummy::calcState1>(1)},
    }};
};

/// @brief Can be thought of as the bat "spawner". It's the class that manages an array of bats.
/// @details Spawns bats in small groups, whose size is determined by @ref m_batsPerGroup. After
/// that many bats have been spawned, it resets the @ref m_cycleTimer.
class ObjectBasabasa final : public ObjectCollidable {
public:
    ObjectBasabasa(const System::MapdataGeoObj &params);
    ~ObjectBasabasa() override;

    void init() override;
    void calc() override;

    /// @addr{0x806B7628}
    [[nodiscard]] u32 loadFlags() const override {
        return 3;
    }

    /// @addr{0x806B761C}
    void createCollision() override {}

    /// @addr{0x806B7620}
    void loadRail() override {}

private:
    std::span<ObjectBasabasaDummy *> m_bats; ///< The array of individual bats
    const s32 m_initialTimer;                ///< The m_cycleTimer starts and resets to this value
    const s32 m_batsPerGroup; ///< Number of bats that will spawn before resetting the m_cycleTimer
    const s32 m_startFrame;   ///< Initial delay before the spawner will start calculating
    const s32 m_batSpacing;   ///< How many frames in between bat spawns
    s32 m_cycleTimer;         ///< Used to determine when to spawn next bat
    s32 m_batsActive;         ///< The number of bats currently spawned
};

} // namespace Field
