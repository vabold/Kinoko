#pragma once

#include "game/system/map/MapdataPointInfo.hh"

namespace Field {

struct RailLineTransition {
    f32 m_length;
    f32 m_lengthInv;
    EGG::Vector3f m_dir;
};

struct RailSplineTransition {
    EGG::Vector3f m_p0;
    EGG::Vector3f m_p1;
    EGG::Vector3f m_p2;
    EGG::Vector3f m_p3;
    f32 m_length;
    f32 m_lengthInv;
};

class Rail {
public:
    Rail(u16 idx, System::MapdataPointInfo *info);
    virtual ~Rail();

    virtual f32 getPathLength() const = 0;
    virtual const std::span<RailLineTransition> &getLinearTransitions() const = 0;
    virtual const std::span<RailSplineTransition> &getSplineTransitions() const = 0;
    virtual s32 getEstimatorSampleCount() const = 0;
    virtual f32 getEstimatorStep() const = 0;
    virtual const std::span<f32> &getPathPercentages() const = 0;

    void addPoint(f32 scale, const EGG::Vector3f &point);
    void checkSphereFull();

    [[nodiscard]] u16 pointCount() const;
    [[nodiscard]] bool isOscillating() const;
    [[nodiscard]] const std::span<System::MapdataPointInfo::Point> points() const;
    [[nodiscard]] const EGG::Vector3f &pointPos(u16 idx) const;
    [[nodiscard]] const EGG::Vector3f &floorNrm(u16 idx) const;

protected:
    virtual void onPointsChanged() = 0;
    virtual void onPointAdded() = 0;

    u16 m_pointCount;
    bool m_isOscillating;
    std::span<System::MapdataPointInfo::Point> m_points;
    f32 m_someScale;

private:
    u16 m_idx;
    u16 m_pointCapacity;
    bool m_hasCheckedCol;
    std::span<EGG::Vector3f> m_floorNrms;
};

class RailLine : public Rail {
public:
    RailLine(u16 idx, System::MapdataPointInfo *info);
    ~RailLine() override;

    s32 getEstimatorSampleCount() const override;
    f32 getEstimatorStep() const override;
    const std::span<f32> &getPathPercentages() const override;

private:
    f32 getPathLength() const override;
    const std::span<RailLineTransition> &getLinearTransitions() const override;
    const std::span<RailSplineTransition> &getSplineTransitions() const override;
    void onPointsChanged() override {}
    void onPointAdded() override {}

    u16 m_dirCount;
    std::span<RailLineTransition> m_transitions;
    f32 m_pathLength;
};

class RailSpline : public Rail {
public:
    RailSpline(u16 idx, System::MapdataPointInfo *info);
    ~RailSpline() override;

    s32 getEstimatorSampleCount() const override;
    f32 getEstimatorStep() const override;
    const std::span<f32> &getPathPercentages() const override;

private:
    f32 getPathLength() const override;
    const std::span<RailLineTransition> &getLinearTransitions() const override;
    const std::span<RailSplineTransition> &getSplineTransitions() const override;
    void onPointsChanged() override;
    void onPointAdded() override;

    void invalidateTransitions(bool lastOnly);
    void calcCubicBezierControlPoints(const EGG::Vector3f &p0, const EGG::Vector3f &p1,
            const EGG::Vector3f &p2, const EGG::Vector3f &p3, s32 count,
            RailSplineTransition &transition);
    f32 estimateLength(const RailSplineTransition &transition, s32 count);
    EGG::Vector3f calcCubicBezierP1(const EGG::Vector3f &p0, const EGG::Vector3f &p1,
            const EGG::Vector3f &p2) const;
    EGG::Vector3f calcCubicBezierP2(const EGG::Vector3f &p0, const EGG::Vector3f &p1,
            const EGG::Vector3f &p2) const;
    EGG::Vector3f cubicBezier(f32 t, const RailSplineTransition &transition) const;

    u16 m_transitionCount;
    std::span<RailSplineTransition> m_transitions;
    u32 m_estimatorSampleCount;
    f32 m_estimatorStep;
    std::span<f32> m_pathPercentages;
    s32 m_segmentCount;
    f32 m_pathLength;
    bool m_doNotAllocatePathPercentages;
};

} // namespace Field
