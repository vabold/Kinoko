#include "game/object/ObjFlow.hh"
#include "game/system/ResourceManager.hh"

namespace Object
{

ObjFlow::ObjFlow(const char * /*filename*/) {
    //void* unsafeData = LoadFile(filename);
}

void *ObjFlow::LoadFile(const char *filename) {
    return System::ResourceManager::Instance()->getFile(filename, nullptr, System::ArchiveId::Core);
}

} // namespace Object