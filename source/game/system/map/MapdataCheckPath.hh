#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/math/Vector.hh>

namespace System {

class MapdataCheckPath {
public:
    struct SData {
        u8 start;
        u8 size;

        u8 prev[6];
        u8 next[6];
        u8 _0e[0x10 - 0x0e];
    };
    STATIC_ASSERT(sizeof(SData) == 0x10);

    MapdataCheckPath(const SData *data);
    void read(EGG::Stream &stream);

private:
    const SData *m_rawData;
    u8 m_start;               ///< Index of the first checkpoint in this checkpath
    u8 m_size;                ///< Number of checkpoints in this checkpath
    std::array<u8, 6> m_prev; ///< Indices of previous connected checkpaths
    std::array<u8, 6> m_next; ///< Indices of next connected checkpaths
    s8 m_depth; ///< Number of checkpaths away from first checkpath (i.e. distance from start)
    f32 m_oneOverCount;
};

class MapdataCheckPathAccessor
    : public MapdataAccessorBase<MapdataCheckPath, MapdataCheckPath::SData> {
public:
    MapdataCheckPathAccessor(const MapSectionHeader *header);
    ~MapdataCheckPathAccessor() override;
};

} // namespace System
