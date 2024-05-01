#pragma once

#include "game/item/ItemInventory.hh"

#include "game/kart/KartObjectProxy.hh"

namespace Item {

/// @brief State management for item usage
/// @nosubgrouping
class KartItem : Kart::KartObjectProxy {
public:
    KartItem();
    ~KartItem();

    void init(size_t playerIdx);
    void calc();

    void activateMushroom();
    void useMushroom();

    /// @beginSetters
    void clearButtonFlags();
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] ItemInventory &inventory();
    /// @endGetters

private:
    bool m_bItemButtonHold;
    bool m_bItemButtonActivation;

    ItemInventory m_inventory;
};

} // namespace Item
