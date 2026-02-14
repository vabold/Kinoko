#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/math/Vector.hh>

namespace System {

class MapdataStartPoint {
public:
    struct SData {
        EGG::Vector3f pos;
        EGG::Vector3f rot;
        // Pre Revision 1830: End of structure
        s16 playerIndex;
    };

    MapdataStartPoint(const SData *data);

    void read(EGG::Stream &stream);
    void findKartStartPoint(EGG::Vector3f &pos, EGG::Vector3f &angles, u8 placement,
            u8 playerCount);

private:
    [[maybe_unused]] const SData *m_rawData;
    EGG::Vector3f m_position;
    EGG::Vector3f m_rotation;
    s16 m_playerIndex;
};

class MapdataStartPointAccessor
    : public MapdataAccessorBase<MapdataStartPoint, MapdataStartPoint::SData> {
public:
    MapdataStartPointAccessor(const MapSectionHeader *header);
    ~MapdataStartPointAccessor() override;
};

} // namespace System
