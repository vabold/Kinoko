#pragma once

#include <Common.hh>

#include "game/field/Rail.hh"

#include "game/system/map/MapdataPointInfo.hh"

#include <egg/math/Vector.hh>

namespace Field {

class RailInterpolator {
public:
    RailInterpolator(f32 speed, u32 idx);
    virtual ~RailInterpolator();

    virtual void init(f32 t, u32 idx) = 0;
    virtual u32 calc() = 0;
    virtual void setCurrVel(f32 speed) = 0;
    virtual void setT(f32 t);
    virtual f32 getCurrVel() = 0;
    virtual void evalCubicBezierOnPath(EGG::Vector3f &currDir, EGG::Vector3f &curTangentDir) = 0;
    virtual void getPathLocation(f32 t, s16 &idx, f32 &len) = 0;

    [[nodiscard]] const EGG::Vector3f floorNrm(size_t idx) const;
    [[nodiscard]] f32 railLength() const;

    [[nodiscard]] const System::MapdataPointInfo::Point &curPoint() const;
    [[nodiscard]] const EGG::Vector3f &curPos() const;
    [[nodiscard]] const EGG::Vector3f &curTangentDir() const;
    [[nodiscard]] bool isMovementDirectionForward() const;

protected:
    void updateVel();
    void calcVelocities();
    bool shouldChangeDirection() const;
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
    f32 m_currPointIdx;
    f32 m_nextPointIdx;
    bool m_46;
};

class RailLinearInterpolator : public RailInterpolator {
public:
    RailLinearInterpolator(f32 speed, u32 idx);
    ~RailLinearInterpolator() override;

    void init(f32 t, u32 idx) override;
    u32 calc() override;
    void setCurrVel(f32 speed) override;
    f32 getCurrVel() override;
    void evalCubicBezierOnPath(EGG::Vector3f &currDir, EGG::Vector3f &curTangentDir) override;
    void getPathLocation(f32 t, s16 &idx, f32 &len) override;

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
    u32 calc() override;
    void setCurrVel(f32 speed) override;
    f32 getCurrVel() override;
    void evalCubicBezierOnPath(EGG::Vector3f &currDir, EGG::Vector3f &curTangentDir) override;
    void getPathLocation(f32 t, s16 &idx, f32 &len) override;

private:
    void calcCubicBezier(f32 t, u32 currIdx, u32 nextIdx, EGG::Vector3f &pos,
            EGG::Vector3f &dir) const;
    EGG::Vector3f calcCubicBezierPos(f32 t, const RailSplineTransition &trans) const;
    EGG::Vector3f calcCubicBezierTangentDir(f32 t, const RailSplineTransition &trans) const;
    f32 calcT(f32 t) const;
    void calcNextSegment();

    std::span<RailSplineTransition> m_transitions;
    u32 m_estimatorSampleCount;
    f32 m_estimatorStep;
    std::span<f32> m_pathPercentages;
    EGG::Vector3f m_prevPos;
    f32 m_velocity;
};

} // namespace Field
