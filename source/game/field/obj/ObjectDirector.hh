#pragma once
#include <Common.hh>

#include "game/field/obj/GeoHitTable.hh"
#include "game/field/obj/GeoObject.hh"
#include "game/field/obj/ObjFlow.hh"

namespace Field {

struct ObjGroup {
    u8 _00[0xf0];
    u32 m_count;
};

class ObjectDirector {
public:
    ObjectDirector();
    virtual ~ObjectDirector();

    void createObjects(bool isMii);
    void constructObject(System::MapdataGeoObj &obj);

    /// @beginGetters
    const ObjFlow *objFlow() const;
    /// @endGetters

    static ObjectDirector *CreateInstance();
    static void DestroyInstance();
    static ObjectDirector *Instance();

private:
    ObjFlow *m_objFlow;
    GeoHitTable *m_getHitTableItem;
    GeoHitTable *m_getHitTableItemObj;
    GeoHitTable *m_getHitTableKart;
    GeoHitTable *m_getHitTableKartObj;
    GeoObjectArray m_objArrays[5];
    GeoObject **m_objs;
    EGG::Vector3f *_44;
    u8 *m_collisionScenarios; // unknown
    ObjGroup *m_managedGroup;
    bool m_hasSunDSOrWiggler = false;
    EGG::Matrix34f _64[4];
    float _124[4];

    static ObjectDirector *s_instance;
};

} // namespace Field
