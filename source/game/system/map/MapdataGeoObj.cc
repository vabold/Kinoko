#include "MapdataGeoObj.hh"

namespace System {

MapdataGeoObj::MapdataGeoObj(const SData *data) : m_rawData(data) {
    EGG::RamStream stream = EGG::RamStream(data, sizeof(SData));
    read(stream);
}

void MapdataGeoObj::read(EGG::Stream &stream) {
    m_id = stream.read_u16();
    stream.skip(2);
    m_pos.read(stream);
    m_rot.read(stream);
    m_scale.read(stream);
    m_pathId = stream.read_s16();

    for (auto &setting : m_settings) {
        setting = stream.read_s16();
    }

    m_presenceFlag = stream.read_u16();
}

MapdataGeoObjAccessor::MapdataGeoObjAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataGeoObj, MapdataGeoObj::SData>(header) {
    init(reinterpret_cast<const MapdataGeoObj::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));
}

MapdataGeoObjAccessor::~MapdataGeoObjAccessor() = default;

} // namespace System
