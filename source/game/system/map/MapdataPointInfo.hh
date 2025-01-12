#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/math/Vector.hh>
#include <egg/util/Stream.hh>

#include <vector>

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

    void read(EGG::RamStream &stream);

    [[nodiscard]] u16 pointCount() const;

private:
    const SData *m_rawData;
    u16 m_pointCount;
    std::array<u8, 2> m_settings;
    std::vector<Point> m_points;
};

class MapdataPointInfoAccessor
    : public MapdataAccessorBase<MapdataPointInfo, MapdataPointInfo::SData> {
public:
    MapdataPointInfoAccessor(const MapSectionHeader *header);
    ~MapdataPointInfoAccessor() override;

    void init(const MapdataPointInfo::SData *start, u16 count) override {
        if (count != 0) {
            m_entryCount = count;
            m_entries = new MapdataPointInfo *[count];
        }

        const MapdataPointInfo::SData *data = start;

        for (u16 i = 0; i < count; ++i) {
            m_entries[i] = new MapdataPointInfo(data);
            data += m_entries[i]->pointCount() * 4 + 1;
        }
    }
};

} // namespace System
