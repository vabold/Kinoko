#pragma once

#include <Common.hh>

#include "game/field/Rail.hh"

#include "game/system/map/MapdataPointInfo.hh"

#include <egg/math/Vector.hh>

namespace Field {

class RailInterpolator {
public:
    enum class Status {
        InProgress = 0,
        SegmentEnd = 1,
        ChangingDirection = 2,
    };

    RailInterpolator(f32 speed, u32 idx);
    virtual ~RailInterpolator();

    virtual void init(f32 t, u32 idx) = 0;
    virtual Status calc() = 0;
    virtual void setCurrVel(f32 speed) = 0;

    virtual f32 getCurrVel() = 0;
    virtual void evalCubicBezierOnPath(f32 t, EGG::Vector3f &currDir,
            EGG::Vector3f &curTangentDir) = 0;
    virtual void getPathLocation(f32 t, s16 &idx, f32 &len) = 0;

    [[nodiscard]] virtual f32 getCurrSegmentLength() const = 0;

    void setPerPointVelocities(bool isSet) {
        m_usePerPointVelocities = isSet;
    }

    /// @addr{0x806C63A8}
    void setT(f32 t) {
        m_segmentT = t;
    }

    [[nodiscard]] const EGG::Vector3f &floorNrm(size_t idx) const;
    [[nodiscard]] f32 railLength() const;

    [[nodiscard]] const System::MapdataPointInfo::Point &curPoint() const {
        ASSERT(static_cast<size_t>(m_currPointIdx) < m_points.size());
        return m_points[m_currPointIdx];
    }

    [[nodiscard]] const System::MapdataPointInfo::Point &nextPoint() const {
        ASSERT(static_cast<size_t>(m_nextPointIdx) < m_points.size());
        return m_points[m_nextPointIdx];
    }

    [[nodiscard]] s16 railIdx() const {
        return m_railIdx;
    }

    [[nodiscard]] u16 pointCount() const {
        return m_pointCount;
    }

    [[nodiscard]] f32 speed() const {
        return m_speed;
    }

    [[nodiscard]] const EGG::Vector3f &curPos() const {
        return m_curPos;
    }

    [[nodiscard]] const EGG::Vector3f &curTangentDir() const {
        return m_curTangentDir;
    }

    [[nodiscard]] f32 currVel() const {
        return m_currVel;
    }

    [[nodiscard]] f32 segmentT() const {
        return m_segmentT;
    }

    [[nodiscard]] bool isMovementDirectionForward() const {
        return m_movementDirectionForward;
    }

    [[nodiscard]] s16 curPointIdx() const {
        return m_currPointIdx;
    }

    [[nodiscard]] s16 nextPointIdx() const {
        return m_nextPointIdx;
    }

protected:
    void updateVel();
    void calcVelocities();
    [[nodiscard]] bool shouldChangeDirection() const;
    void calcDirectionChange();
    void calcNextIndices();

    s16 m_railIdx;
    u16 m_pointCount;
    std::span<System::MapdataPointInfo::Point> m_points;
    bool m_isOscillating;
    f32 m_speed;
    bool m_usePerPointVelocities;
    EGG::Vector3f m_curPos;
    EGG::Vector3f m_curTangentDir;
    f32 m_currVel;
    f32 m_prevPointVel;
    f32 m_nextPointVel;
    f32 m_currSegmentVel;
    f32 m_segmentT;
    bool m_movementDirectionForward;
    s16 m_currPointIdx;
    s16 m_nextPointIdx;
    bool m_4a;
};

class RailLinearInterpolator : public RailInterpolator {
public:
    RailLinearInterpolator(f32 speed, u32 idx);
    ~RailLinearInterpolator() override;

    void init(f32 t, u32 idx) override;
    Status calc() override;
    void setCurrVel(f32 speed) override;

    /// @addr{0x806F0944}
    [[nodiscard]] f32 getCurrVel() override {
        return m_currVel;
    }

    void evalCubicBezierOnPath(f32 t, EGG::Vector3f &currDir,
            EGG::Vector3f &curTangentDir) override;
    void getPathLocation(f32 t, s16 &idx, f32 &len) override;

    /// @addr{0x806F050C}
    [[nodiscard]] f32 getCurrSegmentLength() const override {
        s16 idx = m_movementDirectionForward ? m_currPointIdx : m_nextPointIdx;
        return m_transitions[idx].m_length;
    }

private:
    void calcNextSegment();
    EGG::Vector3f lerp(f32 t, u32 currIdx, u32 nextIdx) const;

    EGG::Vector3f m_currentDirection;
    std::span<RailLineTransition> m_transitions;
};

class RailSmoothInterpolator : public RailInterpolator {
public:
    RailSmoothInterpolator(f32 speed, u32 idx);
    ~RailSmoothInterpolator() override;

    void init(f32 t, u32 idx) override;
    Status calc() override;
    void setCurrVel(f32 speed) override;

    /// @addr{0x806EF93C}
    [[nodiscard]] f32 getCurrVel() override {
        return m_velocity;
    }

    void evalCubicBezierOnPath(f32 t, EGG::Vector3f &currDir,
            EGG::Vector3f &curTangentDir) override;
    void getPathLocation(f32 t, s16 &idx, f32 &len) override;

    /// @addr{0x806EF09C}
    [[nodiscard]] f32 getCurrSegmentLength() const override {
        s16 idx = m_movementDirectionForward ? m_currPointIdx : m_nextPointIdx;
        return m_transitions[idx].m_length;
    }

private:
    void calcCubicBezier(f32 t, u32 currIdx, u32 nextIdx, EGG::Vector3f &pos,
            EGG::Vector3f &dir) const;
    [[nodiscard]] EGG::Vector3f calcCubicBezierPos(f32 t, const RailSplineTransition &trans) const;
    [[nodiscard]] EGG::Vector3f calcCubicBezierTangentDir(f32 t,
            const RailSplineTransition &trans) const;
    [[nodiscard]] f32 calcT(f32 t) const;
    void calcNextSegment();

    std::span<RailSplineTransition> m_transitions;
    u32 m_estimatorSampleCount;
    f32 m_estimatorStep;
    std::span<f32> m_pathPercentages;
    EGG::Vector3f m_prevPos;
    f32 m_velocity;
};

} // namespace Field
