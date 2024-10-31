#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/math/Vector.hh>

namespace System {

class MapdataCheckPoint {
public:
    struct SData {
        EGG::Vector2f left;
        EGG::Vector2f right;
        u8 jugemIndex;
        s8 checkArea;
        u8 prevPt;
        u8 nextPt;
    };
    STATIC_ASSERT(sizeof(SData) == 0x14);

    MapdataCheckPoint(const SData *data);
    void read(EGG::Stream &stream);

private:
    const SData *m_rawData;
    EGG::Vector2f m_left;
    EGG::Vector2f m_right;
    u8 m_jugemIndex; ///< Index of respawn point associated with this checkpoint. Players who die
                     ///< here will be respawned at this point.
    s8 m_checkArea;
    u8 m_prevPt;
    u8 m_nextPt;
    u16 m_nextCount;
    u16 m_prevCount;
    EGG::Vector2f m_midpoint;
    EGG::Vector2f m_dir;
};

class MapdataCheckPointAccessor
    : public MapdataAccessorBase<MapdataCheckPoint, MapdataCheckPoint::SData> {
public:
    MapdataCheckPointAccessor(const MapSectionHeader *header);
    ~MapdataCheckPointAccessor() override;
};

} // namespace System
