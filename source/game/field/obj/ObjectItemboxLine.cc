#include "ObjectItemboxLine.hh"

#include "game/field/obj/ObjectItemboxPress.hh"
#include "game/field/obj/ObjectPress.hh"

namespace Field {

/// @addr{0x8076D044}
ObjectItemboxLine::ObjectItemboxLine(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_cooldownDuration(static_cast<s32>(params.setting(5))) {
    constexpr u32 DEFAULT_PRESS_COUNT = 5;

    auto *senko = new ObjectPressSenko(params);
    senko->load();

    s32 pressCount = (params.setting(6) == 0) ? DEFAULT_PRESS_COUNT : params.setting(6);
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
    ASSERT(m_mapObj);
    s32 timer = static_cast<s32>(m_mapObj->setting(4));
    if (timer == 0) {
        timer = m_cooldownDuration;
    }

    m_stompCooldown = timer;
    m_curPressIdx = 0;
}

/// @addr{0x8076D64C}
void ObjectItemboxLine::calc() {
    if (--m_stompCooldown > 0) {
        return;
    }

    m_stompCooldown = m_cooldownDuration;
    m_press[m_curPressIdx]->startPress();
    m_curPressIdx = (m_curPressIdx + 1) % m_press.size();
}

} // namespace Field
