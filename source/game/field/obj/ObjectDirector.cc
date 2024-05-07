#include "game/field/obj/ObjectDirector.hh"
#include "game/field/obj/ObjDrivableHolder.hh"
#include "game/system/CourseMap.hh"
#include "game/system/RaceConfig.hh"

namespace Field
{

ObjectDirector::ObjectDirector() {
    // more stuff here
    m_objFlow = new ObjFlow("ObjFlow.bin");
    m_getHitTableItem = new GeoHitTable("GeoHitTableItem.bin");
    m_getHitTableItemObj = new GeoHitTable("GeoHitTableItemObj.bin");
    m_getHitTableKart = new GeoHitTable("GeoHitTableKart.bin");
    m_getHitTableKartObj = new GeoHitTable("GeoHitTableKartObj.bin");
    
    auto raceScenario = System::RaceConfig::Instance()->raceScenario();
    m_isOnlineGameMode = raceScenario.isOnlineGameMode();
    m_isTimeTrials = raceScenario.isInTimeTrials();

    Course course = raceScenario.course;
    if (course != Course::Moonview_Highway
    && course != Course::Galaxy_Colosseum
    && course != (Course)0x33
    && course != Course::DS_Desert_Hills
    && course != Course::GBA_Shy_Guy_Beach) {
        m_managedGroup = nullptr;
    } else {
        m_managedGroup = new ObjGroup;
        m_managedGroup->m_count = 0;
    }
    
    for (s32 i = 0; i < 5; i++) {
        m_objArrays[i].m_count = 0;
        m_objArrays[i].m_array = new GeoObject*[800];
    }

    m_objs = new GeoObject*[800];
    _44 = new EGG::Vector3f[200];
    m_collisionScenarios = new u8[800];

    for (s32 i = 0; i < 4; i++) {
        _124[i] = 0.0f;
    }
}

ObjectDirector::~ObjectDirector() {
    // more stuff here
}

ObjectDirector *ObjectDirector::CreateInstance() {
    if (!s_instance) {
        s_instance = new ObjectDirector;
    }
    ObjDrivableHolder::CreateInstance();
    s_instance->CreateObjects(true);
    // destroy some heap, and then
    s_instance->CreateObjects(false);
    // FUN_8088be00()
    return s_instance;
}
void ObjectDirector::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    // more stuff here
    s_instance = nullptr;
}
ObjectDirector *ObjectDirector::Instance() {
    return s_instance;
}

void ObjectDirector::CreateObjects(bool isMii) {
    // isMii will just create all mii-related objects, for now we don't care about those so let's not for now
    if (isMii) return;

    m_hasSunDSOrWiggler = false;

    u32 objCount;
    if (System::CourseMap::Instance()->geoObj() == nullptr) {
        objCount = 0;
    } else {
        objCount = System::CourseMap::Instance()->geoObj()->size();
    }

    for (u32 i = 0; i < objCount; i++) {
        System::MapdataGeoObj* obj = System::CourseMap::Instance()->getGeoObj(i);
        // a bunch of objects have sorta custom handlers, we don't care rn though
        ConstructObject(obj);
    }
}

void ObjectDirector::ConstructObject(System::MapdataGeoObj* pObj) {
    u16 slot = m_objFlow->m_slots[pObj->id()];
    ObjAttrs* attrs = &m_objFlow->m_attrs[slot];
    const char* name = attrs->name;
    // stay tuned
}

ObjectDirector* ObjectDirector::s_instance = nullptr;

} // namespace Field
