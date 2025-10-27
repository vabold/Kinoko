#include "KartPullPath.hh"

#include "game/system/CourseMap.hh"

#include <egg/geom/Plane.hh>

namespace Kart {

/// @addr{0x8059308C}
KartPullPathTracker::KartPullPathTracker(KartPullPath *handle, Type type)
    : m_type(type), m_currentIdx(0), m_pointInfo(nullptr), m_handle(handle) {}

/// @addr{0x805930E4}
KartPullPathTracker::~KartPullPathTracker() = default;

void KartPullPathTracker::calc() {
    ASSERT(m_pointInfo);

    if (m_type == Type::Global) {
        calcTrackerGlobal();
    } else if (m_type == Type::Regional) {
        calcTrackerRegional();
    }
}

/// @addr{0x80593138}
void KartPullPathTracker::calcTrackerGlobal() {
    s16 idx;
    EGG::Vector3f point, dir;
    if (search(SearchDirection::Current, idx, point, dir)) {
        m_handle->changePoint(idx, getDistance(point, dir));
    }

    if (static_cast<size_t>(++m_currentIdx) >= m_pointInfo->pointCount() - 1) {
        m_currentIdx = 0;
    }
}

/// @addr{0x80593814}
void KartPullPathTracker::calcTrackerRegional() {
    if (m_handle->incomingIdx() < 0) {
        return;
    }

    s16 idx;
    EGG::Vector3f point, dir;

    if (search(SearchDirection::Current, idx, point, dir)) {
        m_handle->changePoint(idx, getDistance(point, dir));
        return;
    }

    m_handle->resetDistance();
    if (search(SearchDirection::Next, idx, point, dir)) {
        m_handle->changePoint(idx, getDistance(point, dir));
        return;
    }

    if (search(SearchDirection::Previous, idx, point, dir)) {
        m_handle->changePoint(idx, getDistance(point, dir));
    }
}

/// @addr{0x80593310}
/// @brief Gets the distance from the line formed by the point and direction.
/// @param point A point on the line.
/// @param dir The direction of the line.
f32 KartPullPathTracker::getDistance(const EGG::Vector3f &point, const EGG::Vector3f &dir) const {
    EGG::Vector3f diff = pos() - point;
    f32 dist = diff.length();
    f32 x = EGG::Mathf::abs(diff.dot(dir));
    return EGG::Mathf::sqrt(dist * dist - x * x);
}

/// @addr{0x8059345C}
bool KartPullPathTracker::search(SearchDirection searchDirection, s16 &idx, EGG::Vector3f &point,
        EGG::Vector3f &dir) const {
    ASSERT(m_pointInfo);

    s16 p, c, n;
    switch (searchDirection) {
    case SearchDirection::Current:
        idx = m_currentIdx;
        p = -1;
        c = 0;
        n = 1;
        break;
    case SearchDirection::Next:
        idx = m_currentIdx + 1;
        p = 0;
        c = 1;
        n = 2;
        break;
    case SearchDirection::Previous:
        idx = m_currentIdx - 1;
        p = -2;
        c = -1;
        n = 0;
        break;
    default:
        PANIC("Invalid search direction!");
        break;
    }

    const auto &points = m_pointInfo->points();

    // There always needs to be a current point and a next point to get the direction
    if (m_currentIdx + c < 0 || static_cast<size_t>(m_currentIdx + c) >= points.size()) {
        return false;
    }

    if (m_currentIdx + n < 0 || static_cast<size_t>(m_currentIdx + n) >= points.size()) {
        return false;
    }

    const auto &currentPos = points[m_currentIdx + c].pos;
    const auto &nextPos = points[m_currentIdx + n].pos;

    EGG::Vector3f back = currentPos - nextPos;
    back.normalise();
    EGG::Vector3f front = -back;

    if (m_currentIdx + p >= 0 && static_cast<size_t>(m_currentIdx + p) < points.size()) {
        const auto &prevPos = points[m_currentIdx + p].pos;
        back = prevPos - currentPos;
        back.normalise();
    }

    EGG::Plane3f backPlane = EGG::Plane3f(currentPos, back);
    EGG::Plane3f frontPlane = EGG::Plane3f(nextPos, front);

    if (backPlane.testPoint(pos()) && frontPlane.testPoint(pos())) {
        point = currentPos;
        dir = front;
        return true;
    }

    return false;
}

/// @addr{0x80593FA4}
KartPullPath::KartPullPath()
    : m_globalTracker(this, KartPullPathTracker::Type::Global),
      m_regionalTracker(this, KartPullPathTracker::Type::Regional) {
    init();
}

/// @addr{0x80594094}
KartPullPath::~KartPullPath() = default;

/// @addr{0x805940D4}
void KartPullPath::init() {
    reset();
    m_areaId = -1;
    m_roadSpeedDecay = 1.0f;
}

/// @addr{0x80593CB8}
/// @details This was the init function in the base class, but it gets inlined in calcArea.
void KartPullPath::reset() {
    m_distance = -1.0f;
    m_currentIdx = -1;
    m_incomingIdx = -1;
    m_pointInfo = nullptr;
    m_pullDirection = EGG::Vector3f::zero;
    m_pullSpeed = 0.0f;
    m_maxPullSpeed = 0.0f;
}

/// @addr{0x80594134}
void KartPullPath::calc() {
    if (!calcArea()) {
        return;
    }

    ASSERT(m_pointInfo);
    if (m_pointInfo->pointCount() > 2) {
        calcTrackers();
    }
}

/// @addr{0x80593DBC}
void KartPullPath::changePoint(s16 idx, f32 distance) {
    if ((m_distance >= 0.0f || distance >= 3000.0f) && distance >= m_distance) {
        return;
    }

    m_incomingIdx = idx;
    m_distance = distance;
    m_regionalTracker.setCurrentIdx(idx);
    calcPointChange();
}

/// @addr{0x805941BC}
bool KartPullPath::calcArea() {
    auto *courseMap = System::CourseMap::Instance();

    s16 prevAreaId = m_areaId;
    m_areaId = System::CourseMap::Instance()->getCurrentAreaID(m_areaId, pos(),
            System::MapdataAreaBase::Type::MovingRoad);

    if (m_areaId >= 0) {
        if (prevAreaId < 0 || prevAreaId != m_areaId) {
            reset();
            auto *area = courseMap->getArea(m_areaId);
            ASSERT(area);
            auto *pointInfo = area->getPointInfo();
            ASSERT(pointInfo);
            m_pointInfo = pointInfo;
            if (pointInfo->pointCount() > 2) {
                setTrackerPointInfo(pointInfo);
            } else {
                m_incomingIdx = 0;
                calcPointChange();
            }
            m_roadSpeedDecay = 0.9f + 0.001f * static_cast<f32>(area->param(0));
            m_maxPullSpeed = static_cast<f32>(area->param(1));
        }
    } else {
        m_areaId = -1;
        m_currentIdx = -1;
        m_incomingIdx = -1;
        m_pullDirection = EGG::Vector3f::zero;
    }

    return m_areaId >= 0;
}

/// @addr{0x80593E18}
void KartPullPath::calcPointChange() {
    if (m_currentIdx == m_incomingIdx) {
        return;
    }

    const auto &points = m_pointInfo->points();

    const auto &currentPos = points[m_incomingIdx].pos;
    const auto &nextPos = points[m_incomingIdx + 1].pos;
    m_pullSpeed = static_cast<f32>(points[m_incomingIdx].setting[0]);
    u16 pullInfluence = points[m_incomingIdx + 1].setting[1];

    m_pullDirection = nextPos - currentPos;
    m_pullDirection.normalise();

    // If the pull influence is 0, the pull direction moves forward, so do nothing
    // If the pull influence is 1, the pull direction moves left
    if (pullInfluence == 1) {
        m_pullDirection = getPullUnitNormal() * -1.0f;
    }

    // If the pull influence is 2, the pull direction moves right
    else if (pullInfluence == 2) {
        m_pullDirection = getPullUnitNormal();
    }

    m_currentIdx = m_incomingIdx;
}

/// @addr{0x80593D54}
void KartPullPath::calcTrackers() {
    m_globalTracker.calc();
    m_regionalTracker.calc();
}

/// @addr{0x805AEAD8}
/// @details This isn't a part of KartPullPath, but is only called from this class.
EGG::Vector3f KartPullPath::getPullUnitNormal() const {
    // Dot product of two unit vectors being 1.0f => angle between them is 0
    // They're the same vector, just return zero
    if (EGG::Vector3f::ey.dot(m_pullDirection) == 1.0f) {
        return EGG::Vector3f::zero;
    }

    EGG::Vector3f nrm = EGG::Vector3f::ey.cross(m_pullDirection);
    nrm.normalise();
    return nrm;
}

/// @addr{0x80593D1C}
void KartPullPath::setTrackerPointInfo(System::MapdataPointInfo *info) {
    m_globalTracker.setPointInfo(info);
    m_regionalTracker.setPointInfo(info);
}

} // namespace Kart
