#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/math/Vector.hh>

namespace System {

class MapdataCheckPoint;
class MapdataCheckPointAccessor;

struct LinkedCheckpoint {
    MapdataCheckPoint *checkpoint;
    EGG::Vector2f p0diff;
    EGG::Vector2f p1diff;
    f32 distance;
};

class MapdataCheckPoint {
public:
    struct SData {
        EGG::Vector2f left;
        EGG::Vector2f right;
        s8 jugemIndex;
        s8 checkArea;
        u8 prevPt;
        u8 nextPt;
    };
    STATIC_ASSERT(sizeof(SData) == 0x14);

    MapdataCheckPoint(const SData *data);
    void read(EGG::Stream &stream);
    void initCheckpointLinks(MapdataCheckPointAccessor &accessor, int id);

    /// @beginGetters
    [[nodiscard]] s8 checkArea() const;
    /// @endGetters

    enum class CheckArea {
        NORMAL_CHECKPOINT = -1, ///< Only used for picking respawn position
        FINISH_LINE = 0,        ///< Triggers a lap change
    };

private:
    [[nodiscard]] MapdataCheckPoint *nextPoint(size_t i) const;

    static constexpr size_t MAX_NEIGHBORS = 6;

    const SData *m_rawData;
    EGG::Vector2f m_left;
    EGG::Vector2f m_right;
    s8 m_jugemIndex; ///< Index of respawn point associated with this checkpoint. Players who die
                     ///< here will be respawned at this point.
    /// Either:
    /// - a @ref `NORMAL_CHECKPOINT` (-1) used to calulate respawns,
    /// - a @ref `FINISH_LINE` (0) which updates the lap count when crossed, or
    /// - a "key checkpoint" (1-127) used to ensure racers travel around the entire
    /// course before proceeding to the next lap. the type value represents the index,
    /// i.e. racers must pass checkpoint with @ref `m_type` 1, then 2, then 3 etc..
    s8 m_checkArea;
    u8 m_prevPt;
    u8 m_nextPt;
    u16 m_nextCount;
    u16 m_prevCount;
    EGG::Vector2f m_midpoint;
    EGG::Vector2f m_dir;
    u16 m_id;
    std::array<MapdataCheckPoint *, MAX_NEIGHBORS> m_prevPoints;
    std::array<LinkedCheckpoint, MAX_NEIGHBORS> m_nextPoints;
};

class MapdataCheckPointAccessor
    : public MapdataAccessorBase<MapdataCheckPoint, MapdataCheckPoint::SData> {
public:
    MapdataCheckPointAccessor(const MapSectionHeader *header);
    ~MapdataCheckPointAccessor() override;

    [[nodiscard]] s8 lastKcpType() const;

private:
    void findFinishAndLastKcp();
    void init();

    s8 m_lastKcpType;
    u16 m_finishLineCheckpointId;
};

} // namespace System
