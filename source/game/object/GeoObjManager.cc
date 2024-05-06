#include "game/object/GeoObjManager.hh"

namespace Object
{

GeoObjManager::GeoObjManager() {
    // more stuff here
}

GeoObjManager::~GeoObjManager() {
    // more stuff here
}

GeoObjManager *GeoObjManager::CreateInstance() {
    if (!s_instance) {
        s_instance = new GeoObjManager;
    }
    // more stuff here
    return s_instance;
}
void GeoObjManager::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    // more stuff here
    s_instance = nullptr;
}
GeoObjManager *GeoObjManager::Instance() {
    return s_instance;
}

GeoObjManager* GeoObjManager::s_instance = nullptr;

} // namespace Object
