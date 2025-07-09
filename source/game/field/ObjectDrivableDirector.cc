#include "ObjectDrivableDirector.hh"

namespace Field {

/// @addr{0x8081B500}
void ObjectDrivableDirector::init() {
    for (auto *&obj : m_objects) {
        obj->init();
        obj->calcModel();
    }
}

/// @addr{0x8081B618}
void ObjectDrivableDirector::calc() {
    for (auto *&obj : m_calcObjects) {
        obj->calc();
    }

    for (auto *&obj : m_calcObjects) {
        obj->calcModel();
    }
}

/// @addr{0x8081B6C8}
void ObjectDrivableDirector::addObject(ObjectDrivable *obj) {
    if (obj->loadFlags() & 1) {
        m_calcObjects.push_back(obj);
    }

    m_objects.push_back(obj);
}

/// @brief Creates the rGV2 block manager. Also implicitly adds the block represented by params.
void ObjectDrivableDirector::createObakeManager(const System::MapdataGeoObj &params) {
    ASSERT(!m_obakeManager);
    m_obakeManager = new ObjectObakeManager(params);
    m_obakeManager->load();
}

/// @addr{0x8081BC98}
bool ObjectDrivableDirector::checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    if (m_objects.empty()) {
        return false;
    }

    bool hasCollision = false;
    auto *boxColMgr = BoxColManager::Instance();
    boxColMgr->search(radius, pos, eBoxColFlag::Drivable);

    while (ObjectDrivable *obj = boxColMgr->getNextDrivable()) {
        hasCollision |=
                obj->checkSpherePartial(radius, pos, prevPos, mask, info, maskOut, timeOffset);
    }

    return hasCollision;
}

/// @addr{0x8081BD70}
bool ObjectDrivableDirector::checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    if (m_objects.empty()) {
        return false;
    }

    bool hasCollision = false;
    auto *boxColMgr = BoxColManager::Instance();
    boxColMgr->search(radius, pos, eBoxColFlag::Drivable);

    while (ObjectDrivable *obj = boxColMgr->getNextDrivable()) {
        hasCollision |=
                obj->checkSpherePartialPush(radius, pos, prevPos, mask, info, maskOut, timeOffset);
    }

    return hasCollision;
}

/// @addr{0x8081BE48}
bool ObjectDrivableDirector::checkSphereFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    if (m_objects.empty()) {
        return false;
    }

    bool hasCollision = false;
    auto *boxColMgr = BoxColManager::Instance();
    boxColMgr->search(radius, pos, eBoxColFlag::Drivable);

    while (ObjectDrivable *obj = boxColMgr->getNextDrivable()) {
        hasCollision |= obj->checkSphereFull(radius, pos, prevPos, mask, info, maskOut, timeOffset);
    }

    return hasCollision;
}

/// @addr{0x8081BFA0}
bool ObjectDrivableDirector::checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    if (m_objects.empty()) {
        return false;
    }

    bool hasCollision = false;
    auto *boxColMgr = BoxColManager::Instance();
    boxColMgr->search(radius, pos, eBoxColFlag::Drivable);

    while (ObjectDrivable *obj = boxColMgr->getNextDrivable()) {
        hasCollision |=
                obj->checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut, timeOffset);
    }

    return hasCollision;
}

/// @addr{0x8081C5A0}
bool ObjectDrivableDirector::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    if (m_objects.empty()) {
        return false;
    }

    auto *boxColMgr = BoxColManager::Instance();

    if (boxColMgr->isSphereInSpatialCache(radius, pos, eBoxColFlag::Drivable)) {
        boxColMgr->resetIterators();

        bool hasCollision = false;
        while (ObjectDrivable *obj = boxColMgr->getNextDrivable()) {
            hasCollision |= obj->checkSphereCachedPartial(radius, pos, prevPos, mask, info, maskOut,
                    timeOffset);
        }

        return hasCollision;
    }

    return checkSpherePartial(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x8081C6B4}
bool ObjectDrivableDirector::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    if (m_objects.empty()) {
        return false;
    }

    auto *boxColMgr = BoxColManager::Instance();

    if (boxColMgr->isSphereInSpatialCache(radius, pos, eBoxColFlag::Drivable)) {
        boxColMgr->resetIterators();

        bool hasCollision = false;
        while (ObjectDrivable *obj = boxColMgr->getNextDrivable()) {
            hasCollision |= obj->checkSphereCachedPartialPush(radius, pos, prevPos, mask, info,
                    maskOut, timeOffset);
        }

        return hasCollision;
    }

    return checkSpherePartialPush(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x8081C958}
bool ObjectDrivableDirector::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    if (m_objects.empty()) {
        return false;
    }

    auto *boxColMgr = BoxColManager::Instance();

    if (boxColMgr->isSphereInSpatialCache(radius, pos, eBoxColFlag::Drivable)) {
        bool hasCollision = false;
        boxColMgr->resetIterators();

        while (ObjectDrivable *obj = boxColMgr->getNextDrivable()) {
            hasCollision |= obj->checkSphereCachedFullPush(radius, pos, prevPos, mask, info,
                    maskOut, timeOffset);
        }

        return hasCollision;
    }

    return checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x8081B7CC}
void ObjectDrivableDirector::colNarScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
        u32 timeOffset) {
    if (m_objects.empty()) {
        return;
    }

    auto *boxColMgr = BoxColManager::Instance();
    boxColMgr->search(radius, pos, eBoxColFlag::Drivable);

    while (ObjectDrivable *obj = boxColMgr->getNextDrivable()) {
        obj->narrScLocal(radius, pos, mask, timeOffset);
    }
}

/// @addr{0x8081B428}
ObjectDrivableDirector *ObjectDrivableDirector::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new ObjectDrivableDirector;
    return s_instance;
}

/// @addr{0x8081B4B0}
void ObjectDrivableDirector::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

/// @addr{0x8081B324}
ObjectDrivableDirector::ObjectDrivableDirector() : m_obakeManager(nullptr) {}

/// @addr{0x8081B380}
ObjectDrivableDirector::~ObjectDrivableDirector() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("ObjectDrivableDirector instance not explicitly handled!");
    }

    for (auto *&obj : m_objects) {
        delete obj;
    }
}

ObjectDrivableDirector *ObjectDrivableDirector::s_instance = nullptr; ///< @addr{0x809C4310}

} // namespace Field
