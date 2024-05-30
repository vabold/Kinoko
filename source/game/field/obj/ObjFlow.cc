#include "game/field/obj/ObjFlow.hh"
#include "game/system/ResourceManager.hh"
#include <egg/util/Stream.hh>
#include <cstring>
namespace Field
{

ObjFlow::ObjFlow(const char * filename) {
    size_t size;
    void* file = System::ResourceManager::Instance()->getFile(filename, &size, System::ArchiveId::Core);
    
    m_count = parse<u16>(*reinterpret_cast<u16*>(file));
    m_rawAttrs = reinterpret_cast<ObjAttrs*>(reinterpret_cast<u8*>(file) + sizeof(u16));
    m_rawSlots = reinterpret_cast<u16*>(reinterpret_cast<u8*>(file) + sizeof(ObjAttrs) * m_count + sizeof(u16));
}

u16 ObjFlow::count() const {
    return m_count;
}
ObjAttrs* ObjFlow::attrs(u16 idx) const {
    return &m_rawAttrs[idx];
}
u16 ObjFlow::slots(u16 idx) const {
    return parse<u16>(m_rawSlots[idx]);
}

u16 ObjFlow::getIdFromName(const char* name) const {
    for (s32 i = 0; i < m_count; i++) {
        if (strcmp(m_rawAttrs[i].name(), name) == 0) {
            return m_rawAttrs[i].id();
        }
    }
    return 0;
}

} // namespace Field