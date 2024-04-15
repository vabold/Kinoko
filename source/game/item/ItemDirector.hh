#pragma once

#include "game/item/KartItem.hh"

#include <span>

namespace Item {

class ItemDirector {
public:
    void init();
    void calc();

    static ItemDirector *CreateInstance();
    static void DestroyInstance();
    static ItemDirector *Instance();

private:
    ItemDirector();
    ~ItemDirector();

    std::span<KartItem> m_karts;

    static ItemDirector *s_instance;
};

} // namespace Item
