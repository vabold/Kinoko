#include "ObjectWoodboxW.hh"

#include "game/field/obj/ObjectWoodboxWSub.hh"

namespace Field {

/// @addr{0x8077DF24}
ObjectWoodboxW::ObjectWoodboxW(const System::MapdataGeoObj &params) : ObjectCollidable(params) {
    constexpr u16 DEFAULT_BOX_COUNT = 5;

    ObjectCollidable::init();

    u16 boxCount = params.setting(6);

    if (boxCount == 0) {
        boxCount = DEFAULT_BOX_COUNT;
    }

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
    u32 frames = m_mapObj->setting(4);
    if (frames == 0) {
        frames = m_mapObj->setting(5);
    }

    m_framesUntilSpawn = frames;
    m_nextBoxIdx = 0;
}

/// @addr{0x8077E1E4}
void ObjectWoodboxW::calc() {
    if (--m_framesUntilSpawn >= 1) {
        return;
    }

    m_framesUntilSpawn = m_mapObj->setting(5);
    m_boxes[m_nextBoxIdx]->enableCollision();
    m_nextBoxIdx = (m_nextBoxIdx + 1) % m_boxes.size();
}

} // namespace Field
