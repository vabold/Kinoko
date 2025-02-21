#pragma once

#include "game/kart/KartObject.hh"

namespace Kart {

/// @brief Responsible for the lifecycle and calculation of KartObjects.
/// @addr{0x809C18F8}
/// @nosubgrouping
class KartObjectManager : EGG::Disposer {
public:
    void init();
    void calc();

    /// @beginGetters
    /// @addr{0x80590100}
    [[nodiscard]] KartObject *object(size_t i) const {
        ASSERT(i < m_count);
        return m_objects[i];
    }
    /// @endGetters

    static KartObjectManager *CreateInstance();
    static void DestroyInstance();

    [[nodiscard]] static KartObjectManager *Instance() {
        return s_instance;
    }

private:
    KartObjectManager();
    ~KartObjectManager() override;

    size_t m_count;
    KartObject **m_objects;

    static KartObjectManager *s_instance; ///< @addr{0x809C18F8}
};

} // namespace Kart
