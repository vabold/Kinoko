#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/math/Vector.hh>

namespace System {

class MapdataCheckPathAccessor;

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

    void findDepth(s8 depth, const MapdataCheckPathAccessor &accessor);

    [[nodiscard]] bool isPointInPath(u16 checkpointId) const {
        return m_start <= checkpointId && checkpointId <= end();
    }

    static constexpr size_t MAX_NEIGHBORS = 6;

    /// @beginGetters
    [[nodiscard]] u8 start() const {
        return m_start;
    }

    [[nodiscard]] u8 end() const {
        return m_start + m_size - 1;
    }

    [[nodiscard]] const std::array<u8, MAX_NEIGHBORS> &next() const {
        return m_next;
    }

    [[nodiscard]] const std::array<u8, MAX_NEIGHBORS> &prev() const {
        return m_prev;
    }

    [[nodiscard]] s8 depth() const {
        return m_depth;
    }

    [[nodiscard]] f32 oneOverCount() const {
        return m_oneOverCount;
    }
    /// @endGetters

private:
    const SData *m_rawData;
    u8 m_start;                           ///< Index of the first checkpoint in this checkpath
    u8 m_size;                            ///< Number of checkpoints in this checkpath
    std::array<u8, MAX_NEIGHBORS> m_prev; ///< Indices of previous connected checkpaths
    std::array<u8, MAX_NEIGHBORS> m_next; ///< Indices of next connected checkpaths
    s8 m_depth;                           ///< Number of checkpaths away from first checkpath (i.e.
                                          ///< distance from start)
    f32 m_oneOverCount;
};

class MapdataCheckPathAccessor
    : public MapdataAccessorBase<MapdataCheckPath, MapdataCheckPath::SData> {
public:
    MapdataCheckPathAccessor(const MapSectionHeader *header);
    ~MapdataCheckPathAccessor() override;

    [[nodiscard]] MapdataCheckPath *findCheckpathForCheckpoint(u16 checkpointId) const;

    [[nodiscard]] f32 lapProportion() const;

private:
    /// @brief Minimum proportion of a lap a checkpath can be. Calculated as
    /// 1/(maxDepth+1).
    /// @details Another way to think of it: maxDepth+1 is the number of
    /// checkpaths in the longest route through the course, where longest means
    /// *most checkpaths traversed*, not most _distance_ traversed. So if one
    /// plans their route to hit the most checkpaths possible (no backtracking),
    /// they hit maxDepth+1 checkpaths, and each checkpath is `lapProportion`%
    /// of the total checkpaths on the route.
    f32 m_lapProportion;
};

} // namespace System
