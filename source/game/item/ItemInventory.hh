#pragma once

#include "game/item/ItemId.hh"

namespace Item {

class ItemInventory {
public:
    ItemInventory();
    ~ItemInventory();

    void setItem(ItemId id);
    void useItem(int count);

    ItemId id() const;

private:
    ItemId m_currentId;
    int m_currentCount;
};

} // namespace Item
