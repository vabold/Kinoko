#include "ObjectHeyhoShipManager.hh"

#include "game/field/RailManager.hh"
#include "game/field/obj/ObjectHeyhoBall.hh"
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

    m_projectileAngles = std::span<f32>(new f32[pointCount], pointCount);
    m_objPos = std::span<EGG::Vector3f>(new EGG::Vector3f[pointCount], pointCount);
}

/// @addr{0x806D2514}
ObjectHeyhoShipManager::~ObjectHeyhoShipManager() {
    delete[] m_projectiles.data();
    delete[] m_pointIdxs.data();
    delete[] m_projectileAngles.data();
    delete[] m_objPos.data();
}

/// @addr{0x806D2590}
void ObjectHeyhoShipManager::init() {
    const auto *launcherRailInterp = m_launcher->railInterpolator();

    for (u16 i = 0; i < launcherRailInterp->pointCount(); ++i) {
        m_pointIdxs[i] = -1;
        m_projectileAngles[i] = 0.0f;
        m_objPos[i] = EGG::Vector3f::zero;
    }

    for (size_t i = 0; i < m_projectiles.size(); ++i) {
        m_pointIdxs[m_projectiles[i]->idx()] = i;
    }

    auto *launcherRail = RailManager::Instance()->rail(launcherRailInterp->railIdx());

    for (auto *&obj : m_projectiles) {
        auto *ball = reinterpret_cast<ObjectHeyhoBall *>(obj);
        const s16 idx = obj->idx();

        const EGG::Vector3f &local_4c =
                reinterpret_cast<ObjectHeyhoShip *>(m_launcher)->FUN_806D1CC4(idx);
        EGG::Vector3f vStack_7c = RotateXZByYaw(F_PI / 2.0f, local_4c) * 1200.0f;
        EGG::Vector3f local_64 = vStack_7c + EGG::Vector3f::ey * 1600.0f - local_4c * 700.0f;
        EGG::Vector3f vStack_b8 = launcherRail->pointPos(idx) + local_64;

        obj->initProjectile(vStack_b8);
        m_projectileAngles[idx] =
                1.40625f * EGG::Mathf::Atan2FIdx(ball->initYSpeed(), ball->yDist()) - 40.0f;
        m_objPos[idx] = obj->pos();
    }

    m_launcher->init();
    reinterpret_cast<ObjectHeyhoShip *>(m_launcher)->setAngles(m_projectileAngles);
    reinterpret_cast<ObjectHeyhoShip *>(m_launcher)->setObjPos(m_objPos);
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
