#include "RailInterpolator.hh"

#include "game/field/RailManager.hh"

namespace Field {

/// @addr{0x806ED160}
RailInterpolator::RailInterpolator(f32 speed, u32 idx) : m_currVel(0.0f) {
    m_railIdx = idx;
    auto *rail = RailManager::Instance()->rail(idx);
    m_pointCount = rail->pointCount();
    m_points = rail->points();
    m_isOscillating = rail->isOscillating();
    m_speed = speed;
}

/// @addr{0x806ED53C}
RailInterpolator::~RailInterpolator() = default;

/// @addr{0806ED24C}
const EGG::Vector3f &RailInterpolator::floorNrm(size_t idx) const {
    return RailManager::Instance()->rail(m_railIdx)->floorNrm(idx);
}

/// @addr{0x806ED30C}
f32 RailInterpolator::railLength() const {
    return RailManager::Instance()->rail(m_railIdx)->getPathLength();
}

/// @addr{0x806ED3E4}
void RailInterpolator::updateVel() {
    f32 t = m_segmentT;
    setCurrVel((1.0f - t) * m_prevPointVel + t * m_nextPointVel);
}

/// @addr{0x806ED34C}
void RailInterpolator::calcVelocities() {
    m_prevPointVel = static_cast<f32>(m_points[m_currPointIdx].setting[0]);
    m_nextPointVel = static_cast<f32>(m_points[m_nextPointIdx].setting[0]);

    if (m_prevPointVel == 0.0f) {
        m_prevPointVel = m_speed;
    }

    if (m_nextPointVel == 0.0f) {
        m_nextPointVel = m_speed;
    }
}

/// @addr{0x806F0814}
bool RailInterpolator::shouldChangeDirection() const {
    if (!m_isOscillating) {
        return m_pointCount == m_nextPointIdx;
    }

    return m_movementDirectionForward ? m_nextPointIdx == m_pointCount : m_nextPointIdx == -1;
}

/// @addr{0x806F0880}
void RailInterpolator::calcDirectionChange() {
    if (!m_isOscillating) {
        return;
    }

    if (m_movementDirectionForward) {
        m_nextPointIdx -= 2;
    } else {
        m_nextPointIdx += 2;
    }

    m_movementDirectionForward = !m_movementDirectionForward;
}

void RailInterpolator::calcNextIndices() {
    if (m_movementDirectionForward) {
        ++m_currPointIdx;
        ++m_nextPointIdx;
    } else {
        --m_currPointIdx;
        --m_nextPointIdx;
    }

    if (!m_isOscillating) {
        if (m_nextPointIdx == m_pointCount) {
            m_nextPointIdx = 0;
        }

        if (m_currPointIdx == m_pointCount) {
            m_currPointIdx = 0;
        }
    }
}

/// @addr{0x806EFDC4}
RailLinearInterpolator::RailLinearInterpolator(f32 speed, u32 idx) : RailInterpolator(speed, idx) {
    m_transitions = RailManager::Instance()->rail(m_railIdx)->getLinearTransitions();
    init(0.0f, 0);
}

/// @addr{0x806F094C}
RailLinearInterpolator::~RailLinearInterpolator() = default;

/// @addr{0x806EFEAC}
void RailLinearInterpolator::init(f32 t, u32 idx) {
    m_segmentT = t;
    m_currPointIdx = idx;

    bool isLastPoint = (static_cast<u16>(idx) == m_pointCount - 1);
    m_nextPointIdx = isLastPoint ? idx - 1 : idx + 1;
    m_movementDirectionForward = isLastPoint ? !m_isOscillating : true;

    m_curPos = m_points[m_currPointIdx].pos;
    m_currentDirection = m_points[m_nextPointIdx].pos - m_curPos;
    m_curTangentDir = m_currentDirection;
    m_curTangentDir.normalise2();
    m_currVel = m_speed;
    m_prevPointVel = m_speed;
    m_nextPointVel = m_speed;
    m_4a = false;
    m_usePerPointVelocities = false;
    m_currSegmentVel = m_speed / m_currentDirection.length();
}

/// @addr{0x806F0050}
RailInterpolator::Status RailLinearInterpolator::calc() {
    if (m_4a) {
        m_curPos = m_points[m_pointCount - 1].pos;

        return Status::ChangingDirection;
    }

    if (m_usePerPointVelocities) {
        updateVel();
    }

    m_segmentT += m_currSegmentVel;
    m_curPos = lerp(m_segmentT, m_currPointIdx, m_nextPointIdx);

    if (m_segmentT <= 1.0f) {
        return Status::InProgress;
    }

    Status status = Status::SegmentEnd;

    calcNextSegment();

    if (shouldChangeDirection()) {
        status = Status::ChangingDirection;

        calcDirectionChange();
    }

    m_currentDirection = m_points[m_nextPointIdx].pos - m_points[m_currPointIdx].pos;
    m_curTangentDir = m_currentDirection;
    m_currSegmentVel = m_currVel / m_currentDirection.length();
    m_curTangentDir.normalise2();

    return status;
}

/// @addr{0x806EFFF4}
void RailLinearInterpolator::setCurrVel(f32 speed) {
    m_currVel = speed;
    m_currSegmentVel = m_currVel / m_currentDirection.length();
}

/// @addr{0x806F02EC}
void RailLinearInterpolator::evalCubicBezierOnPath(f32 t, EGG::Vector3f &currDir,
        EGG::Vector3f &curTangentDir) {
    s16 currIdx = 0;
    f32 len = 0.0f;

    getPathLocation(t, currIdx, len);

    s16 nextIdx = currIdx + 1;
    if (nextIdx == m_pointCount) {
        nextIdx = 0;
    }

    currDir = m_points[currIdx].pos * (1.0f - len) + m_points[nextIdx].pos * len;
    curTangentDir = m_transitions[currIdx].m_dir;
}

/// @addr{0x806F041C}
void RailLinearInterpolator::getPathLocation(f32 t, s16 &idx, f32 &len) {
    if (!m_movementDirectionForward) {
        return;
    }

    f32 dist = m_segmentT * m_transitions[m_currPointIdx].m_length;
    if (t <= dist) {
        idx = m_currPointIdx;
        len = (dist - t) * m_transitions[m_currPointIdx].m_lengthInv;
        return;
    }

    for (s32 i = 0; i < m_pointCount; ++i) {
        s32 currIdx = m_currPointIdx - 1;
        if (currIdx == -1) {
            currIdx = m_pointCount - 1;
        }

        currIdx -= i;
        if (currIdx < 0) {
            currIdx += m_pointCount;
        }

        dist += m_transitions[currIdx].m_length;
        if (t <= dist) {
            idx = currIdx;
            len = (dist - t) * m_transitions[currIdx].m_lengthInv;
            return;
        }
    }
}

/// @addr{0x806F0610}
void RailLinearInterpolator::calcNextSegment() {
    calcNextIndices();

    f32 prevDirLength = m_currentDirection.length();
    m_currentDirection = m_points[m_nextPointIdx].pos - m_points[m_currPointIdx].pos;
    m_segmentT = ((m_segmentT - 1.0f) * prevDirLength) / m_currentDirection.length();

    if (shouldChangeDirection()) {
        m_segmentT = 0.0f;
    }

    if (m_segmentT > 1.0f) {
        m_segmentT = 0.99f;
    }

    if (m_usePerPointVelocities) {
        calcVelocities();
    }
}

/// @addr{0x806F0540}
EGG::Vector3f RailLinearInterpolator::lerp(f32 t, u32 currIdx, u32 nextIdx) const {
    return m_points[currIdx].pos * (1.0f - t) + m_points[nextIdx].pos * t;
}

/// @addr{0x806EE830}
RailSmoothInterpolator::RailSmoothInterpolator(f32 speed, u32 idx) : RailInterpolator(speed, idx) {
    auto *rail = RailManager::Instance()->rail(m_railIdx);
    m_transitions = rail->getSplineTransitions();
    m_estimatorSampleCount = static_cast<u32>(rail->getEstimatorSampleCount());
    m_estimatorStep = rail->getEstimatorStep();
    m_pathPercentages = rail->getPathPercentages();

    init(0.0f, 0);
}

/// @addr{0x806EF944}
RailSmoothInterpolator::~RailSmoothInterpolator() = default;

/// @addr{0x806EE924}
void RailSmoothInterpolator::init(f32 t, u32 idx) {
    m_segmentT = t;

    m_currPointIdx = idx;

    if (idx == static_cast<u32>(m_pointCount - 1) && m_isOscillating) {
        m_nextPointIdx = m_currPointIdx - 1;
        m_movementDirectionForward = false;
        m_curTangentDir = calcCubicBezierTangentDir(m_segmentT, m_transitions[m_currPointIdx]);
        m_currSegmentVel = m_currVel * m_transitions[m_nextPointIdx].m_lengthInv;
    } else {
        m_nextPointIdx = idx + 1 == m_pointCount ? 0 : idx + 1;
        m_movementDirectionForward = true;
        m_curTangentDir = calcCubicBezierTangentDir(m_segmentT, m_transitions[m_currPointIdx]);
        m_currSegmentVel = m_currVel * m_transitions[m_currPointIdx].m_lengthInv;
    }

    m_curPos = m_points[m_currPointIdx].pos;
    m_prevPos = m_points[m_currPointIdx].pos;
    m_currVel = m_speed;
    m_prevPointVel = m_speed;
    m_nextPointVel = m_speed;
    m_velocity = 0.0f;
    m_4a = false;
    m_usePerPointVelocities = false;
}

/// @addr{0x806EEBEC}
RailInterpolator::Status RailSmoothInterpolator::calc() {
    if (m_4a) {
        m_curPos = m_transitions[m_pointCount - 2].m_p3;

        return Status::ChangingDirection;
    }

    if (m_usePerPointVelocities) {
        updateVel();
    }

    m_prevPos = m_curPos;

    f32 t = m_movementDirectionForward ? calcT(m_segmentT) : calcT(1.0f - m_segmentT);

    calcCubicBezier(t, m_currPointIdx, m_nextPointIdx, m_curPos, m_curTangentDir);

    EGG::Vector3f deltaPos = m_curPos - m_prevPos;
    m_velocity = deltaPos.length();
    m_segmentT += m_currSegmentVel;

    if (m_segmentT <= 1.0f) {
        return Status::InProgress;
    }

    Status status = Status::SegmentEnd;

    calcNextSegment();

    if (shouldChangeDirection()) {
        status = Status::ChangingDirection;

        calcDirectionChange();
    }

    if (m_movementDirectionForward) {
        m_currSegmentVel = m_currVel * m_transitions[m_currPointIdx].m_lengthInv;
    } else {
        m_currSegmentVel = m_currVel * m_transitions[m_nextPointIdx].m_lengthInv;
    }

    return status;
}

/// @addr{0x806EEB94}
void RailSmoothInterpolator::setCurrVel(f32 speed) {
    m_currVel = speed;

    if (m_movementDirectionForward) {
        m_currSegmentVel = speed * m_transitions[m_currPointIdx].m_lengthInv;
    } else {
        m_currSegmentVel = speed * m_transitions[m_nextPointIdx].m_lengthInv;
    }
}

/// @addr{0x806EEEBC}
void RailSmoothInterpolator::evalCubicBezierOnPath(f32 t, EGG::Vector3f &currDir,
        EGG::Vector3f &curTangentDir) {
    s16 currIdx = 0;
    f32 len = 0.0f;

    getPathLocation(t, currIdx, len);

    s16 nextIdx = currIdx + 1;
    if (nextIdx == m_pointCount) {
        nextIdx = 0;
    }

    len = m_movementDirectionForward ? calcT(len) : calcT(1.0f - len);

    calcCubicBezier(len, currIdx, nextIdx, currDir, curTangentDir);
}

/// @addr{0x806EEFA0}
void RailSmoothInterpolator::getPathLocation(f32 t, s16 &idx, f32 &len) {
    if (!m_movementDirectionForward) {
        return;
    }

    f32 dist = m_segmentT * m_transitions[m_currPointIdx].m_length;
    if (t <= dist) {
        idx = m_currPointIdx;
        len = (dist - t) * m_transitions[m_currPointIdx].m_lengthInv;
        return;
    }

    for (s32 i = 0; i < m_pointCount; ++i) {
        s32 currIdx = m_currPointIdx - 1;
        if (currIdx == -1) {
            currIdx = m_pointCount - 1;
        }

        currIdx -= i;
        if (currIdx < 0) {
            currIdx += m_pointCount;
        }

        dist += m_transitions[currIdx].m_length;
        if (t <= dist) {
            idx = currIdx;
            len = (dist - t) * m_transitions[currIdx].m_lengthInv;
            return;
        }
    }
}

/// @addr{0x806EF224}
void RailSmoothInterpolator::calcCubicBezier(f32 t, u32 currIdx, u32 nextIdx, EGG::Vector3f &pos,
        EGG::Vector3f &dir) const {
    auto &transition = m_movementDirectionForward ? m_transitions[currIdx] : m_transitions[nextIdx];

    pos = calcCubicBezierPos(t, transition);
    dir = calcCubicBezierTangentDir(t, transition);
}

/// @addr{0x806EF350}
EGG::Vector3f RailSmoothInterpolator::calcCubicBezierPos(f32 t,
        const RailSplineTransition &trans) const {
    f32 dt = 1.0f - t;

    EGG::Vector3f res = trans.m_p0 * (dt * dt * dt);
    res += trans.m_p1 * (3.0f * t * (dt * dt));
    res += trans.m_p2 * (3.0f * (t * t) * dt);
    res += trans.m_p3 * (t * t * t);

    return res;
}

/// @addr{0x806EF454}
EGG::Vector3f RailSmoothInterpolator::calcCubicBezierTangentDir(f32 t,
        const RailSplineTransition &trans) const {
    EGG::Vector3f c1 = trans.m_p0 * -1.0f + trans.m_p1 * 3.0f - (trans.m_p2 * 3.0f) + trans.m_p3;
    EGG::Vector3f c2 = trans.m_p0 * 3.0f - trans.m_p1 * 6.0f + trans.m_p2 * 3.0f;
    EGG::Vector3f c3 = trans.m_p0 * -3.0f + trans.m_p1 * 3.0f;
    EGG::Vector3f ret = c1 * 3.0f * (t * t) + c2 * 2.0f * t + c3;

    ret.normalise2();

    if (!m_movementDirectionForward) {
        ret *= -1.0f;
    }

    return ret;
}

/// @addr{0x806EF0F8}
f32 RailSmoothInterpolator::calcT(f32 t) const {
    u16 sampleIdx = m_movementDirectionForward ? m_currPointIdx * m_estimatorSampleCount :
                                                 m_nextPointIdx * m_estimatorSampleCount;

    f32 delta = 0.0f;
    u16 idx = 0;

    for (u16 i = 0; i < m_estimatorSampleCount - 1; ++i) {
        f32 currPercent = m_pathPercentages[sampleIdx + i];
        f32 nextPercent = m_pathPercentages[sampleIdx + i + 1];

        if (currPercent <= t && nextPercent > t) {
            delta = (t - currPercent) / (nextPercent - currPercent);
            idx = i;
        }
    }

    f32 lastPercent = m_pathPercentages[sampleIdx + m_estimatorSampleCount - 1];

    if (lastPercent <= t && 1.0f >= t) {
        idx = m_estimatorSampleCount - 1;
        delta = (t - lastPercent) / (1.0f - lastPercent);
    }

    return m_estimatorStep * static_cast<f32>(idx) + m_estimatorStep * delta;
}

/// @addr{0x806EF664}
void RailSmoothInterpolator::calcNextSegment() {
    f32 nextT = m_segmentT - 1.0f;

    if (m_isOscillating) {
        if (m_nextPointIdx == 0 || m_nextPointIdx == m_pointCount - 1) {
            m_segmentT = 0.0f;
        } else if (m_movementDirectionForward) {
            m_segmentT = nextT * m_transitions[m_currPointIdx].m_length *
                    m_transitions[m_nextPointIdx].m_lengthInv;
        } else {
            m_segmentT = nextT * m_transitions[m_currPointIdx - 1].m_length *
                    m_transitions[m_nextPointIdx - 1].m_lengthInv;
        }
    } else {
        m_segmentT = nextT * m_transitions[m_currPointIdx].m_length *
                m_transitions[m_nextPointIdx].m_lengthInv;
    }

    if (m_segmentT > 1.0f) {
        m_segmentT = 0.99f;
    }

    calcNextIndices();

    if (m_usePerPointVelocities) {
        calcVelocities();
    }
}

} // namespace Field
