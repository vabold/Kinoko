#include "Rail.hh"

#include "game/field/CollisionDirector.hh"

namespace Field {

/// @addr{0x806EC9A4}
Rail::Rail(u16 idx, System::MapdataPointInfo *info) {
    m_idx = idx;
    m_pointCapacity = info->pointCount();
    m_pointCount = info->pointCount();
    m_isOscillating = info->setting(1) == 1;
    m_points = info->points();
    m_hasCheckedCol = false;
    m_someScale = 0.1f;
}

/// @addr{0x806ECC40}
/// TODO: If we make m_points allocate on the heap, then we need to free here
Rail::~Rail() {
    delete m_floorNrms.data();
}

/// @addr{0x806ED110}
void Rail::addPoint(f32 scale, const EGG::Vector3f &point) {
    m_points.back().pos = point;
    m_someScale = scale;
    onPointAdded();
}

/// @addr{0x806ECCC0}
void Rail::checkSphereFull() {
    if (m_hasCheckedCol) {
        return;
    }

    m_floorNrms = std::span<EGG::Vector3f>(new EGG::Vector3f[m_pointCount], m_pointCount);

    for (size_t i = 0; i < m_pointCount; ++i) {
        CollisionInfo info;
        info.bbox.setZero();

        bool hasCourseCol = CollisionDirector::Instance()->checkSphereFull(100.0f, m_points[i].pos,
                EGG::Vector3f::inf, KCL_TYPE_FLOOR, &info, nullptr, 0);

        if (hasCourseCol) {
            if (info.floorDist > std::numeric_limits<f32>::min()) {
                m_floorNrms[i] = info.floorNrm;
            }
        } else {
            m_floorNrms[i] = EGG::Vector3f::ey;
        }
    }

    m_hasCheckedCol = true;
}

/// @addr{0x806EF9B4}
RailLine::RailLine(u16 idx, System::MapdataPointInfo *info) : Rail(idx, info) {
    m_dirCount = m_isOscillating ? m_pointCount - 1 : m_pointCount;
    m_transitions = std::span<RailLineTransition>(new RailLineTransition[m_dirCount], m_dirCount);
    m_pathLength = 0.0f;

    for (u16 i = 0; i < m_pointCount - 1; ++i) {
        auto &transition = m_transitions[i];
        transition.m_dir = m_points[i + 1].pos - m_points[i].pos;
        transition.m_length = transition.m_dir.normalise();
        transition.m_lengthInv = 1.0f / transition.m_length;
        m_pathLength += transition.m_length;
    }

    if (!m_isOscillating) {
        auto &transition = m_transitions.back();
        transition.m_dir = m_points.front().pos - m_points.back().pos;
        transition.m_length = transition.m_dir.normalise();
        transition.m_lengthInv = 1.0f / transition.m_length;
        m_pathLength += transition.m_length;
    }
}

/// @addr{0x806EFD6C}
RailLine::~RailLine() {
    delete m_transitions.data();
}

/// @addr{0x806ED57C}
RailSpline::RailSpline(u16 idx, System::MapdataPointInfo *info) : Rail(idx, info) {
    m_transitionCount = m_isOscillating ? m_pointCount - 1 : m_pointCount;
    m_transitions = std::span<RailSplineTransition>(new RailSplineTransition[m_transitionCount],
            m_transitionCount);
    m_estimatorSampleCount = 10;
    m_estimatorStep = 1.0f / static_cast<f32>(m_estimatorSampleCount);

    // This is normally not set until AFTER the call to invalidateTransitions,
    // but the expected behavior requires that this is set to false.
    // This misordering was probably never noticed since EGG::Heap zeroes memory.
    m_doNotAllocatePathPercentages = false;

    invalidateTransitions(false);

    m_pathLength = 0.0f;

    for (size_t i = 0; i < m_transitionCount; ++i) {
        m_pathLength += m_transitions[i].m_length;
    }
}

/// @addr{0x806ED828}
RailSpline::~RailSpline() {
    delete m_transitions.data();
    delete m_pathPercentages.data();
}

/// @addr{0x806ED8BC}
void RailSpline::onPointsChanged() {
    m_doNotAllocatePathPercentages = true;
    m_transitionCount = m_isOscillating ? m_pointCount - 1 : m_pointCount;

    invalidateTransitions(false);

    m_pathLength = 0.0f;

    for (auto &transition : m_transitions) {
        m_pathLength += transition.m_length;
    }
}

/// @addr{0x806ED960}
void RailSpline::onPointAdded() {
    m_doNotAllocatePathPercentages = true;
    m_transitionCount = m_isOscillating ? m_pointCount - 1 : m_pointCount;

    invalidateTransitions(true);

    m_pathLength = 0.0f;

    for (auto &transition : m_transitions) {
        m_pathLength += transition.m_length;
    }
}

/// @addr{0x806EDA04}
void RailSpline::invalidateTransitions(bool lastOnly) {
    if (!m_doNotAllocatePathPercentages) {
        size_t count = m_estimatorSampleCount * m_transitionCount + 1;
        m_pathPercentages = std::span<f32>(new f32[count], count);
    }

    m_segmentCount = 0;

    if (m_isOscillating) {
        if (!lastOnly) {
            auto &firstTransition = m_transitions[0];
            firstTransition.m_p0 = m_points[0].pos;
            firstTransition.m_p1 =
                    (m_points[1].pos - m_points[0].pos).multInv(4.0f) + m_points[0].pos;
            firstTransition.m_p2 =
                    calcCubicBezierP2(m_points[0].pos, m_points[1].pos, m_points[2].pos);
            firstTransition.m_p3 = m_points[1].pos;
            firstTransition.m_length = estimateLength(firstTransition, m_estimatorSampleCount);
            firstTransition.m_lengthInv = 1.0f / firstTransition.m_length;

            for (u16 i = 1; i < m_transitionCount - 1; ++i) {
                calcCubicBezierControlPoints(m_points[i - 1].pos, m_points[i].pos,
                        m_points[i + 1].pos, m_points[i + 2].pos, m_estimatorSampleCount,
                        m_transitions[i]);
            }
        }

        auto &lastTransition = m_transitions.back();
        lastTransition.m_p0 = m_points[m_transitionCount - 1].pos;
        lastTransition.m_p1 = calcCubicBezierP1(m_points[m_transitionCount - 2].pos,
                m_points[m_transitionCount - 1].pos, m_points[m_transitionCount].pos);
        lastTransition.m_p2 =
                (m_points[m_transitionCount - 1].pos - m_points[m_transitionCount].pos)
                        .multInv(4.0f) +
                m_points[m_transitionCount].pos;
        lastTransition.m_p3 = m_points[m_transitionCount].pos;
        lastTransition.m_length = estimateLength(lastTransition, m_estimatorSampleCount);
        lastTransition.m_lengthInv = 1.0f / lastTransition.m_length;
    } else {
        if (!lastOnly) {
            auto &firstTransition = m_transitions[0];
            firstTransition.m_p0 = m_points[0].pos;
            firstTransition.m_p1 = calcCubicBezierP1(m_points[m_transitionCount - 1].pos,
                    m_points[0].pos, m_points[1].pos);
            firstTransition.m_p2 =
                    calcCubicBezierP2(m_points[0].pos, m_points[1].pos, m_points[2].pos);
            firstTransition.m_p3 = m_points[1].pos;
            firstTransition.m_length = estimateLength(firstTransition, m_estimatorSampleCount);
            firstTransition.m_lengthInv = 1.0f / firstTransition.m_length;

            for (u16 i = 1; i < m_transitionCount - 1; ++i) {
                if (i + 2 != m_transitionCount) {
                    calcCubicBezierControlPoints(m_points[i - 1].pos, m_points[i].pos,
                            m_points[i + 1].pos, m_points[i + 2].pos, m_estimatorSampleCount,
                            m_transitions[i]);
                } else {
                    calcCubicBezierControlPoints(m_points[i - 1].pos, m_points[i].pos,
                            m_points[i + 1].pos, m_points[0].pos, m_estimatorSampleCount,
                            m_transitions[i]);
                }
            }
        }

        auto &lastTransition = m_transitions.back();
        lastTransition.m_p0 = m_points[m_transitionCount - 1].pos;
        lastTransition.m_p1 = calcCubicBezierP1(m_points[m_transitionCount - 2].pos,
                m_points[m_transitionCount - 1].pos, m_points[0].pos);
        lastTransition.m_p2 = calcCubicBezierP2(m_points[m_transitionCount - 1].pos,
                m_points[0].pos, m_points[1].pos);
        lastTransition.m_p3 = m_points[0].pos;
        lastTransition.m_length = estimateLength(lastTransition, m_estimatorSampleCount);
        lastTransition.m_lengthInv = 1.0f / lastTransition.m_length;
    }
}

/// @addr{0x806EE27C}
void RailSpline::calcCubicBezierControlPoints(const EGG::Vector3f &p0, const EGG::Vector3f &p1,
        const EGG::Vector3f &p2, const EGG::Vector3f &p3, s32 count,
        RailSplineTransition &transition) {
    transition.m_p0 = p1;
    transition.m_p1 = calcCubicBezierP1(p0, p1, p2);
    transition.m_p2 = calcCubicBezierP2(p1, p2, p3);
    transition.m_p3 = p2;
    transition.m_length = estimateLength(transition, count);
    transition.m_lengthInv = 1.0f / transition.m_length;
}

/// @addr{0x806EE56C}
f32 RailSpline::estimateLength(const RailSplineTransition &transition, s32 count) {
    std::array<EGG::Vector3f, 11> waypoints;

    for (s32 i = 0; i < count + 1; ++i) {
        waypoints[i] = cubicBezier(m_estimatorStep * static_cast<f32>(i), transition);
    }
    f32 length = 0.0f;

    // Numerator loop
    for (s32 i = 0; i < count; ++i) {
        m_pathPercentages[m_segmentCount++] = length;
        length += (waypoints[i] - waypoints[i + 1]).length();
    }

    // Denominator loop
    for (s32 i = m_segmentCount - 1; i > m_segmentCount - count - 1; --i) {
        m_pathPercentages[i] /= length;
    }

    return length;
}

/// @addr{0x806EE408}
EGG::Vector3f RailSpline::calcCubicBezierP1(const EGG::Vector3f &p0, const EGG::Vector3f &p1,
        const EGG::Vector3f &p2) const {
    EGG::Vector3f res = p2 - p0;
    f32 len = res.length();
    res.normalise2();
    return p1 + res * (len * m_someScale);
}

/// @addr{0x806EE4B8}
EGG::Vector3f RailSpline::calcCubicBezierP2(const EGG::Vector3f &p0, const EGG::Vector3f &p1,
        const EGG::Vector3f &p2) const {
    EGG::Vector3f res = p0 - p2;
    f32 len = res.length();
    res.normalise2();
    return p1 + res * (len * m_someScale);
}

/// @addr{0x806EE72C}
EGG::Vector3f RailSpline::cubicBezier(f32 t, const RailSplineTransition &transition) const {
    f32 dt = 1.0f - t;

    EGG::Vector3f res = transition.m_p0 * (dt * dt * dt);
    res += transition.m_p1 * (3.0f * t * (dt * dt));
    res += transition.m_p2 * (3.0f * (t * t) * dt);
    res += transition.m_p3 * (t * t * t);

    return res;
}

} // namespace Field
