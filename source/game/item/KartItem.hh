#pragma once

#include "game/item/ItemInventory.hh"

#include "game/kart/KartObjectProxy.hh"

#include <egg/core/BitFlag.hh>

namespace Item {

/// @brief State management for item usage
/// @nosubgrouping
class KartItem : Kart::KartObjectProxy {
public:
    KartItem();
    ~KartItem();

    void init(size_t playerIdx);
    void calc();
    void clear();

    void activateMushroom();
    void useMushroom();

    /// @beginGetters
    [[nodiscard]] ItemInventory &inventory() {
        return m_inventory;
    }
    /// @endGetters

private:
    enum class eFlags {
        Lockout = 10,
        ItemButtonHold = 12,
        ItemButtonActivation = 14,
    };
    typedef EGG::TBitFlag<u16, eFlags> Flags;

    Flags m_flags;
    ItemInventory m_inventory;
};

} // namespace Item
