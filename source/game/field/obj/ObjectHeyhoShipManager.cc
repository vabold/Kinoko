#include "ObjectHeyhoShipManager.hh"

#include "game/field/ObjectDirector.hh"
#include "game/field/RailInterpolator.hh"
#include "game/field/RailManager.hh"
#include "game/field/obj/ObjectHeyhoShip.hh"
#include "game/field/obj/ObjectProjectile.hh"

#include <cstring>

namespace Kinoko::Field {

/// @addr{0x806D2368}
/// @copydoc ObjectSniper::ObjectSniper()
/// @pre All @ref ObjectHeyhoBall objects must be constructed and registered to the vector of
/// managed objects in @ref ObjectDirector.
/// @details Computes the number of managed @ref ObjectHeyhoBall cannonballs and sizes @ref
/// m_projectiles accordingly. For each registered @ref ObjectHeyhoBall, caches it to @ref
/// m_projectiles with a `reinterpret_cast` to @ref ObjectProjectile and caches the @ref
/// ObjectHeyhoShip to @ref m_launcher. Finally, creates an @ref owning_span for all point indices
/// to track which rail points are associated with which cannonballs.
/// @note If for any reason there are two registered @ref ObjectHeyhoShip objects, only the last one
/// encountered will be cached to @ref m_launcher.
ObjectHeyhoShipManager::ObjectHeyhoShipManager() {
    auto &managedObjs = ObjectDirector::Instance()->managedObjects();
    size_t count = 0;
    for (auto *&obj : managedObjs) {
        if (strcmp(obj->getName(), "HeyhoBallGBA") == 0) {
            ++count;
        }
    }

    m_projectiles = owning_span<ObjectProjectile *>(count);
    size_t curIdx = 0;
    for (auto *&obj : managedObjs) {
        if (strcmp(obj->getName(), "HeyhoBallGBA") == 0) {
            m_projectiles[curIdx++] = reinterpret_cast<ObjectProjectile *>(obj);
        } else if (strcmp(obj->getName(), "HeyhoShipGBA") == 0) {
            m_launcher = reinterpret_cast<ObjectProjectileLauncher *>(obj);
        }
    }

    u16 pointCount = m_launcher->railInterpolator()->pointCount();
    m_pointIdxs = owning_span<s16>(pointCount);
}

/// @addr{0x806D2514}
/// @brief Default virtual destructor
ObjectHeyhoShipManager::~ObjectHeyhoShipManager() = default;

/// @addr{0x806D2590}
/// @copybrief ObjectBase::init()
/// @details Initializes every entry in @ref m_pointIdxs to -1. For each cannonball, stores the
/// cannonball's index from @ref m_projectiles to the corresponding rail point index in @ref
/// m_pointIdxs. For each cannonball, calls @ref ObjectHeyhoBall::initProjectile() with the ship's
/// position so that the cannonballs can compute their appropriate launch direction and speeds.
/// Finally, initializes the @ref ObjectHeyhoShip object.
void ObjectHeyhoShipManager::init() {
    constexpr EGG::Vector3f HEIGHT_OFFSET = EGG::Vector3f::ey * 1600.0f;
    constexpr f32 FORWARD_OFFSET = 1200.0f;
    constexpr f32 LATERAL_OFFSET = 700.0f;

    const auto *launcherRailInterp = m_launcher->railInterpolator();
    for (u16 i = 0; i < launcherRailInterp->pointCount(); ++i) {
        m_pointIdxs[i] = -1;
    }

    for (size_t i = 0; i < m_projectiles.size(); ++i) {
        m_pointIdxs[m_projectiles[i]->idx()] = i;
    }

    auto *ship = reinterpret_cast<ObjectHeyhoShip *>(m_launcher);
    auto *launcherRail = RailManager::Instance()->rail(launcherRailInterp->railIdx());
    for (auto *&obj : m_projectiles) {
        const s16 idx = obj->idx();
        const auto &dir = ship->initRailDir(idx);

        // Projectiles are fired perpendicular to the ship direction
        EGG::Vector3f forward = RotateXZByYaw(HALF_PI, dir) * FORWARD_OFFSET;
        EGG::Vector3f posOffset = forward + HEIGHT_OFFSET - dir * LATERAL_OFFSET;
        EGG::Vector3f shipPos = launcherRail->pointPos(idx) + posOffset;

        obj->initProjectile(shipPos);
    }

    m_launcher->init();
}

/// @addr{0x806D2868}
/// @copybrief ObjectBase::calc()
/// @details Checks to see if there is a cannonball projectile corresponding with the ship's current
/// rail point and calls the @ref ObjectHeyhoBall::onLaunch() callback method if so.
void ObjectHeyhoShipManager::calc() {
    s32 idx = m_launcher->launchPointIdx();

    if (idx != -1) {
        s16 pointIdx = m_pointIdxs[idx];

        if (pointIdx != -1) {
            m_projectiles[pointIdx]->onLaunch();
        }
    }
}

} // namespace Kinoko::Field
