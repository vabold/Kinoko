#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/math/Vector.hh>

namespace System {

class MapdataGeoObj {
public:
    struct SData {
        u16 id;
        EGG::Vector3f position;
        EGG::Vector3f rotation;
        EGG::Vector3f scale;
        s16 pathId;
        u16 settings[8];
        u16 presenceFlag;
    };

    MapdataGeoObj(const SData *data);
    void read(EGG::Stream &stream);

    /// @beginGetters
    u16 id() const;
    const EGG::Vector3f &pos() const;
    const EGG::Vector3f &rot() const;
    const EGG::Vector3f &scale() const;
    u16 presenceFlag() const;
    /// @endGetters

private:
    const SData *m_rawData;
    u16 m_id;
    EGG::Vector3f m_pos;
    EGG::Vector3f m_rot;
    EGG::Vector3f m_scale;
    s16 m_pathId;
    std::array<u16, 8> m_settings;
    u16 m_presenceFlag;
};

class MapdataGeoObjAccessor : public MapdataAccessorBase<MapdataGeoObj, MapdataGeoObj::SData> {
public:
    MapdataGeoObjAccessor(const MapSectionHeader *header);
    ~MapdataGeoObjAccessor() override;
};

} // namespace System
