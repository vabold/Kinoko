#include "MapdataGeoObj.hh"

#include "game/system/CourseMap.hh"

namespace System {

MapdataGeoObj::MapdataGeoObj(const SData *data) : m_rawData(data) {
    u8 *unsafeData = reinterpret_cast<u8 *>(const_cast<SData *>(data));
    EGG::RamStream stream = EGG::RamStream(unsafeData, sizeof(SData));
    read(stream);
}

void MapdataGeoObj::read(EGG::Stream &stream) {
    m_id = stream.read_u16();
    stream.skip(sizeof(u16));
    m_position.read(stream);
    m_rotation.read(stream);
    m_scale.read(stream);
    m_route = stream.read_u16();
    for (s32 i = 0; i < N_SETTINGS; i++) {
        m_settings[i] = stream.read_u16();
    }
    m_presenceFlags = stream.read_u16();
}

u16 MapdataGeoObj::id() const {
    return m_id;
}
EGG::Vector3f MapdataGeoObj::position() const {
    return m_position;
}
EGG::Vector3f MapdataGeoObj::rotation() const {
    return m_rotation;
}
EGG::Vector3f MapdataGeoObj::scale() const {
    return m_scale;
}

MapdataGeoObjAccessor::MapdataGeoObjAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataGeoObj, MapdataGeoObj::SData>(header) {
    init(reinterpret_cast<const MapdataGeoObj::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));
}

MapdataGeoObjAccessor::~MapdataGeoObjAccessor() = default;

} // namespace System
