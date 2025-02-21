#include "ItemDirector.hh"

#include "game/system/RaceConfig.hh"

namespace Item {

/// @addr{0x80799794}
void ItemDirector::init() {
    for (auto &kart : m_karts) {
        kart.inventory().setItem(ItemId::TRIPLE_MUSHROOM);
    }
}

/// @addr{0x80799850}
void ItemDirector::calc() {
    for (auto &kart : m_karts) {
        kart.calc();
    }
}

/// @addr{0x80799138}
ItemDirector *ItemDirector::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new ItemDirector;
    return s_instance;
}

/// @addr{0x80799188}
void ItemDirector::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

/// @addr{0x807992D8}
ItemDirector::ItemDirector() {
    size_t playerCount = System::RaceConfig::Instance()->raceScenario().playerCount;
    m_karts = std::span<KartItem>(new KartItem[playerCount], playerCount);

    for (size_t i = 0; i < playerCount; ++i) {
        m_karts[i].init(i);
    }
}

/// @addr{0x80798F9C}
ItemDirector::~ItemDirector() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("ItemDirector instance not explicitly handled!");
    }

    delete[] m_karts.data();
}

ItemDirector *ItemDirector::s_instance = nullptr; ///< @addr{0x809C3618}

} // namespace Item
