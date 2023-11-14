#include "MapdataFileAccessor.hh"

namespace System {

MapdataFileAccessor::MapdataFileAccessor(const MapdataFileAccessor::SData *data) : m_rawData(data) {
    u32 offset = parse<u16>(data->headerSize) - parse<u16>(data->sectionCount) * 4;
    m_sectionDefOffset = offset;
    m_sectionDef = reinterpret_cast<const u32 *>(reinterpret_cast<const u8 *>(m_rawData) + offset);
    m_version = offset > 12 ? parse<u32>(m_sectionDef[-1]) : 0;
}

const MapSectionHeader *MapdataFileAccessor::findSection(u32 signature) const {
    const MapSectionHeader *sectionPtr = nullptr;

    for (size_t i = 0; i < parse<u16>(m_rawData->sectionCount); ++i) {
        const MapSectionHeader *header =
                reinterpret_cast<const MapSectionHeader *>(reinterpret_cast<const u8 *>(m_rawData) +
                        parse<u16>(m_rawData->headerSize) + parse<u32>(m_sectionDef[i]));
        if (parse<u32>(header->magic) == signature) {
            sectionPtr = header;
            break;
        }
    }

    return sectionPtr;
}

} // namespace System
