#pragma once

#include "game/field/ObjectDirector.hh"
#include "game/field/RailInterpolator.hh"
#include "game/field/obj/ObjectSniper.hh"

#include <cstring>

namespace Kinoko::Field {

/// @brief Handles the synchronization between an @ref ObjectSunDS and its @ref ObjectFireSnake
/// projectiles
/// @warning It is expected that all @ref ObjectFireSnake projectiles and the @ref ObjectSunDS
/// object are already constructed and registered to the @ref ObjectDirector array of managed
/// objects. Otherwise, they will not be visible to this manager.
class ObjectSunManager final : public ObjectSniper {
public:
    /// @addr{0x806DE624}
    /// @brief Constructor that caches pointers to all existing @ref ObjectFireSnake projectiles
    /// and the @ref ObjectSunDS launcher.
    /// @details Iterates the @ref ObjectDirector array of managed objects to find the count of
    /// @ref ObjectFireSnake objects in order to properly size @ref m_projectiles. Then populates
    /// @ref m_projectiles with pointers to each of the @ref ObjectFireSnake objects and also caches
    /// a pointer to the @ref ObjectSunDS and saves it to @ref m_launcher. Finally, creates @ref
    /// m_pointIdxs based on the launcher's rail point count.
    ObjectSunManager() {
        const auto &managedObjs = ObjectDirector::Instance()->managedObjects();

        size_t count = 0;

        for (auto *const &obj : managedObjs) {
            if (strcmp(obj->getName(), "FireSnake") == 0) {
                ++count;
            }
        }

        m_projectiles = owning_span<ObjectProjectile *>(count);

        size_t curIdx = 0;

        for (auto *const &obj : managedObjs) {
            if (strcmp(obj->getName(), "FireSnake") == 0) {
                m_projectiles[curIdx++] = reinterpret_cast<ObjectProjectile *>(obj);
            } else if (strcmp(obj->getName(), "sunDS") == 0) {
                m_launcher = reinterpret_cast<ObjectProjectileLauncher *>(obj);
            }
        }

        u16 pointCount = m_launcher->railInterpolator()->pointCount();
        m_pointIdxs = owning_span<s16>(pointCount);
    }

    /// @addr{0x806DE780}
    /// @brief Default virtual destructor
    ~ObjectSunManager() override = default;
};

} // namespace Kinoko::Field
