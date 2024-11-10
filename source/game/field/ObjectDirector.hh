#pragma once

#include "game/field/ObjectCollisionConvexHull.hh"
#include "game/field/ObjectFlowTable.hh"
#include "game/field/ObjectHitTable.hh"
#include "game/field/obj/ObjectCollidable.hh"

#include <vector>

namespace Field {

class ObjectDirector : EGG::Disposer {
public:
    void init();
    void calc();
    void addObject(ObjectCollidable *obj);

    size_t checkKartObjectCollision(Kart::KartObject *kartObj,
            ObjectCollisionConvexHull *convexHull);

    const ObjectFlowTable &flowTable() const;
    Kart::Reaction reaction(size_t idx) const;
    const EGG::Vector3f &hitDepth(size_t idx) const;

    static ObjectDirector *CreateInstance();
    static void DestroyInstance();
    static ObjectDirector *Instance();

private:
    ObjectDirector();
    ~ObjectDirector() override;

    void createObjects();
    ObjectBase *createObject(const System::MapdataGeoObj &params);

    ObjectFlowTable m_flowTable;
    ObjectHitTable m_hitTableKart;
    ObjectHitTable m_hitTableKartObject;

    std::vector<ObjectBase *> m_objects;          ///< All objects live here
    std::vector<ObjectBase *> m_calcObjects;      ///< Objects needing calc() live here too.
    std::vector<ObjectBase *> m_collisionObjects; ///< Objects having collision live here too

    static constexpr size_t MAX_UNIT_COUNT = 0x100;

    std::array<EGG::Vector3f, MAX_UNIT_COUNT> m_hitDepths;
    std::array<Kart::Reaction, MAX_UNIT_COUNT> m_reactions;

    static ObjectDirector *s_instance;
};

} // namespace Field
