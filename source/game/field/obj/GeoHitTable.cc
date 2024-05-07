#include "game/field/obj/GeoHitTable.hh"
#include "egg/util/Stream.hh"
#include "game/system/ResourceManager.hh"

namespace Field {

GeoHitTable::GeoHitTable(const char* filename) {
    constexpr int OFFSET_HITPARAMS = 8;
    auto atOffset = [](void* buffer, u32 offset) -> void* {
        return reinterpret_cast<void*>(reinterpret_cast<u8*>(buffer) + offset);
    };

    void* file = GeoHitTable::LoadFile(filename);
    EGG::RamStream header = EGG::RamStream(reinterpret_cast<u8*>(file), sizeof(GeoHitTableHeader));

    m_objectCount = header.read_u32();
    m_hitParameterCount = header.read_u32();
    m_hitParametersArray = reinterpret_cast<void**>(new u8[m_objectCount * sizeof(void*)]);

    u32 hitParamsSize = m_hitParameterCount * sizeof(u16);
    u32 entrySize = sizeof(u16) + hitParamsSize; // add id
    u8* hitParamData = reinterpret_cast<u8*>(atOffset(file, OFFSET_HITPARAMS));
    EGG::RamStream hitParams = EGG::RamStream(hitParamData, entrySize * m_objectCount);

    for (u16 i = 0; i < m_objectCount; i++) {
        hitParams.skip(sizeof(u16));
        m_hitParametersArray[i] = hitParams.data();
        hitParams.skip(hitParamsSize);
    }

    m_moreData = hitParams.data();
}

void* GeoHitTable::LoadFile(const char* filename) {
    return System::ResourceManager::Instance()->getFile(filename, nullptr, System::ArchiveId::Core);
}

GeoHitTable::~GeoHitTable() = default;

} // namespace Field