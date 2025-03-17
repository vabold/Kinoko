#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/math/Vector.hh>
#include <egg/util/Stream.hh>

#include <span>

namespace System {

class MapdataPointInfo {
public:
    struct Point {
        EGG::Vector3f pos;
        u16 setting[2];
    };

    struct SData {
        u16 pointCount;
        u8 settings[2];
        Point points[];
    };
    STATIC_ASSERT(sizeof(SData) == 0x4);

    MapdataPointInfo(const SData *data);
    ~MapdataPointInfo();

    void read(EGG::RamStream &stream);

    [[nodiscard]] size_t pointCount() const;

private:
    const SData *m_rawData;
    std::array<u8, 2> m_settings;
    std::span<Point> m_points;
};

class MapdataPointInfoAccessor
    : public MapdataAccessorBase<MapdataPointInfo, MapdataPointInfo::SData> {
public:
    MapdataPointInfoAccessor(const MapSectionHeader *header);
    ~MapdataPointInfoAccessor() override;

    void init(const MapdataPointInfo::SData *start, u16 count);
};

} // namespace System
