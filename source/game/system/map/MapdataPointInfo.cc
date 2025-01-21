#include "MapdataPointInfo.hh"

#include "game/system/CourseMap.hh"

namespace System {

MapdataPointInfo::MapdataPointInfo(const SData *data) : m_rawData(data) {
    EGG::RamStream stream =
            EGG::RamStream(data, sizeof(SData) + parse<u16>(data->pointCount) * sizeof(Point));
    read(stream);
}

MapdataPointInfo::~MapdataPointInfo() {
    delete m_points.data();
}

void MapdataPointInfo::read(EGG::RamStream &stream) {
    u16 count = stream.read_u16();

    m_points = std::span<Point>(new Point[count], count);

    for (auto &setting : m_settings) {
        setting = stream.read_u8();
    }

    for (auto &point : m_points) {
        EGG::Vector3f pos;
        pos.read(stream);

        u16 settings[2];
        settings[0] = stream.read_u16();
        settings[1] = stream.read_u16();

        point = Point(pos, {settings[0], settings[1]});
    }
}

/// @addr{0x80515D3C}
MapdataPointInfoAccessor::MapdataPointInfoAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataPointInfo, MapdataPointInfo::SData>(header) {
    init(reinterpret_cast<const MapdataPointInfo::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));
}

MapdataPointInfoAccessor::~MapdataPointInfoAccessor() = default;

void MapdataPointInfoAccessor::init(const MapdataPointInfo::SData *start, u16 count) {
    if (count != 0) {
        m_entryCount = count;
        m_entries = new MapdataPointInfo *[count];
    }

    uintptr_t data = reinterpret_cast<uintptr_t>(start);

    for (u16 i = 0; i < count; ++i) {
        m_entries[i] = new MapdataPointInfo(reinterpret_cast<MapdataPointInfo::SData *>(data));
        data += m_entries[i]->pointCount() * sizeof(MapdataPointInfo::Point) +
                offsetof(MapdataPointInfo::SData, MapdataPointInfo::SData::points);
    }
}

} // namespace System
