#pragma once

#include "game/item/KartItem.hh"

#include <span>

class SavestateManager;

/// @brief Pertains to item handling.
namespace Item {

/// @addr{0x809C3618}
class ItemDirector : EGG::Disposer {
    friend class ::SavestateManager;

public:
    void init();
    void calc();

    [[nodiscard]] KartItem &kartItem(size_t idx) {
        ASSERT(idx < m_karts.size());
        return m_karts[idx];
    }

    static ItemDirector *CreateInstance();
    static void DestroyInstance();

    [[nodiscard]] static ItemDirector *Instance() {
        return s_instance;
    }

private:
    ItemDirector();
    ~ItemDirector() override;

    std::span<KartItem> m_karts;

    static ItemDirector *s_instance; ///< @addr{0x809C3618}
};

} // namespace Item
