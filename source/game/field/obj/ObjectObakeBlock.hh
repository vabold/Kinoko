#pragma once

#include "game/field/obj/ObjectBase.hh"

#include <egg/math/BoundBox.hh>

namespace Field {

class ObjectObakeBlock : public ObjectBase {
public:
    enum class FallState {
        Rest = 0,
        Falling = 1,
        FinishedFalling = 2,
    };

    ObjectObakeBlock(const System::MapdataGeoObj &params);
    ~ObjectObakeBlock() override;

    void calc() override;

    /// @addr{0x8080BDE0}
    void load() override {}

    /// @addr{0x8080BDDC}
    void createCollision() override {}

    /// @addr{0x8080BDD4}
    void calcCollisionTransform() override {}

    /// @beginSetters
    void setFallState(FallState state) {
        m_fallState = state;
    }
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] FallState fallState() const {
        return m_fallState;
    }

    [[nodiscard]] s32 fallFrame() const {
        return m_fallFrame;
    }
    /// @endGetters

private:
    const EGG::Vector3f m_initialPos; ///< Position is computed relative to initialPos.
    FallState m_fallState;
    s32 m_framesFallen; ///< How long the block has been falling for, capped at 256.
    EGG::Vector3f m_fallVel;
    EGG::Vector3f m_fallAngVel;
    EGG::BoundBox3f m_bbox;
    const s32 m_fallFrame; ///< Frame the block starts falling, or 0 if it never falls.
};

} // namespace Field
