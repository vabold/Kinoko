#include "game/field/obj/ObjFlow.hh"
#include "game/system/ResourceManager.hh"

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

} // namespace Field