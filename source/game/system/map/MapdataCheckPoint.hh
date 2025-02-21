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

    enum class SectorOccupancy {
        InsideSector,  ///< Player is inside the given checkpoint group
        OutsideSector, ///< Player is outside the given checkpoint group
        BetweenSides,  ///< Player is between sides of the checkpoint group but not
                       ///< between this checkpoint and next
    };

    MapdataCheckPoint(const SData *data);
    void read(EGG::Stream &stream);
    void initCheckpointLinks(MapdataCheckPointAccessor &accessor, int id);
    [[nodiscard]] SectorOccupancy checkSectorAndDistanceRatio(const EGG::Vector3f &pos,
            f32 &distanceRatio) const;

    [[nodiscard]] u16 getEntryOffsetMs(const EGG::Vector2f &prevPos,
            const EGG::Vector2f &pos) const;
    [[nodiscard]] f32 getEntryOffsetExact(const EGG::Vector2f &prevPos,
            const EGG::Vector2f &pos) const;

    [[nodiscard]] bool isNormalCheckpoint() const {
        return static_cast<CheckArea>(m_checkArea) == CheckArea::NormalCheckpoint;
    }

    [[nodiscard]] bool isFinishLine() const {
        return static_cast<CheckArea>(m_checkArea) == CheckArea::FinishLine;
    }

    /// @beginSetters
    void setSearched() {
        m_searched = true;
    }

    void clearSearched() {
        m_searched = false;
    }
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] bool searched() const {
        return m_searched;
    }

    [[nodiscard]] s8 jugemIndex() const {
        return m_jugemIndex;
    }

    [[nodiscard]] s8 checkArea() const {
        return m_checkArea;
    }

    [[nodiscard]] u16 nextCount() const {
        return m_nextCount;
    }

    [[nodiscard]] u16 prevCount() const {
        return m_prevCount;
    }

    [[nodiscard]] u16 id() const {
        return m_id;
    }

    [[nodiscard]] MapdataCheckPoint *prevPoint(size_t i) const {
        ASSERT(i < m_prevPoints.size());
        return m_prevPoints[i];
    }

    [[nodiscard]] MapdataCheckPoint *nextPoint(size_t i) const {
        ASSERT(i < m_nextPoints.size());
        return m_nextPoints[i].checkpoint;
    }
    /// @endGetters

    enum class CheckArea {
        NormalCheckpoint = -1, ///< Only used for picking respawn position
        FinishLine = 0,        ///< Triggers a lap change
    };

private:
    [[nodiscard]] SectorOccupancy checkSectorAndDistanceRatio(const LinkedCheckpoint &next,
            const EGG::Vector2f &p0, const EGG::Vector2f &p1, f32 &distanceRatio) const;
    [[nodiscard]] bool checkSector(const LinkedCheckpoint &next, const EGG::Vector2f &p0,
            const EGG::Vector2f &p1) const;
    [[nodiscard]] bool checkDistanceRatio(const LinkedCheckpoint &next, const EGG::Vector2f &p0,
            const EGG::Vector2f &p1, f32 &distanceRatio) const;

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
    bool m_searched;
    u16 m_id;
    std::array<MapdataCheckPoint *, MAX_NEIGHBORS> m_prevPoints;
    std::array<LinkedCheckpoint, MAX_NEIGHBORS> m_nextPoints;
};

class MapdataCheckPointAccessor
    : public MapdataAccessorBase<MapdataCheckPoint, MapdataCheckPoint::SData> {
public:
    MapdataCheckPointAccessor(const MapSectionHeader *header);
    ~MapdataCheckPointAccessor() override;

    [[nodiscard]] s8 lastKcpType() const {
        return m_lastKcpType;
    }

private:
    void init();

    s8 m_lastKcpType;
    u16 m_finishLineCheckpointId;
};

} // namespace System
