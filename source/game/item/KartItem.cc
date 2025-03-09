#include "KartItem.hh"

#include "game/kart/KartMove.hh"
#include "game/kart/KartState.hh"

#include "game/system/RaceManager.hh"

namespace Item {

/// @addr{0x8079754C}
KartItem::KartItem() {
    m_flags.makeAllZero();
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
    bool prevButton = m_flags.onBit(eFlags::ItemButtonHold);
    m_flags.resetBit(eFlags::ItemButtonHold, eFlags::ItemButtonActivation);

    const auto &currentInputs = inputs()->currentState();
    if (currentInputs.item()) {
        m_flags.setBit(eFlags::ItemButtonHold).changeBit(!prevButton, eFlags::ItemButtonActivation);
    }

    if (m_flags.onBit(eFlags::Lockout)) {
        if (!state()->isBeforeRespawn() && !state()->isInAction() && !state()->isTriggerRespawn() &&
                !state()->isCannonStart() && !state()->isInCannon() && !state()->isAfterCannon()) {
            m_flags.resetBit(eFlags::Lockout);
        }
    } else {
        if (state()->isInRespawn() || state()->isInAction() || state()->isTriggerRespawn() ||
                state()->isCannonStart() || state()->isInCannon()) {
            m_flags.setBit(eFlags::Lockout);
        } else {
            const auto *raceMgr = System::RaceManager::Instance();
            bool canUse = m_flags.onBit(eFlags::ItemButtonActivation);
            canUse = canUse && raceMgr->isStageReached(System::RaceManager::Stage::Race);
            canUse = canUse && !state()->isInAction();
            canUse = canUse && !state()->isBurnout();
            canUse = canUse && (m_inventory.id() != ItemId::NONE);

            if (canUse) {
                // For now, assume only time trials are valid and use a mushroom
                useMushroom();
            }
        }
    }
}

/// @addr{0x80798848}
void KartItem::clear() {
    if (m_inventory.id() != ItemId::NONE) {
        m_inventory.clear();
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

} // namespace Item
