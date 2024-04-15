#include "ItemDirector.hh"

#include "game/system/RaceConfig.hh"

namespace Item {

void ItemDirector::init() {
    for (auto &kart : m_karts) {
        kart.inventory().setItem(ItemId::TRIPLE_MUSHROOM);
    }
}

void ItemDirector::calc() {
    for (auto &kart : m_karts) {
        kart.calc();
    }
}

ItemDirector *ItemDirector::CreateInstance() {
    assert(!s_instance);
    s_instance = new ItemDirector;
    return s_instance;
}

void ItemDirector::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

ItemDirector *ItemDirector::Instance() {
    return s_instance;
}

ItemDirector::ItemDirector() {
    size_t playerCount = System::RaceConfig::Instance()->raceScenario().playerCount;
    m_karts = std::span<KartItem>(new KartItem[playerCount], playerCount);

    for (size_t i = 0; i < playerCount; ++i) {
        m_karts[i].init(i);
    }
}

ItemDirector::~ItemDirector() {
    delete[] m_karts.data();
}

ItemDirector *ItemDirector::s_instance = nullptr;

} // namespace Item
