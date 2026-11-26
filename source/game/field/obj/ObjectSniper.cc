#include "ObjectSniper.hh"

#include "game/field/Rail.hh"
#include "game/field/RailInterpolator.hh"
#include "game/field/obj/ObjectProjectile.hh"
#include "game/field/obj/ObjectProjectileLauncher.hh"

namespace Kinoko::Field {

/// @addr{0x806DDA84}
/// @brief Constructor that creates the object named `MapObjSniper`
ObjectSniper::ObjectSniper()
    : ObjectCollidable("MapObjSniper", EGG::Vector3f::zero, EGG::Vector3f::ez,
              EGG::Vector3f::unit) {}

/// @addr{0x806DDAF4}
/// @brief Default virtual destructor
ObjectSniper::~ObjectSniper() = default;

/// @addr{0x806DDB34}
/// @copybrief ObjectBase::init()
/// @details Constructs the @ref m_pointIdxs mapping from each launcher rail point index to the
/// index of the projectile to launch there. Initializes each projectile object at its corresponding
/// rail point's position.
void ObjectSniper::init() {
    const auto *launcherRailInterp = m_launcher->railInterpolator();

    for (auto &point : m_pointIdxs) {
        point = -1;
    }

    const auto *launcherRail = launcherRailInterp->rail();

    for (size_t i = 0; i < m_projectiles.size(); ++i) {
        auto *obj = m_projectiles[i];
        m_pointIdxs[obj->idx()] = i;
        obj->initProjectile(launcherRail->pointPos(obj->idx()));
    }
}

/// @addr{0x806DDC44}
/// @copybrief ObjectBase::calc()
/// @details Checks if the launcher should launch a projectile and triggers the
/// corresponding projectile's launch.
void ObjectSniper::calc() {
    s32 idx = m_launcher->launchPointIdx();

    if (idx != -1) {
        m_projectiles[m_pointIdxs[idx]]->onLaunch();
    }
}

/// @addr{0x806D28FC}
/// @copybrief ObjectBase::loadGraphics()
/// @details No-op because the manager object does not have any graphics to load.
void ObjectSniper::loadGraphics() {}

/// @addr{0x806D28F4}
/// @copybrief ObjectBase::createCollision()
/// @details No-op because the manager object does not have any collision to load. The launcher
/// object independently loads its own collision.
void ObjectSniper::createCollision() {}

/// @addr{0x806D28F8}
/// @copybrief ObjectBase::loadRail()
/// @details No-op because the manager object does not have any rail to load. The launcher
/// object independently manages its own position.
void ObjectSniper::loadRail() {}

/// @addr{0x806D2900}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame
ObjectBase::LoadFlags ObjectSniper::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

} // namespace Kinoko::Field
