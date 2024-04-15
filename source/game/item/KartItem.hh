#pragma once

#include "game/item/ItemInventory.hh"

#include "game/kart/KartObjectProxy.hh"

namespace Item {

class KartItem : Kart::KartObjectProxy {
public:
    KartItem();
    ~KartItem();

    void init(size_t playerIdx);
    void calc();

    void activateMushroom();
    void useMushroom();

    ItemInventory &inventory();

    void clearButtonFlags();

private:
    bool m_bItemButtonHold;
    bool m_bItemButtonActivation;

    ItemInventory m_inventory;
};

} // namespace Item
