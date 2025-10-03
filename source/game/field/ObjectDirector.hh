#pragma once

#include "game/field/ObjectCollisionConvexHull.hh"
#include "game/field/ObjectFlowTable.hh"
#include "game/field/ObjectHitTable.hh"
#include "game/field/obj/ObjectCollidable.hh"
#include "game/field/obj/ObjectNoImpl.hh"

#include <vector>

namespace Host {

class Context;

} // namespace Host

namespace Field {

class ObjectDirector : EGG::Disposer {
    friend class Host::Context;

public:
    void init();
    void calc();
    void addObject(ObjectCollidable *obj);
    void addObjectNoImpl(ObjectNoImpl *obj);
    void addManagedObject(ObjectCollidable *obj);

    size_t checkKartObjectCollision(Kart::KartObject *kartObj,
            ObjectCollisionConvexHull *convexHull);

    [[nodiscard]] const ObjectFlowTable &flowTable() const {
        return m_flowTable;
    }

    [[nodiscard]] const ObjectHitTable &hitTableKart() const {
        return m_hitTableKart;
    }

    [[nodiscard]] const ObjectCollidable *collidingObject(size_t idx) const {
        ASSERT(idx < m_collidingObjects.size());
        return m_collidingObjects[idx];
    }

    [[nodiscard]] Kart::Reaction reaction(size_t idx) const {
        ASSERT(idx < m_reactions.size());
        return m_reactions[idx];
    }

    [[nodiscard]] const EGG::Vector3f &hitDepth(size_t idx) const {
        ASSERT(idx < m_hitDepths.size());
        return m_hitDepths[idx];
    }

    [[nodiscard]] std::vector<ObjectCollidable *> &managedObjects() {
        return m_managedObjects;
    }

    [[nodiscard]] const std::vector<ObjectCollidable *> &managedObjects() const {
        return m_managedObjects;
    }

    static ObjectDirector *CreateInstance();
    static void DestroyInstance();

    [[nodiscard]] static ObjectDirector *Instance() {
        return s_instance;
    }

private:
    ObjectDirector();
    ~ObjectDirector() override;

    void createObjects();
    [[nodiscard]] ObjectBase *createObject(const System::MapdataGeoObj &params);

    ObjectFlowTable m_flowTable;
    ObjectHitTable m_hitTableKart;
    ObjectHitTable m_hitTableKartObject;

    std::vector<ObjectBase *> m_objects;          ///< All objects live here
    std::vector<ObjectBase *> m_calcObjects;      ///< Objects needing calc() live here too.
    std::vector<ObjectBase *> m_collisionObjects; ///< Objects having collision live here too

    static constexpr size_t MAX_UNIT_COUNT = 0x100;

    std::array<ObjectCollidable *, MAX_UNIT_COUNT>
            m_collidingObjects; ///< Objects we are currently colliding with
    std::array<EGG::Vector3f, MAX_UNIT_COUNT> m_hitDepths;
    std::array<Kart::Reaction, MAX_UNIT_COUNT> m_reactions;
    std::vector<ObjectCollidable *> m_managedObjects;

    static ObjectDirector *s_instance;
};

} // namespace Field
