#include "MapdataCheckPoint.hh"

#include "game/system/CourseMap.hh"
#include "game/system/map/MapdataCheckPath.hh"

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

/// @brief Calculates @ref m_nextPoints and @ref m_prevPoints from @ref m_nextPt and @ref m_prevPt.
/// @details Also calculates the quadrilaterals for the next checkpoints, filling the fields of @ref
/// LinkedCheckpoint for each.
/// @addr{0x80515624}
void MapdataCheckPoint::initCheckpointLinks(MapdataCheckPointAccessor &accessor, int id) {
    m_id = id;
    const auto *checkPathAccessor = CourseMap::Instance()->checkPath();

    // Calculate the quadrilateral's `m_prevPoints`. If the check point is the first in its group,
    // it has multiple previous checkpoints defined by its preceding checkpaths
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

    // Calculate the quadrilateral's `m_nextPoints`. If the checkpoint is the last in its group, it
    // can have multiple quadrilaterals (and nextCheckpoint) which are determined by its next
    // path(s)
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

s8 MapdataCheckPoint::checkArea() const {
    return m_checkArea;
}

MapdataCheckPoint *MapdataCheckPoint::nextPoint(size_t i) const {
    ASSERT(i < m_nextPoints.size());
    return m_nextPoints[i].checkpoint;
}

/// @addr{0x80515244}
/// @brief Initializes all checkpoint links, and finds the finish line and last key checkpoint.
void MapdataCheckPointAccessor::init() {
    s8 lastKcpType = -1;
    s16 finishLineCheckpointId = -1;

    for (size_t ckptId = 0; ckptId < size(); ckptId++) {
        MapdataCheckPoint *checkpoint = get(ckptId);
        s8 checkArea = checkpoint->checkArea();
        checkpoint->initCheckpointLinks(*this, ckptId);

        if (static_cast<MapdataCheckPoint::CheckArea>(checkArea) ==
                MapdataCheckPoint::CheckArea::FINISH_LINE) {
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
