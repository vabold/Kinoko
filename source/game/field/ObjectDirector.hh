#pragma once

#include "game/field/ObjectFlowTable.hh"
#include "game/field/obj/ObjectBase.hh"

#include <vector>

namespace Field {

class ObjectDirector {
public:
    const ObjectFlowTable &flowTable() const;

    static ObjectDirector *CreateInstance();
    static void DestroyInstance();
    static ObjectDirector *Instance();

private:
    ObjectDirector();
    ~ObjectDirector();

    void createObjects();
    ObjectBase *createObject(const System::MapdataGeoObj &params);

    std::vector<ObjectBase *> m_objects;
    ObjectFlowTable m_flowTable;

    static ObjectDirector *s_instance;
};

} // namespace Field
