#include "KartItem.hh"

#include "game/kart/KartMove.hh"
#include "game/system/RaceManager.hh"

namespace Item {

/// @addr{0x8079754C}
KartItem::KartItem() {
    clearButtonFlags();
}

/// @addr{0x8079951C}
KartItem::~KartItem() = default;

/// @addr{0x807976E0}
void KartItem::init(size_t playerIdx) {
    apply(playerIdx);
}

/// @brief Calculates item activation based on the controller input state
/// @addr{0x80797928}
void KartItem::calc() {
    bool prevButton = m_bItemButtonHold;
    clearButtonFlags();

    const auto &currentInputs = inputs()->currentState();
    if (currentInputs.item()) {
        m_bItemButtonHold = true;
        m_bItemButtonActivation = !prevButton;
    }

    const auto *raceMgr = System::RaceManager::Instance();
    bool canUse = m_bItemButtonActivation;
    canUse = canUse && raceMgr->isStageReached(System::RaceManager::Stage::Race);
    canUse = canUse && (m_inventory.id() != ItemId::NONE);

    if (canUse) {
        // For now, assume only time trials are valid and use a mushroom
        useMushroom();
    }
}

/// @addr{0x8079864C}
void KartItem::activateMushroom() {
    move()->activateMushroom();
}

/// @addr{0x807A9D3C}
void KartItem::useMushroom() {
    activateMushroom();
    m_inventory.useItem(1);
}

void KartItem::clearButtonFlags() {
    m_bItemButtonHold = false;
    m_bItemButtonActivation = false;
}

ItemInventory &KartItem::inventory() {
    return m_inventory;
}

} // namespace Item
