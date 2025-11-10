#pragma once

#include "game/field/obj/ObjectDossun.hh"

namespace Field {

/// @brief Represents Thwomps that move sideways on the ground on rBC.
class ObjectDossunYokoMove final : public ObjectDossun {
public:
    /// @addr{0x80763B60}
    ObjectDossunYokoMove(const System::MapdataGeoObj &params) : ObjectDossun(params) {}

    /// @addr{0x807645C4}
    ~ObjectDossunYokoMove() override = default;

    /// @addr{0x80763C14}
    void init() override {
        m_railInterpolator->init(0.0f, 0);
    }

    /// @addr{0x80763E0C}
    void calc() override {
        m_railInterpolator->calc();
        m_flags.setBit(eFlags::Position);
        m_pos = m_railInterpolator->curPos();
    }
};

} // namespace Field
