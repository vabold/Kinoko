#pragma once
#include <Common.hh>
#include "game/field/obj/GeoObject.hh"

namespace Field {

class ObjDrivableHolder {
public:
    static ObjDrivableHolder* CreateInstance();
    static void DestroyInstance();
    static ObjDrivableHolder* Instance();

    ObjDrivableHolder();
    virtual ~ObjDrivableHolder();
    
    void initObjs();
    void update();
    s32 push(GeoObjectDrivable* pObj);
private:
    static ObjDrivableHolder* s_instance;

    u16 m_count;
    GeoObjectDrivable** m_objs;
};

} // namespace Field
