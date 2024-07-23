#include "ObjectDirector.hh"

#include "game/field/obj/ObjectRegistry.hh"

#include "game/system/CourseMap.hh"

namespace Field {

/// @addr{0x8082A784}
ObjectDirector *ObjectDirector::CreateInstance() {
    assert(!s_instance);
    s_instance = new ObjectDirector;

    s_instance->createObjects();

    return s_instance;
}

/// @addr{0x8082A824}
void ObjectDirector::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

ObjectDirector *ObjectDirector::Instance() {
    return s_instance;
}

/// @addr{0x8082A38C}
ObjectDirector::ObjectDirector() = default;

/// @addr{0x8082A694}
ObjectDirector::~ObjectDirector() {
    for (auto *&obj : m_objects) {
        delete obj;
    }
}

/// @addr{0x80826E8C}
void ObjectDirector::createObjects() {
    const auto *courseMap = System::CourseMap::Instance();
    size_t objectCount = courseMap->getGeoObjCount();
    m_objects.reserve(objectCount);

    for (size_t i = 0; i < objectCount; ++i) {
        const auto *pObj = courseMap->getGeoObj(i);
        assert(pObj);

        // Assume one player - if the presence flag isn't set, don't construct it
        if (!(pObj->presenceFlag() & 1)) {
            continue;
        }

        // Prevent construction of objects with disabled or no collision
        if (IsObjectBlacklisted(pObj->id())) {
            continue;
        }

        m_objects.push_back(createObject(*pObj));
    }
}

/// @addr{0x80821E14}
ObjectBase *ObjectDirector::createObject(const System::MapdataGeoObj &params) {
    ObjectId id = static_cast<ObjectId>(params.id());
    switch (id) {
    case ObjectId::DokanSFC:
        return new ObjectDokan(params);
    case ObjectId::OilSFC:
        return new ObjectOilSFC(params);
    default:
        return new ObjectNoImpl(params);
    }
}

ObjectDirector *ObjectDirector::s_instance = nullptr; ///< @addr{0x809C4330}

} // namespace Field
