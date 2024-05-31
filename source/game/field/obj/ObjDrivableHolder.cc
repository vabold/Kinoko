#include "game/field/obj/ObjDrivableHolder.hh"

namespace Field {

ObjDrivableHolder *ObjDrivableHolder::CreateInstance() {
    if (!s_instance) {
        s_instance = new ObjDrivableHolder;
    }

    return s_instance;
}
void ObjDrivableHolder::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}
ObjDrivableHolder *ObjDrivableHolder::Instance() {
    return s_instance;
}

ObjDrivableHolder::ObjDrivableHolder() {
    static constexpr size_t CAPACITY = 400;
    m_objs = new GeoObjectDrivable *[CAPACITY];
    m_count = 0;
}

ObjDrivableHolder::~ObjDrivableHolder() {
    if (m_objs == nullptr) {
        return;
    }
    for (s32 i = 0; i < m_count; i++) {
        delete m_objs[i];
        m_objs[i] = nullptr;
    }
}

void ObjDrivableHolder::initObjs() {
    for (s32 i = 0; i < m_count; i++) {
        if (m_objs[i]) {
            // m_objs[i]->setup();
            // m_objs[i]->update();
        }
    }
}

s32 ObjDrivableHolder::push(GeoObjectDrivable *pObj) {
    for (s32 i = 0; i < m_count; i++) {
        if (m_objs[i] == pObj) {
            return -1;
        }
    }
    m_objs[m_count] = pObj;
    pObj->setDirectorIndex(m_count + 1000);
    m_count++;
    return m_count - 1;
}

ObjDrivableHolder *ObjDrivableHolder::s_instance = nullptr;

} // namespace Field
