#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko {

namespace Host {

class Context;

} // namespace Host

namespace Field {

/// @brief Represents a single Swooper (bat). It's owned and managed by @ref ObjectBasabasa.
/// @note This class normally inherits from @ref StateManager. However, the bat's second state is
/// only ever used when the bat is hit by a star or a shock. For the purposes of Kinoko, we can
/// simplify by removing the inheritance and just calling @ref calcState() directly in @ref calc().
class ObjectBasabasaDummy final : public ObjectCollidable {
public:
    ObjectBasabasaDummy(const System::MapdataGeoObj &params);
    ~ObjectBasabasaDummy() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] virtual const char *getKclName() const override;
    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    /// @beginSetters

    /// @brief Sets whether the bat is spawned
    /// @param isSet Whether the bat should be active or not
    void setActive(bool isSet) {
        m_active = isSet;
    }

    /// @endSetters

    /// @beginGetters

    /// @brief Checks whether the bat is currently spawned
    /// @return `true` if the bat is active, `false` otherwise
    [[nodiscard]] bool active() const {
        return m_active;
    }

    /// @endGetters

private:
    void calcState();

    const bool m_bigBump;      ///< Affects the severity of the "push" when colliding with bat
    EGG::Vector3f m_posOffset; ///< RNG-based starting position offset for the bat
    bool m_active;             ///< Whether or not this bat is currently spawned
};

/// @brief Can be thought of as the Swooper (bat) "spawner". Manages an array of bats.
/// @details Spawns bats in small groups, whose size is determined by @ref m_batsPerGroup. After
/// that many bats have been spawned, it resets the @ref m_cycleTimer.
class ObjectBasabasa final : public ObjectCollidable {
    /// @brief Grants access to static member variables so a @ref Host::Context can restore their
    /// values on context switch
    friend Host::Context;

public:
    ObjectBasabasa(const System::MapdataGeoObj &params);
    ~ObjectBasabasa() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    void createCollision() override;
    void loadRail() override;

    /// @beginGetters

    /// @brief Exposes the range of initial X positions so the dummy can access it
    /// @return The range of initial X positions for the bats
    [[nodiscard]] static f32 InitialXRange() {
        return s_initialXRange;
    }

    /// @brief Exposes the range of initial Y positions so the dummy can access it
    /// @return The range of initial Y positions for the bats
    [[nodiscard]] static f32 InitialYRange() {
        return s_initialYRange;
    }

    /// @endGetters

private:
    owning_span<ObjectBasabasaDummy *> m_bats; ///< The array of individual bats
    const u32 m_initialTimer; ///< The @ref m_cycleTimer starts and resets to this value
    const u32 m_batsPerGroup; ///< Number of bats that spawn before resetting @ref m_cycleTimer
    const u32 m_startFrame;   ///< Initial delay before the spawner will start calculating
    const u32 m_batSpacing;   ///< How many frames in between bat spawns
    u32 m_cycleTimer;         ///< Used to determine when to spawn next bat
    u32 m_batsActive;         ///< The number of bats currently spawned

    /// @addr{0x809C2200}
    /// @brief Range of random offsets for the bats' initial X-axis position
    static f32 s_initialXRange;

    /// @addr{0x809C2204}
    /// @brief Range of random offsets for the bats' initial Y-axis position
    static f32 s_initialYRange;
};

} // namespace Field

} // namespace Kinoko
