#include "ObjectHeyhoShipManager.hh"

#include "game/field/ObjectDirector.hh"
#include "game/field/RailManager.hh"
#include "game/field/obj/ObjectHeyhoShip.hh"

namespace Field {

/// @addr{0x806D2368}
ObjectHeyhoShipManager::ObjectHeyhoShipManager() {
    auto &managedObjs = ObjectDirector::Instance()->managedObjects();
    size_t count = 0;
    for (auto *&obj : managedObjs) {
        if (strcmp(obj->getName(), "HeyhoBallGBA") == 0) {
            ++count;
        }
    }

    m_projectiles = std::span<ObjectProjectile *>(new ObjectProjectile *[count], count);
    size_t curIdx = 0;
    for (auto *&obj : managedObjs) {
        if (strcmp(obj->getName(), "HeyhoBallGBA") == 0) {
            m_projectiles[curIdx++] = reinterpret_cast<ObjectProjectile *>(obj);
        } else if (strcmp(obj->getName(), "HeyhoShipGBA") == 0) {
            m_launcher = reinterpret_cast<ObjectProjectileLauncher *>(obj);
        }
    }

    u16 pointCount = m_launcher->railInterpolator()->pointCount();
    m_pointIdxs = std::span<s16>(new s16[pointCount], pointCount);
}

/// @addr{0x806D2514}
ObjectHeyhoShipManager::~ObjectHeyhoShipManager() {
    delete[] m_projectiles.data();
    delete[] m_pointIdxs.data();
}

/// @addr{0x806D2590}
void ObjectHeyhoShipManager::init() {
    /// Projectiles are fired perpendicular to the ship direction
    constexpr f32 PROJECTILE_ANGLE = F_PI / 2.0f;
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
        EGG::Vector3f forward = RotateXZByYaw(PROJECTILE_ANGLE, dir) * FORWARD_OFFSET;
        EGG::Vector3f posOffset = forward + HEIGHT_OFFSET - dir * LATERAL_OFFSET;
        EGG::Vector3f shipPos = launcherRail->pointPos(idx) + posOffset;

        obj->initProjectile(shipPos);
    }

    m_launcher->init();
}

/// @addr{0x806D2868}
void ObjectHeyhoShipManager::calc() {
    s32 idx = m_launcher->launchPointIdx();

    if (idx != -1) {
        s16 pointIdx = m_pointIdxs[idx];

        if (pointIdx != -1) {
            m_projectiles[pointIdx]->onLaunch();
        }
    }
}

} // namespace Field
