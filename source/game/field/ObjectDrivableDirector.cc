#include "game/field/ObjectDrivableDirector.hh"

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
    u32 loadFlags = obj->loadFlags();

    if (loadFlags & 1) {
        m_calcObjects.push_back(obj);
    }

    m_objects.push_back(obj);
}

/// @addr{0x8081B7CC}
void ObjectDrivableDirector::colNarScLocal(f32 scale, f32 radius, const EGG::Vector3f &pos,
        KCLTypeMask mask, bool bScaledUp) {
    if (m_objects.empty()) {
        return;
    }

    BoxColManager::Instance()->search(radius, pos, eBoxColFlag::Drivable);

    while (ObjectDrivable *obj = BoxColManager::Instance()->getNextDrivable()) {
        obj->narrScLocal(scale, pos, mask, bScaledUp);
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

ObjectDrivableDirector *ObjectDrivableDirector::Instance() {
    return s_instance;
}

/// @addr{0x8082A38C}
ObjectDrivableDirector::ObjectDrivableDirector() {}

/// @addr{0x8082A694}
ObjectDrivableDirector::~ObjectDrivableDirector() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("ObjectDrivableDirector instance not explicitly handled!");
    }
}

ObjectDrivableDirector *ObjectDrivableDirector::s_instance = nullptr; ///< @addr{0x809C4310}

} // namespace Field
