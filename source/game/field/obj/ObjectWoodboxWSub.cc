#include "ObjectWoodboxWSub.hh"

namespace Field {

/// @addr{0x8077E34C}
ObjectWoodboxWSub::ObjectWoodboxWSub(const System::MapdataGeoObj &params) : ObjectWoodbox(params) {}

/// @addr{0x8077E388}
ObjectWoodboxWSub::~ObjectWoodboxWSub() = default;

/// @addr{0x8077E49C}
void ObjectWoodboxWSub::calc() {
    if (m_state - 1 > 1) {
        return;
    }

    calcPosition();
}

/// @addr{0x8077E56C}
void ObjectWoodboxWSub::calcPosition() {
    auto status = m_railInterpolator->calc();

    if (status == RailInterpolator::Status::ChangingDirection) {
        m_state = 0;
    }

    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);
}

} // namespace Field
