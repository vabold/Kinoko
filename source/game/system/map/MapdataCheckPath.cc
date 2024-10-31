#include "game/system/map/MapdataCheckPath.hh"

namespace System {

MapdataCheckPath::MapdataCheckPath(const SData *data) : m_rawData(data), m_depth(-1) {
    u8 *unsafeData = reinterpret_cast<u8 *>(const_cast<SData *>(data));
    EGG::RamStream stream = EGG::RamStream(unsafeData, sizeof(SData));
    read(stream);
    m_oneOverCount = 1.0f / m_size;
}

void MapdataCheckPath::read(EGG::Stream &stream) {
    m_start = stream.read_u8();
    m_size = stream.read_u8();
    for (auto &prev : m_prev) {
        prev = stream.read_u8();
    }

    for (auto &next : m_next) {
        next = stream.read_u8();
    }
}

MapdataCheckPathAccessor::MapdataCheckPathAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataCheckPath, MapdataCheckPath::SData>(header) {
    init(reinterpret_cast<const MapdataCheckPath::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));
}

MapdataCheckPathAccessor::~MapdataCheckPathAccessor() = default;

} // namespace System
