#pragma once

#include "game/item/ItemId.hh"

namespace Item {

/// @nosubgrouping
class ItemInventory {
public:
    ItemInventory();
    ~ItemInventory();

    /// @beginSetters
    void setItem(ItemId id);
    void useItem(int count);
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] ItemId id() const;
    [[nodiscard]] int currentCount() const;
    /// @endGetters

private:
    ItemId m_currentId;
    int m_currentCount;
};

} // namespace Item
