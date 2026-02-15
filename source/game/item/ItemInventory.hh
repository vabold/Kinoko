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
    void clear();
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] int currentCount() const {
        return m_currentCount;
    }

    [[nodiscard]] ItemId id() const {
        return m_currentId;
    }
    /// @endGetters

private:
    ItemId m_currentId;
    int m_currentCount;
};

} // namespace Item
