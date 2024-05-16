#include "game/field/obj/ObjFlow.hh"
#include "game/system/ResourceManager.hh"
#include <cstring>
namespace Field
{

ObjFlow::ObjFlow(const char * filename) {
    _ObjFlow* file = reinterpret_cast<_ObjFlow*>(LoadFile(filename));
    m_count = file->m_count;
    m_attrs = file->m_attrs;
    m_slots = file->m_slots;
}

void *ObjFlow::LoadFile(const char *filename) {
    return System::ResourceManager::Instance()->getFile(filename, nullptr, System::ArchiveId::Core);
}

u16 ObjFlow::getIdFromName(const char* name) const {
    for (s32 i = 0; i < m_count; i++) {
        if (strcmp(m_attrs[i].name, name) == 0) {
            return m_attrs[i].id;
        }
    }
    return 0;
}

} // namespace Field