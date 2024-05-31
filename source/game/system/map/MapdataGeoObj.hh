#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/math/Vector.hh>

namespace System {

class MapdataGeoObj {
public:
    static constexpr int N_SETTINGS = 8;
    struct SData {
        u16 id;
        EGG::Vector3f pos;
        EGG::Vector3f rot;
        EGG::Vector3f scale;
        u16 route;
        u16 settings[N_SETTINGS];
        u16 presenceFlags;
    };
    static_assert(sizeof(SData) == 0x3c);

    MapdataGeoObj(const SData *data);

    /// @beginGetters
    u16 id() const;
    const EGG::Vector3f &position() const;
    const EGG::Vector3f &rotation() const;
    const EGG::Vector3f &scale() const;
    /// @endGetters

    void read(EGG::Stream &stream);

private:
    const SData *m_rawData;
    u16 m_id;
    EGG::Vector3f m_position;
    EGG::Vector3f m_rotation;
    EGG::Vector3f m_scale;
    u16 m_route;
    u16 m_settings[N_SETTINGS];
    u16 m_presenceFlags;
};

class MapdataGeoObjAccessor : public MapdataAccessorBase<MapdataGeoObj, MapdataGeoObj::SData> {
public:
    MapdataGeoObjAccessor(const MapSectionHeader *header);
    ~MapdataGeoObjAccessor() override;
};

} // namespace System
