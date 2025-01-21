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

    [[nodiscard]] u16 pointCount() const {
        return m_pointCount;
    }

    [[nodiscard]] bool isOscillating() const {
        return m_isOscillating;
    }

    [[nodiscard]] const std::span<System::MapdataPointInfo::Point> &points() const {
        return m_points;
    }

    /// @addr{0x806ED150}
    [[nodiscard]] const EGG::Vector3f &pointPos(u16 idx) const {
        ASSERT(idx < m_pointCount);
        return m_points[idx].pos;
    }

    [[nodiscard]] const EGG::Vector3f &floorNrm(u16 idx) const {
        ASSERT(m_floorNrms.data() && idx < m_floorNrms.size());
        return m_floorNrms[idx];
    }

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

    /// @addr{0x806F09A8}
    [[nodiscard]] s32 getEstimatorSampleCount() const override {
        return 0;
    }

    /// @addr{0x806F099C}
    [[nodiscard]] f32 getEstimatorStep() const override {
        return 0.0f;
    }

    /// @addr{0x806F0994}
    /// @brief In the base game we return a nullptr. To mimic this, return an empty vector.
    [[nodiscard]] const std::span<f32> &getPathPercentages() const override {
        static std::span<f32> EMPTY_PERCENTAGES;
        return EMPTY_PERCENTAGES;
    }

private:
    /// @addr{0x806F09C0}
    [[nodiscard]] f32 getPathLength() const override {
        return m_pathLength;
    }

    /// @addr{0x806F09B8}
    [[nodiscard]] const std::span<RailLineTransition> &getLinearTransitions() const override {
        return m_transitions;
    }

    /// @addr{0x806F09B0}
    /// @brief In the base game we return a nullptr. To mimic this, return an empty vector.
    [[nodiscard]] const std::span<RailSplineTransition> &getSplineTransitions() const override {
        static std::span<RailSplineTransition> EMPTY_TRANSITIONS;
        return EMPTY_TRANSITIONS;
    }

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

    /// @addr{0x806EF994}
    [[nodiscard]] s32 getEstimatorSampleCount() const override {
        return m_estimatorSampleCount;
    }

    /// @addr{0x806EF98C}
    [[nodiscard]] f32 getEstimatorStep() const override {
        return m_estimatorStep;
    }

    /// @addr{0x806EF984}
    [[nodiscard]] const std::span<f32> &getPathPercentages() const override {
        return m_pathPercentages;
    }

private:
    /// @addr{0x806EF9AC}
    [[nodiscard]] f32 getPathLength() const override {
        return m_pathLength;
    }

    /// @addr{0x806EF9A4}
    [[nodiscard]] const std::span<RailLineTransition> &getLinearTransitions() const override {
        static std::span<RailLineTransition> EMPTY_TRANSITIONS;
        return EMPTY_TRANSITIONS;
    }

    /// @addr{0x806EF99C}
    [[nodiscard]] const std::span<RailSplineTransition> &getSplineTransitions() const override {
        return m_transitions;
    }

    void onPointsChanged() override;
    void onPointAdded() override;

    void invalidateTransitions(bool lastOnly);
    void calcCubicBezierControlPoints(const EGG::Vector3f &p0, const EGG::Vector3f &p1,
            const EGG::Vector3f &p2, const EGG::Vector3f &p3, s32 count,
            RailSplineTransition &transition);
    [[nodiscard]] f32 estimateLength(const RailSplineTransition &transition, s32 count);
    [[nodiscard]] EGG::Vector3f calcCubicBezierP1(const EGG::Vector3f &p0, const EGG::Vector3f &p1,
            const EGG::Vector3f &p2) const;
    [[nodiscard]] EGG::Vector3f calcCubicBezierP2(const EGG::Vector3f &p0, const EGG::Vector3f &p1,
            const EGG::Vector3f &p2) const;
    [[nodiscard]] EGG::Vector3f cubicBezier(f32 t, const RailSplineTransition &transition) const;

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
