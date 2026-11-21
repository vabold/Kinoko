#pragma once

#include "game/field/obj/ObjectBase.hh"

namespace Kinoko::Field {

/// @brief Represents a falling block on SNES Ghost Valley 2
/// @details Most @ref ObjectBase virtual functions are no-ops because collision is handled by @ref
/// ObjectObakeManager, which manages the lifecycle of these blocks in a spatially indexed cache.
/// The block's second and third param settings define the number of seconds and additional frames
/// to wait before falling. If these are both zero, then the block never falls.
class ObjectObakeBlock final : public ObjectBase {
public:
    /// @brief Represents the current state of the block in its falling lifecycle
    enum class FallState {
        Rest = 0,            ///< The block is stationary and has not yet started falling
        Falling = 1,         ///< The block is currently falling
        FinishedFalling = 2, ///< The block has finished falling and is now stationary
    };

    ObjectObakeBlock(const System::MapdataGeoObj &params);
    ~ObjectObakeBlock() override;

    void calc() override;
    void load() override;
    void createCollision() override;
    void calcCollisionTransform() override;

    /// @beginSetters

    /// @brief Sets the block's fall state
    /// @param state The new fall state to set for the block
    void setFallState(FallState state) {
        m_fallState = state;
    }

    /// @endSetters

    /// @beginGetters

    /// @brief Gets the block's current fall state
    /// @return The current fall state of the block, @ref m_fallState
    [[nodiscard]] FallState fallState() const {
        return m_fallState;
    }

    /// @brief Gets the frame the block starts falling, or zero if the block never falls
    /// @return The value of @ref m_fallFrame
    [[nodiscard]] s32 fallFrame() const {
        return m_fallFrame;
    }

    /// @endGetters

private:
    const EGG::Vector3f m_initPos; ///< Initial position of the block
    FallState m_fallState;         ///< Current state of the block in its falling lifecycle
    s32 m_framesFallen;            ///< How long the block has been falling for, capped at 256
    EGG::Vector3f m_xzFallVel;     ///< Lateral velocity while the block is falling
    EGG::Vector3f m_fallAngVel;    ///< Angular velocity while the block is falling
    const s32 m_fallFrame;         ///< Frame the block starts falling, or 0 if it never falls
};

} // namespace Kinoko::Field
