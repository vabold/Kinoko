#include "MapdataJugemPoint.hh"

#include "game/system/CourseMap.hh"

namespace System {

/// @addr{0x805183A8}
MapdataJugemPoint::MapdataJugemPoint(const SData *data) : m_rawData(data) {
    u8 *unsafeData = reinterpret_cast<u8 *>(const_cast<SData *>(data));
    EGG::RamStream stream = EGG::RamStream(unsafeData, sizeof(SData));
    read(stream);
}

void MapdataJugemPoint::read(EGG::Stream &stream) {
    m_pos.read(stream);
    m_rot.read(stream);
}

const EGG::Vector3f &MapdataJugemPoint::pos() const {
    return m_pos;
}

const EGG::Vector3f &MapdataJugemPoint::rot() const {
    return m_rot;
}

/// @addr{Inlined at 0x805130C4}
MapdataJugemPointAccessor::MapdataJugemPointAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataJugemPoint, MapdataJugemPoint::SData>(header) {
    init(reinterpret_cast<const MapdataJugemPoint::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));
}

MapdataJugemPointAccessor::~MapdataJugemPointAccessor() = default;

} // namespace System
