#pragma once

#include "game/field/Object.hh"

#include <vector>

namespace Field {

class ObjectDirector {
public:
    static ObjectDirector *CreateInstance();
    static void DestroyInstance();
    static ObjectDirector *Instance();

private:
    ObjectDirector();
    ~ObjectDirector();

    void createObjects();
    Object *createObject(const System::MapdataGeoObj &params);

    std::vector<Object *> m_objects;

    static ObjectDirector *s_instance;
};

} // namespace Field
