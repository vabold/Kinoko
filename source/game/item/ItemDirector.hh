#pragma once

#include "game/item/KartItem.hh"

#include <span>

/// @brief Pertains to item handling.
namespace Item {

/// @addr{0x809C3618}
class ItemDirector : EGG::Disposer {
public:
    void init();
    void calc();

    static ItemDirector *CreateInstance();
    static void DestroyInstance();
    [[nodiscard]] static ItemDirector *Instance();
    [[nodiscard]] ItemInventory &itemInventory(s16 idx);

private:
    ItemDirector();
    ~ItemDirector() override;

    std::span<KartItem> m_karts;

    static ItemDirector *s_instance; ///< @addr{0x809C3618}
};

} // namespace Item
