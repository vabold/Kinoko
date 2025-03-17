#include "MapdataCannonPoint.hh"

#include "game/system/CourseMap.hh"

namespace System {

MapdataCannonPoint::MapdataCannonPoint(const SData *data) : m_rawData(data) {
    EGG::RamStream stream = EGG::RamStream(data, sizeof(SData));
    read(stream);
}

void MapdataCannonPoint::read(EGG::Stream &stream) {
    m_pos.read(stream);
    m_rot.read(stream);
    m_id = stream.read_u16();
    m_parameterIdx = stream.read_s16();
}

/// @addr{Inlined at 0x80512FA4}
MapdataCannonPointAccessor::MapdataCannonPointAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataCannonPoint, MapdataCannonPoint::SData>(header) {
    init(reinterpret_cast<const MapdataCannonPoint::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));
}

MapdataCannonPointAccessor::~MapdataCannonPointAccessor() = default;

} // namespace System
