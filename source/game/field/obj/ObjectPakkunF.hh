#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief Represents the piranhas on GCN Mario Circuit.
/// @details This object does not include the pipe that is normally found underneath the piranha.
class ObjectPakkunF final : public ObjectCollidable {
public:
    ObjectPakkunF(const System::MapdataGeoObj &params);
    ~ObjectPakkunF() override;

    void init() override;
    void calc() override;

    /// @addr{0x807754F4}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    void loadAnims() override;
    void calcCollisionTransform() override;

    /// @addr{0x80775458}
    [[nodiscard]] f32 getCollisionRadius() const override {
        return 1000.0f;
    }

private:
    enum class State {
        Wait = 0,
        Attack = 1,
    };

    void calcWait();
    void calcAttack();
    void enterAttack();

    State m_state;
    s32 m_waitFrames;      ///< How long until the piranha starts chomping
    s32 m_attackFrames;    ///< How long until the piranha stops chomping
    u32 m_currAttackFrame; /// How long the piranhas has been attacking for

    /// Total time the piranha will be idle for. Not in the base game, but this prevents
    /// having to dereference m_mapObj multiple times.
    const s32 m_waitDuration;
};

} // namespace Field
