#include "ObjectDirector.hh"

#include "game/field/BoxColManager.hh"
#include "game/field/obj/ObjectRegistry.hh"

#include "game/kart/KartObject.hh"

#include "game/system/CourseMap.hh"

namespace Field {

/// @addr{0x8082A2B4}
void ObjectDirector::init() {
    for (auto *&obj : m_objects) {
        obj->init();
        obj->calcModel();
    }
}

/// @addr{0x8082A8F4}
void ObjectDirector::calc() {
    for (auto *&obj : m_calcObjects) {
        obj->calc();
    }

    for (auto *&obj : m_calcObjects) {
        obj->calcModel();
    }
}

/// @addr{0x8082B0E8}
void ObjectDirector::addObject(ObjectCollidable *obj) {
    u32 loadFlags = obj->loadFlags();

    if (loadFlags & 1) {
        m_calcObjects.push_back(obj);
    }

    if (m_flowTable.set(m_flowTable.slot(obj->id()))->mode != 0) {
        if (obj->collision()) {
            m_collisionObjects.push_back(obj);
        }
    }

    m_objects.push_back(obj);
}

/// @addr{0x8082AB04}
size_t ObjectDirector::checkKartObjectCollision(Kart::KartObject *kartObj,
        ObjectCollisionConvexHull *convexHull) {
    size_t count = 0;

    while (ObjectCollidable *obj = BoxColManager::Instance()->getNextObject()) {
        auto *objCollision = obj->collision();
        if (!objCollision) {
            continue;
        }

        obj->calcCollisionTransform();
        if (!obj->checkCollision(convexHull, m_hitDepths[count])) {
            continue;
        }

        // We have a collision, process it
        // Assume that we are not in a star, mega, or bullet
        Kart::Reaction reactionOnKart = m_hitTableKart.reaction(m_hitTableKart.slot(obj->id()));
        Kart::Reaction reactionOnObj =
                m_hitTableKartObject.reaction(m_hitTableKartObject.slot(obj->id()));

        // The object might change the reaction states
        obj->processKartReactions(kartObj, reactionOnKart, reactionOnObj);

        Kart::Reaction reaction =
                obj->onCollision(kartObj, reactionOnKart, reactionOnObj, m_hitDepths[count]);
        m_reactions[count] = reaction;

        if (reaction == Kart::Reaction::WallAllSpeed || reaction == Kart::Reaction::WallSpark) {
            obj->onWallCollision(kartObj, m_hitDepths[count]);
        } else {
            obj->onObjectCollision(kartObj);
        }

        m_collidingObjects[count] = obj;
        if (m_hitDepths[count].y < 0.0f) {
            m_hitDepths[count].y = 0.0f;
        }

        ++count;
    }

    return count;
}

const ObjectFlowTable &ObjectDirector::flowTable() const {
    return m_flowTable;
}

const ObjectBase *ObjectDirector::collidingObject(size_t idx) const {
    ASSERT(idx < m_collidingObjects.size());

    return m_collidingObjects[idx];
}

Kart::Reaction ObjectDirector::reaction(size_t idx) const {
    ASSERT(idx < m_reactions.size());

    return m_reactions[idx];
}

const EGG::Vector3f &ObjectDirector::hitDepth(size_t idx) const {
    ASSERT(idx < m_hitDepths.size());

    return m_hitDepths[idx];
}

/// @addr{0x8082A784}
ObjectDirector *ObjectDirector::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new ObjectDirector;

    s_instance->createObjects();

    return s_instance;
}

/// @addr{0x8082A824}
void ObjectDirector::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

ObjectDirector *ObjectDirector::Instance() {
    return s_instance;
}

/// @addr{0x8082A38C}
ObjectDirector::ObjectDirector()
    : m_flowTable("ObjFlow.bin"), m_hitTableKart("GeoHitTableKart.bin"),
      m_hitTableKartObject("GeoHitTableKartObj.bin") {}

/// @addr{0x8082A694}
ObjectDirector::~ObjectDirector() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("ObjectDirector instance not explicitly handled!");
    }

    for (auto *&obj : m_objects) {
        delete obj;
    }
}

/// @addr{0x80826E8C}
void ObjectDirector::createObjects() {
    const auto *courseMap = System::CourseMap::Instance();
    size_t objectCount = courseMap->getGeoObjCount();

    // It's possible for the KMP to specify settings for objects that aren't tracked here
    // MAX_UNIT_COUNT is the upper bound for tracked object count, so we reserve the minimum
    size_t maxCount = std::min(objectCount, MAX_UNIT_COUNT);
    m_objects.reserve(maxCount);
    m_calcObjects.reserve(maxCount);
    m_collisionObjects.reserve(maxCount);

    for (size_t i = 0; i < objectCount; ++i) {
        const auto *pObj = courseMap->getGeoObj(i);
        ASSERT(pObj);

        // Assume one player - if the presence flag isn't set, don't construct it
        if (!(pObj->presenceFlag() & 1)) {
            continue;
        }

        // Prevent construction of objects with disabled or no collision
        if (IsObjectBlacklisted(pObj->id())) {
            continue;
        }

        ObjectBase *object = createObject(*pObj);
        object->load();
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
    // Non-specified objects are stock collidable objects by default
    // However, we need to specify an impl, so we don't use default
    case ObjectId::DummyPole:
        return new ObjectCollidable(params);
    default:
        return new ObjectNoImpl(params);
    }
}

ObjectDirector *ObjectDirector::s_instance = nullptr; ///< @addr{0x809C4330}

} // namespace Field
