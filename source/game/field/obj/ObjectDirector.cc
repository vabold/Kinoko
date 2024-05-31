#include "game/field/obj/ObjectDirector.hh"
#include "game/field/obj/ObjDrivableHolder.hh"
#include "game/field/obj/objects/Mdush.hh"
#include "game/system/CourseMap.hh"
#include "game/system/RaceConfig.hh"
#include <cstring>

namespace Field {

/// @addr{0x8082A38C}
ObjectDirector::ObjectDirector() {
    static constexpr size_t OBJS_CAPACITY = 800;
    static constexpr size_t VECS_CAPACITY = 200;
    // more stuff here
    m_objFlow = new ObjFlow("ObjFlow.bin");
    m_getHitTableItem = new GeoHitTable("GeoHitTableItem.bin");
    m_getHitTableItemObj = new GeoHitTable("GeoHitTableItemObj.bin");
    m_getHitTableKart = new GeoHitTable("GeoHitTableKart.bin");
    m_getHitTableKartObj = new GeoHitTable("GeoHitTableKartObj.bin");

    auto raceScenario = System::RaceConfig::Instance()->raceScenario();

    Course course = raceScenario.course;
    if (course != Course::Moonview_Highway && course != Course::Galaxy_Colosseum &&
            course != (Course)0x33 && course != Course::DS_Desert_Hills &&
            course != Course::GBA_Shy_Guy_Beach) {
        m_managedGroup = nullptr;
    } else {
        m_managedGroup = new ObjGroup;
        m_managedGroup->m_count = 0;
    }

    for (s32 i = 0; i < 5; i++) {
        m_objArrays[i].m_count = 0;
        m_objArrays[i].m_array = new GeoObject *[OBJS_CAPACITY];
    }

    m_objs = new GeoObject *[OBJS_CAPACITY];
    _44 = new EGG::Vector3f[VECS_CAPACITY];
    m_collisionScenarios = new u8[OBJS_CAPACITY];

    for (s32 i = 0; i < 4; i++) {
        _124[i] = 0.0f;
    }
}

ObjectDirector::~ObjectDirector() {
    // destroying stuff the game doesn't normally destroy because i'm a decent human being
    for (s32 i = 0; i < 5; i++) {
        for (s32 j = 0; j < m_objArrays[i].m_count; j++) {
            delete m_objArrays[i].m_array[j];
        }
        delete[] m_objArrays[i].m_array;
    }

    delete m_managedGroup;

    delete[] m_collisionScenarios;
    delete[] _44;
    delete[] m_objs;

    delete m_getHitTableKartObj;
    delete m_getHitTableKart;
    delete m_getHitTableItemObj;
    delete m_getHitTableItem;
    delete m_objFlow;

    // more stuff here
}

ObjectDirector *ObjectDirector::CreateInstance() {
    if (!s_instance) {
        s_instance = new ObjectDirector;
    }
    ObjDrivableHolder::CreateInstance();
    s_instance->createObjects(true);
    // destroy some heap, and then
    s_instance->createObjects(false);
    // FUN_8088be00()
    return s_instance;
}

void ObjectDirector::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    ObjDrivableHolder::DestroyInstance();
    // more stuff here
    s_instance = nullptr;
}

ObjectDirector *ObjectDirector::Instance() {
    return s_instance;
}

void ObjectDirector::createObjects(bool isMii) {
    // isMii will just create all mii-related objects, for now we don't care about those so let's
    // not for now
    if (isMii) {
        return;
    }

    m_hasSunDSOrWiggler = false;

    u32 objCount;
    if (System::CourseMap::Instance()->geoObj() == nullptr) {
        objCount = 0;
    } else {
        objCount = System::CourseMap::Instance()->geoObj()->size();
    }

    for (u32 i = 0; i < objCount; i++) {
        System::MapdataGeoObj *obj = System::CourseMap::Instance()->getGeoObj(i);
        // a bunch of objects have sorta custom handlers, we don't care rn though
        constructObject(*obj);
    }
}

void ObjectDirector::constructObject(System::MapdataGeoObj &obj) {
    GeoObject *out = nullptr;

    u16 slot = m_objFlow->slots(obj.id());
    ObjAttrs *attrs = m_objFlow->attrs(slot);
    const char *name = attrs->name();

    if (strcmp(name, "Mdush") == 0) {
        out = new Mdush(&obj);
        out->init();
    }
}

const ObjFlow *ObjectDirector::objFlow() const {
    return m_objFlow;
}

ObjectDirector *ObjectDirector::s_instance = nullptr;

} // namespace Field
