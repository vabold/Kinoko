#include "KartItem.hh"

#include "game/kart/KartMove.hh"
#include "game/system/RaceManager.hh"

namespace Item {

KartItem::KartItem() = default;

KartItem::~KartItem() = default;

void KartItem::init(size_t playerIdx) {
    apply(playerIdx);
}

void KartItem::calc() {
    bool prevButton = m_bItemButtonHold;
    clearButtonFlags();

    const auto &currentInputs = inputs()->currentState();
    if (currentInputs.item()) {
        m_bItemButtonHold = true;
        m_bItemButtonActivation = !prevButton;
    }

    bool canUse = false;
    if (m_bItemButtonActivation) {
        canUse = true;
        if (!System::RaceManager::Instance()->isStageReached(System::RaceManager::Stage::Race)) {
            canUse = false;
        }

        if (m_inventory.id() == ItemId::NONE) {
            canUse = false;
        }
    }

    if (canUse) {
        // For now, assume only time trials are valid and use a mushroom
        useMushroom();
    }
}

void KartItem::activateMushroom() {
    move()->activateMushroom();
}

void KartItem::useMushroom() {
    activateMushroom();
    m_inventory.useItem(1);
}

ItemInventory &KartItem::inventory() {
    return m_inventory;
}

void KartItem::clearButtonFlags() {
    m_bItemButtonHold = false;
    m_bItemButtonActivation = false;
}

} // namespace Item
