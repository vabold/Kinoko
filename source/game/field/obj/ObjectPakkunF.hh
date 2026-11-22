#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Represents the piranhas on GCN Mario Circuit
/// @details These piranhas often exist above a pipe, but these piranha objects are separate from
/// the pipe object, which is a primitive @ref ObjectCollidable represented with @ref
/// ObjectId::PakkunDokan. Piranhas cycle between a waiting state and an attacking state. When they
/// attack, their collision transformation matrix is updated to reflect the piranha chomping forward
/// and downward.
class ObjectPakkunF final : public ObjectCollidable {
public:
    ObjectPakkunF(const System::MapdataGeoObj &params);
    ~ObjectPakkunF() override;

    void init() override;
    void calc() override;
    void loadAnims() override;
    void calcCollisionTransform() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] f32 getCollisionRadius() const override;

private:
    /// @brief Distinguishes between whether the piranha is idle or chomping
    enum class State {
        Wait = 0,   ///< The piranha is idle
        Attack = 1, ///< The piranha is chomping
    };

    void enterWait();
    void enterAttack();
    void calcWait();
    void calcAttack();
    void calcAttackTimer();

    State m_state;            ///< Tracks whether the piranha is idle or chomping
    s32 m_waitFrames;         ///< How long until the piranha starts chomping
    s32 m_attackFrames;       ///< How long until the piranha stops chomping
    u32 m_currAttackFrame;    ///< How long the piranha has been attacking for
    const s32 m_waitDuration; ///< Total time the piranha will be idle for
};

} // namespace Kinoko::Field
