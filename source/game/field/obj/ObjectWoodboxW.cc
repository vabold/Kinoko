#include "ObjectWoodboxW.hh"

#include "game/field/obj/ObjectWoodboxWSub.hh"

namespace Field {

/// @addr{0x8077DF24}
ObjectWoodboxW::ObjectWoodboxW(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_cooldownDuration(static_cast<s32>(params.setting(5))) {
    constexpr u16 DEFAULT_BOX_COUNT = 5;

    ObjectCollidable::init();

    s16 boxCount = (params.setting(6) == 0) ? DEFAULT_BOX_COUNT : params.setting(6);
    m_boxes = std::span<ObjectWoodboxWSub *>(new ObjectWoodboxWSub *[boxCount], boxCount);

    for (auto *&box : m_boxes) {
        box = new ObjectWoodboxWSub(params);
        box->load();
    }
}

/// @addr{0x8077E120}
ObjectWoodboxW::~ObjectWoodboxW() {
    delete[] m_boxes.data();
}

/// @addr{0x8077E1A0}
void ObjectWoodboxW::init() {
    ASSERT(m_mapObj);
    s32 frames = static_cast<s32>(m_mapObj->setting(4));
    if (frames == 0) {
        frames = m_cooldownDuration;
    }

    m_framesUntilSpawn = frames;
    m_nextBoxIdx = 0;
}

/// @addr{0x8077E1E4}
void ObjectWoodboxW::calc() {
    if (--m_framesUntilSpawn >= 1) {
        return;
    }

    m_framesUntilSpawn = m_cooldownDuration;
    m_boxes[m_nextBoxIdx]->enableCollision();
    m_nextBoxIdx = (m_nextBoxIdx + 1) % m_boxes.size();
}

} // namespace Field
