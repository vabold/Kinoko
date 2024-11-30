#include "MapdataCheckPoint.hh"

#include "game/system/CourseMap.hh"
#include "game/system/map/MapdataCheckPath.hh"

#include <cstddef>
#include <ranges>

namespace System {

/// @addr{0x805154E4}
MapdataCheckPoint::MapdataCheckPoint(const SData *data)
    : m_rawData(data), m_nextCount(0), m_prevCount(0) {
    u8 *unsafeData = reinterpret_cast<u8 *>(const_cast<SData *>(data));
    EGG::RamStream stream = EGG::RamStream(unsafeData, sizeof(SData));
    read(stream);
    m_midpoint = 0.5f * (m_left + m_right);
    m_dir = EGG::Vector2f(m_left.y - m_right.y, m_right.x - m_left.x);
    m_dir.normalise();
}

void MapdataCheckPoint::read(EGG::Stream &stream) {
    m_left.read(stream);
    m_right.read(stream);
    m_jugemIndex = stream.read_s8();
    m_checkArea = stream.read_s8();
    m_prevPt = stream.read_u8();
    m_nextPt = stream.read_u8();
}

/// @addr{0x80515624}
/// @brief Calculates @ref m_nextPoints and @ref m_prevPoints from @ref m_nextPt
/// and @ref m_prevPt.
/// @details Also calculates the quadrilaterals for the next checkpoints,
/// filling the fields of @ref LinkedCheckpoint for each.
void MapdataCheckPoint::initCheckpointLinks(MapdataCheckPointAccessor &accessor, int id) {
    m_id = id;
    const auto *checkPathAccessor = CourseMap::Instance()->checkPath();

    // Calculate the quadrilateral's `m_prevPoints`. If the check point is the
    // first in its group, it has multiple previous checkpoints defined by its
    // preceding checkpaths
    if (m_prevPt == 0xFF) {
        MapdataCheckPath *checkpath = checkPathAccessor->findCheckpathForCheckpoint(id);
        if (checkpath) {
            m_prevCount = 0;

            for (auto [i, prevID] : std::views::enumerate(checkpath->prev())) {
                if (prevID == 0xFF) {
                    continue;
                }

                m_prevPoints[i] = accessor.get(checkPathAccessor->get(prevID)->end());
                ++m_prevCount;
            }
        }
    } else {
        m_prevPoints[0] = accessor.get(m_prevPt);
        ++m_prevCount;
    }

    // Calculate the quadrilateral's `m_nextPoints`. If the checkpoint is the
    // last in its group, it can have multiple quadrilaterals (and
    // nextCheckpoint) which are determined by its next path(s)
    if (m_nextPt == 0xFF) {
        MapdataCheckPath *checkpath = checkPathAccessor->findCheckpathForCheckpoint(id);
        if (checkpath) {
            m_nextCount = 0;

            for (auto [i, nextID] : std::views::enumerate(checkpath->next())) {
                if (nextID == 0xFF) {
                    continue;
                }

                m_nextPoints[i].checkpoint = accessor.get(checkPathAccessor->get(nextID)->start());
                ++m_nextCount;
            }
        }
    } else {
        m_nextPoints[0].checkpoint = accessor.get(m_nextPt);
        ++m_nextCount;
    }

    // Form the checkpoint's quadrilateral(s)
    for (auto [i, next] : std::views::enumerate(m_nextPoints)) {
        if (i < m_nextCount) {
            auto &nextLinked = m_nextPoints[i];
            auto *nextPoint = nextLinked.checkpoint;

            next.distance = (nextPoint->m_midpoint - m_midpoint).normalise();
            next.p0diff = nextPoint->m_left - m_left;
            next.p1diff = nextPoint->m_right - m_right;
        } else {
            next.distance = 0.0f;
            next.p0diff = EGG::Vector2f::zero;
            next.p1diff = EGG::Vector2f::zero;
        }
    }
}

/// @addr{0x80510D7C}
/// @see MapdataCheckPoint::checkSectorAndCheckpointCompletion_
/// @rename
MapdataCheckPoint::SectorOccupancy MapdataCheckPoint::checkSectorAndCheckpointCompletion(
        const EGG::Vector3f &pos, float *completion) const {
    EGG::Vector2f p1 = m_right;
    p1.y = pos.z - p1.y;
    p1.x = pos.x - p1.x;

    for (auto &next : m_nextPoints) {
        EGG::Vector2f p0 = next.checkpoint->m_left;
        p0.y = pos.z - p0.y;
        p0.x = pos.x - p0.x;
        MapdataCheckPoint::SectorOccupancy result =
                checkSectorAndCheckpointCompletion_(next, p0, p1, completion);

        if (result == SectorOccupancy::OutsideSector) {
            continue;
        } else {
            return result;
        }
    }

    return SectorOccupancy::OutsideSector;
}

void MapdataCheckPoint::setSearched() {
    m_searched = true;
}

void MapdataCheckPoint::clearSearched() {
    m_searched = false;
}

bool MapdataCheckPoint::searched() const {
    return m_searched;
}

s8 MapdataCheckPoint::checkArea() const {
    return m_checkArea;
}

bool MapdataCheckPoint::isNormalCheckpoint() const {
    return static_cast<MapdataCheckPoint::CheckArea>(m_checkArea) ==
            MapdataCheckPoint::CheckArea::NORMAL_CHECKPOINT;
}

bool MapdataCheckPoint::isFinishLine() const {
    return static_cast<MapdataCheckPoint::CheckArea>(m_checkArea) ==
            MapdataCheckPoint::CheckArea::FINISH_LINE;
}

u16 MapdataCheckPoint::nextCount() const {
    return m_nextCount;
}

u16 MapdataCheckPoint::prevCount() const {
    return m_prevCount;
}

u16 MapdataCheckPoint::id() const {
    return m_id;
}

MapdataCheckPoint *MapdataCheckPoint::prevPoint(s32 i) const {
    return m_prevPoints[i];
}

MapdataCheckPoint *MapdataCheckPoint::nextPoint(size_t i) const {
    ASSERT(i < m_nextPoints.size());
    return m_nextPoints[i].checkpoint;
}

/// @addr{Inlined in 0x80510C74}
/// @brief Returns true if player is between the two sides of the checkpoint
/// quad, otherwise false
bool MapdataCheckPoint::checkSector(const LinkedCheckpoint &next, const EGG::Vector2f &p0,
        const EGG::Vector2f &p1) const {
    if (-(next.p0diff.y) * p0.x + next.p0diff.x * p0.y < 0.0f) {
        return false;
    }

    if (next.p1diff.y * p1.x - next.p1diff.x * p1.y < 0.0f) {
        return false;
    }

    return true;
}

/// @addr{Inlined in 0x80510C74}
/// @brief Updates @param checkpointCompletion\, which is the percentage of
/// distance the player has traveled through the checkpoint quad
/// @return True if 0 <= checkpointCompletion <= 1, meaning the player is
/// between the current checkpoint line and the next; otherwise, false
bool MapdataCheckPoint::checkCheckpointCompletion(const LinkedCheckpoint &next,
        const EGG::Vector2f &p0, const EGG::Vector2f &p1, float *checkpointCompletion) const {
    f32 d1 = m_dir.dot(p1);
    f32 d2 = -(next.checkpoint->m_dir.dot(p0));
    f32 checkpointCompletion_ = d1 / (d1 + d2);
    *checkpointCompletion = checkpointCompletion_;
    return (checkpointCompletion_ >= 0.0f && checkpointCompletion_ <= 1.0f);
}

/// @addr{0x80510C74}
/// @brief Calls both @ref checkSector and @ref checkCheckpointCompletion;
/// updates @param checkpointCompletion
/// @rename consider mk7 symbol `4adb64
/// Field::MapdataCheckPoint::checkSectorAndDistanceRatio(float*, const sead::Vector3<float>&)
/// const`, which seems undescriptive ("DistanceRatio"). also, this should probably not share a name
/// with @ref MapdataCheckPoint::checkSectorAndCheckpointCompletion
MapdataCheckPoint::SectorOccupancy MapdataCheckPoint::checkSectorAndCheckpointCompletion_(
        const LinkedCheckpoint &next, const EGG::Vector2f &p0, const EGG::Vector2f &p1,
        float *checkpointCompletion) const {
    if (!checkSector(next, p0, p1)) {
        return SectorOccupancy::OutsideSector;
    }

    return checkCheckpointCompletion(next, p0, p1, checkpointCompletion) ?
            SectorOccupancy::InsideSector :
            SectorOccupancy::OutsideSector_BetweenSides;
}

/// @addr{0x80515244}
/// @brief Initializes all checkpoint links, and finds the finish line and last
/// key checkpoint.
void MapdataCheckPointAccessor::init() {
    s8 lastKcpType = -1;
    s16 finishLineCheckpointId = -1;

    for (size_t ckptId = 0; ckptId < size(); ckptId++) {
        MapdataCheckPoint *checkpoint = get(ckptId);
        s8 checkArea = checkpoint->checkArea();
        checkpoint->initCheckpointLinks(*this, ckptId);

        if (checkpoint->isFinishLine()) {
            finishLineCheckpointId = ckptId;
        }

        lastKcpType = std::max(lastKcpType, checkArea);
    }

    m_lastKcpType = lastKcpType;
    m_finishLineCheckpointId = finishLineCheckpointId;
}

MapdataCheckPointAccessor::MapdataCheckPointAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataCheckPoint, MapdataCheckPoint::SData>(header) {
    MapdataAccessorBase::init(
            reinterpret_cast<const MapdataCheckPoint::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));
    init();
}

MapdataCheckPointAccessor::~MapdataCheckPointAccessor() = default;

} // namespace System
