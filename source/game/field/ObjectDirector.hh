#pragma once

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

    const ObjectFlowTable &flowTable() const;

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

    static ObjectDirector *s_instance;
};

} // namespace Field
