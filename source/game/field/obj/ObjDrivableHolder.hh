#pragma once

#include "game/field/obj/GeoObject.hh"

namespace Field {

class ObjDrivableHolder {
public:
    ObjDrivableHolder();
    virtual ~ObjDrivableHolder();

    void initObjs();
    void update();
    s32 push(GeoObjectDrivable *pObj);

    static ObjDrivableHolder *CreateInstance();
    static void DestroyInstance();
    static ObjDrivableHolder *Instance();

private:
    u16 m_count;
    GeoObjectDrivable **m_objs;

    static ObjDrivableHolder *s_instance;
};

} // namespace Field
