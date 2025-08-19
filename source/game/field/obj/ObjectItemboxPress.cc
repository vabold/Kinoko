#include "ObjectItemboxPress.hh"

#include "game/field/obj/ObjectPress.hh"

namespace Field {

/// @addr{0x8076D9E4}
ObjectItemboxPress::ObjectItemboxPress(const System::MapdataGeoObj &params)
    : ObjectCollidable(params) {}

/// @addr{0x8076DA48}
ObjectItemboxPress::~ObjectItemboxPress() = default;

/// @addr{0x8076DAF4}
void ObjectItemboxPress::calc() {
    switch (m_state) {
    case 1:
    case 2:
        calcPosition();
        break;
    default:
        break;
    }
}

/// @brief Used by @ref ObjectItemboxLine to activate the stomper.
void ObjectItemboxPress::startPress() {
    m_state = 2;
    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setPerPointVelocities(true);
}

/// @addr{0x8076DF44}
void ObjectItemboxPress::calcPosition() {
    constexpr f32 HEIGHT_OFFSET = 180.0f;

    auto result = m_railInterpolator->calc();

    if (result == RailInterpolator::Status::SegmentEnd) {
        if (m_railInterpolator->curPoint().setting[1] == 1) {
            m_senko->setWindup(true);
        }
    } else if (result == RailInterpolator::Status::ChangingDirection) {
        m_state = 0;
    }

    m_pos = m_railInterpolator->curPos();
    m_pos.y = HEIGHT_OFFSET + m_pos.y;
    m_flags.setBit(eFlags::Position);
}

} // namespace Field
