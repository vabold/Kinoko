#include "MapdataJugemPoint.hh"

#include "game/system/CourseMap.hh"

namespace System {

/// @addr{0x805183A8}
MapdataJugemPoint::MapdataJugemPoint(const SData *data) : m_rawData(data) {
    EGG::RamStream stream = EGG::RamStream(data, sizeof(SData));
    read(stream);
}

void MapdataJugemPoint::read(EGG::Stream &stream) {
    m_pos.read(stream);
    m_rot.read(stream);
}

/// @addr{Inlined at 0x805130C4}
MapdataJugemPointAccessor::MapdataJugemPointAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataJugemPoint, MapdataJugemPoint::SData>(header) {
    init(reinterpret_cast<const MapdataJugemPoint::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));
}

MapdataJugemPointAccessor::~MapdataJugemPointAccessor() = default;

} // namespace System
