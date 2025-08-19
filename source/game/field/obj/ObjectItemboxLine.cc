#include "ObjectItemboxLine.hh"

#include "game/field/obj/ObjectItemboxPress.hh"
#include "game/field/obj/ObjectPress.hh"

namespace Field {

/// @addr{0x8076D044}
ObjectItemboxLine::ObjectItemboxLine(const System::MapdataGeoObj &params)
    : ObjectCollidable(params) {
    constexpr u32 DEFAULT_PRESS_COUNT = 5;

    auto *senko = new ObjectPressSenko(params);
    senko->load();

    u32 pressCount = params.setting(6);
    if (pressCount == 0) {
        pressCount = DEFAULT_PRESS_COUNT;
    }

    m_press = std::span<ObjectItemboxPress *>(new ObjectItemboxPress *[pressCount], pressCount);

    for (auto *&press : m_press) {
        press = new ObjectItemboxPress(params);
        press->load();
        press->setSenko(senko);
    }
}

/// @addr{0x8076D558}
ObjectItemboxLine::~ObjectItemboxLine() {
    // Individual objects' lifecycle is managed by the ObjectDirector.
    delete[] m_press.data();
}

/// @addr{0x8076D604}
void ObjectItemboxLine::init() {
    u32 timer = static_cast<u32>(m_mapObj->setting(4));

    if (timer == 0) {
        timer = static_cast<u32>(m_mapObj->setting(5));
    }

    m_stompCooldown = timer;
    m_curPressIdx = 0;
}

/// @addr{0x8076D64C}
void ObjectItemboxLine::calc() {
    if (--m_stompCooldown > 0) {
        return;
    }

    m_stompCooldown = static_cast<u32>(m_mapObj->setting(5));

    m_press[m_curPressIdx]->startPress();
    m_curPressIdx = (m_curPressIdx + 1) % m_press.size();
}

} // namespace Field
