#include "MapdataPointInfo.hh"

#include "game/system/CourseMap.hh"

namespace System {

MapdataPointInfo::MapdataPointInfo(const SData *data) : m_rawData(data) {
    u8 *unsafeData = reinterpret_cast<u8 *>(const_cast<SData *>(data));
    EGG::RamStream stream = EGG::RamStream(unsafeData, sizeof(SData));
    read(stream);
}

void MapdataPointInfo::read(EGG::RamStream &stream) {
    m_pointCount = stream.read_u16();

    for (auto &setting : m_settings) {
        setting = stream.read_u8();
    }

    // Resize the buffer to account for dynamic number of points
    stream.extendSize(m_pointCount * sizeof(Point));

    m_points.reserve(m_pointCount);

    for (size_t i = 0; i < m_pointCount; ++i) {
        EGG::Vector3f pos;
        pos.read(stream);

        u8 settings[2];
        settings[0] = stream.read_u16();
        settings[1] = stream.read_u16();

        m_points.push_back(Point(pos, {settings[0], settings[1]}));
    }
}

u16 MapdataPointInfo::pointCount() const {
    return m_pointCount;
}

/// @addr{0x80515D3C}
MapdataPointInfoAccessor::MapdataPointInfoAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataPointInfo, MapdataPointInfo::SData>(header) {
    init(reinterpret_cast<const MapdataPointInfo::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));
}

MapdataPointInfoAccessor::~MapdataPointInfoAccessor() = default;

} // namespace System
