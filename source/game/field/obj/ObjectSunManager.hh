#pragma once

#include "game/field/ObjectDirector.hh"
#include "game/field/obj/ObjectSniper.hh"

#include <string>

namespace Field {

/// @brief Handles the synchronization between the @ref ObjectSunDS and @ref ObjectFireSnake
/// projectiles.
class ObjectSunManager final : public ObjectSniper {
public:
    /// @addr{0x806DE624}
    ObjectSunManager() {
        auto &managedObjs = ObjectDirector::Instance()->managedObjects();

        size_t count = 0;

        for (auto *&obj : managedObjs) {
            if (strcmp(obj->getName(), "FireSnake") == 0) {
                ++count;
            }
        }

        m_projectiles = std::span<ObjectProjectile *>(new ObjectProjectile *[count], count);

        size_t curIdx = 0;

        for (auto *&obj : managedObjs) {
            if (strcmp(obj->getName(), "FireSnake") == 0) {
                m_projectiles[curIdx++] = reinterpret_cast<ObjectProjectile *>(obj);
            } else if (strcmp(obj->getName(), "sunDS") == 0) {
                m_launcher = reinterpret_cast<ObjectProjectileLauncher *>(obj);
            }
        }

        u16 pointCount = m_launcher->railInterpolator()->pointCount();
        m_pointIdxs = std::span<s16>(new s16[pointCount], pointCount);
    }

    /// @addr{0x806DE780}
    ~ObjectSunManager() override {
        delete[] m_projectiles.data();
        delete[] m_pointIdxs.data();
    }
};

} // namespace Field
