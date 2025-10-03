#include "ObjectSniper.hh"

#include "game/field/RailManager.hh"

namespace Field {

/// @addr{0x806DDA84}
ObjectSniper::ObjectSniper()
    : ObjectCollidable("MapObjSniper", EGG::Vector3f::zero, EGG::Vector3f::ez,
              EGG::Vector3f::unit) {}

/// @addr{0x806DDAF4}
ObjectSniper::~ObjectSniper() = default;

/// @addr{0x806DDB34}
void ObjectSniper::init() {
    const auto *launcherRailInterp = m_launcher->railInterpolator();

    for (auto &point : m_pointIdxs) {
        point = -1;
    }

    auto *launcherRail = RailManager::Instance()->rail(launcherRailInterp->railIdx());

    for (size_t i = 0; i < m_projectiles.size(); ++i) {
        auto *obj = m_projectiles[i];
        m_pointIdxs[obj->idx()] = i;
        obj->initProjectile(launcherRail->pointPos(obj->idx()));
    }
}

/// @addr{0x806DDC44}
void ObjectSniper::calc() {
    s32 idx = m_launcher->launchPointIdx();

    if (idx != -1) {
        m_projectiles[m_pointIdxs[idx]]->onLaunch();
    }
}

} // namespace Field
