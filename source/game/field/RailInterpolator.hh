#pragma once

#include "game/system/map/MapdataPointInfo.hh"

namespace Kinoko::Field {

class Rail;
struct RailLineTransition;
struct RailSplineTransition;

/// @brief Base class for interpolating along a rail. This class is responsible for calculating the
/// current position and tangent direction along the rail based on a given speed. It supports both
/// linear (@ref RailLinearInterpolator) and smooth (@ref RailSmoothInterpolator) interpolation
/// methods.
class RailInterpolator {
public:
    /// @brief Represents the status of the interpolation process
    enum class Status {
        InProgress = 0, ///< Currently interpolating along a segment
        SegmentEnd = 1, ///< Reached the end of a segment, ready to transition to the next
        RailEnd = 2,    ///< Reached the end of the rail
    };

    RailInterpolator(f32 speed, u32 idx);
    virtual ~RailInterpolator();

    /// @brief Initializes the interpolator with a specific segment and position along that segment
    /// @param t The normalized position along the current segment `[0.0f, 1.0f]`
    /// @param idx The index of the current point in the rail's point list
    virtual void init(f32 t, u32 idx) = 0;

    /// @brief Calculates the next position and tangent direction along the rail
    /// @return The status of the interpolation after the calculation
    virtual Status calc() = 0;

    /// @brief Sets the current velocity that will be used for interpolation
    /// @param speed The new velocity to set
    virtual void setSpeed(f32 speed) = 0;

    /// @brief Returns the current velocity being used for interpolation
    /// @return The current velocity
    [[nodiscard]] virtual f32 getCurrSpeed() = 0;

    /// @brief Fetches the position and tangent direction along the rail at a specific `dist`
    /// distance *behind* the current position
    /// @param dist The distance behind the current position along the rail
    /// @param currPos The calculated position along the rail at `dist`
    /// @param curTangentDir The calculated tangent direction along the rail at `dist`
    virtual void evalPositionAndTangentBehind(f32 dist, EGG::Vector3f &currPos,
            EGG::Vector3f &curTangentDir) const = 0;

    /// @brief Finds the segment of the rail that corresponds to a given distance `dist` behind the
    /// current position and calculates the normalized position along that segment
    /// @param dist The distance behind the current position along the rail
    /// @param idx The index of the segment that corresponds to `dist`
    /// @param t The normalized position along the segment that corresponds to `dist`
    virtual void getPathLocation(f32 dist, s16 &idx, f32 &t) const = 0;

    /// @brief The length of the current segment being interpolated along
    /// @return The length of the current segment
    [[nodiscard]] virtual f32 getCurrSegmentLength() const = 0;

    void reverseDirection();

    /// @beginSetters

    /// @brief Sets whether to use per-point velocities for interpolation
    /// @param isSet `true` to use per-point velocities, `false` to use a constant velocity
    void setPerPointVelocities(bool isSet) {
        m_usePerPointVelocities = isSet;
    }

    /// @addr{0x806C63A8}
    /// @brief Sets the normalized position along the current segment
    /// @param t The normalized position along the current segment [0.0, 1.0]
    void setT(f32 t) {
        m_segmentT = t;
    }

    /// @endSetters

    /// @beginGetters

    [[nodiscard]] const EGG::Vector3f &floorNrm(size_t idx) const;
    [[nodiscard]] f32 railLength() const;

    /// @brief Gets the point that the rail interpolator is currently at or past
    /// @return The current point that the rail interpolator is at or past
    /// @pre Requires that @ref m_currPointIdx is a valid index within @ref m_points
    [[nodiscard]] const System::MapdataPointInfo::Point &curPoint() const {
        ASSERT(static_cast<size_t>(m_currPointIdx) < m_points.size());
        return m_points[m_currPointIdx];
    }

    /// @brief Gets the point that the rail interpolator is moving towards
    /// @return The next point that the rail interpolator is moving towards
    /// @pre Requires that @ref m_nextPointIdx is a valid index within @ref m_points
    [[nodiscard]] const System::MapdataPointInfo::Point &nextPoint() const {
        ASSERT(static_cast<size_t>(m_nextPointIdx) < m_points.size());
        return m_points[m_nextPointIdx];
    }

    /// @brief Gets the underlying rail that this class interpolates along
    /// @return A const pointer to the underlying rail, @ref m_rail
    [[nodiscard]] const Rail *rail() const {
        return m_rail;
    }

    /// @brief Gets the number of points along the rail
    /// @return The number of points along the rail, @ref m_pointCount
    [[nodiscard]] u16 pointCount() const {
        return m_pointCount;
    }

    /// @brief Gets the default speed of the rail interpolator
    /// @return The default speed of the rail interpolator, @ref m_baseSpeed
    [[nodiscard]] f32 speed() const {
        return m_baseSpeed;
    }

    /// @brief Gets the current position of the rail interpolator
    /// @return The current position of the rail interpolator, @ref m_curPos
    [[nodiscard]] const EGG::Vector3f &curPos() const {
        return m_curPos;
    }

    /// @brief Gets the current tangent direction of the rail interpolator
    /// @return The current tangent direction of the rail interpolator, @ref m_curTangentDir
    [[nodiscard]] const EGG::Vector3f &curTangentDir() const {
        return m_curTangentDir;
    }

    /// @brief Gets the current speed of the rail interpolator
    /// @return The current speed of the rail interpolator, @ref m_currSpeed
    [[nodiscard]] f32 currSpeed() const {
        return m_currSpeed;
    }

    /// @brief Gets the normalized position along the current segment
    /// @return The normalized position along the current segment [0.0, 1.0], @ref m_segmentT
    [[nodiscard]] f32 segmentT() const {
        return m_segmentT;
    }

    /// @brief Checks if the movement direction is forward
    /// @return The value of @ref m_forward
    [[nodiscard]] bool isMovementDirectionForward() const {
        return m_forward;
    }

    /// @brief Gets the index of the current point
    /// @return The index of the current point within the rail's points, @ref m_currPointIdx
    [[nodiscard]] s16 curPointIdx() const {
        return m_currPointIdx;
    }

    /// @brief Gets the index of the next point
    /// @return The index of the next point within the rail's points, @ref m_nextPointIdx
    [[nodiscard]] s16 nextPointIdx() const {
        return m_nextPointIdx;
    }

    /// @endGetters

protected:
    void updateVel();
    void calcVelocities();
    void calcDirectionChange();
    void calcNextIndices();

    [[nodiscard]] bool hasReachedEndOfRail() const;

    template <typename T>
        requires std::is_same_v<T, RailLineTransition> || std::is_same_v<T, RailSplineTransition>
    void getPathLocationImpl(std::span<const T> transitions, f32 dist, s16 &idx, f32 &t) const;

    const Rail *const m_rail; ///< Pointer to the rail being interpolated along
    const std::span<const System::MapdataPointInfo::Point> m_points; ///< The points in the rail
    const u16 m_pointCount;        ///< The number of nodes/points in the rail
    const bool m_isOscillating;    ///< Whether the rail is a loop or a back-and-forth path
    const f32 m_baseSpeed;         ///< Default speed used when per-point velocities are not enabled
    bool m_usePerPointVelocities;  ///< When true, uses velocities defined at each point in the rail
    EGG::Vector3f m_curPos;        ///< The current position along the rail
    EGG::Vector3f m_curTangentDir; ///< The current tangent direction along the rail
    f32 m_currSpeed;    ///< Current speed either set via @ref setSpeed or from per-point velocities
    f32 m_currPointVel; ///< The velocity at the current point in the rail
    f32 m_nextPointVel; ///< The velocity at the next point in the rail
    f32 m_currSegmentVel; ///< The velocity along the current segment
    f32 m_segmentT;       ///< The normalized position along the current segment `[0.0f, 1.0f]`
    bool m_forward;       ///< Whether the interpolator is moving forward or backward along the rail
    s16 m_currPointIdx;   ///< The index of the current point in the rail's point list
    s16 m_nextPointIdx;   ///< The index of the next point in the rail's point list
};

/// @brief Interpolates along a rail using linear interpolation between points
class RailLinearInterpolator final : public RailInterpolator {
public:
    RailLinearInterpolator(f32 speed, u32 idx);
    ~RailLinearInterpolator() override;

    void init(f32 t, u32 idx) override;
    Status calc() override;

    void setSpeed(f32 speed) override;
    void evalPositionAndTangentBehind(f32 dist, EGG::Vector3f &currPos,
            EGG::Vector3f &curTangentDir) const override;
    void getPathLocation(f32 dist, s16 &idx, f32 &t) const override;
    [[nodiscard]] f32 getCurrSpeed() override;
    [[nodiscard]] f32 getCurrSegmentLength() const override;

private:
    void calcNextSegment();
    [[nodiscard]] EGG::Vector3f lerp(f32 t, u32 currIdx, u32 nextIdx) const;

    EGG::Vector3f m_currVel; ///< Position displacement from the current point to the next point

    /// @brief The length of @ref m_currVel
    /// @details Kinoko-specific optimization. @ref setSpeed() is called every frame when using
    /// per-point velocities, so we cache the length rather than recomputing it each time.
    f32 m_currVelLength;

    const std::span<const RailLineTransition> m_transitions; ///< Transitions between rail points
};

/// @brief Interpolates along a rail using cubic bezier curves between points
class RailSmoothInterpolator final : public RailInterpolator {
public:
    RailSmoothInterpolator(f32 speed, u32 idx);
    ~RailSmoothInterpolator() override;

    void init(f32 t, u32 idx) override;
    Status calc() override;
    void setSpeed(f32 speed) override;
    [[nodiscard]] f32 getCurrSpeed() override;
    [[nodiscard]] f32 getCurrSegmentLength() const override;
    void evalPositionAndTangentBehind(f32 dist, EGG::Vector3f &currPos,
            EGG::Vector3f &curTangentDir) const override;
    void getPathLocation(f32 dist, s16 &idx, f32 &t) const override;

private:
    void calcNextSegment();
    void calcCubicBezier(f32 t, u32 currIdx, u32 nextIdx, EGG::Vector3f &pos,
            EGG::Vector3f &dir) const;
    [[nodiscard]] EGG::Vector3f calcCubicBezierTangentDir(f32 t,
            const RailSplineTransition &trans) const;
    [[nodiscard]] f32 calcT(f32 t) const;

    const std::span<const RailSplineTransition> m_transitions; ///< Transitions between rail points

    /// @brief The percentage of the arc length at each sample point along the cubic bezier curves
    const std::span<const f32> m_pathPercentages;

    EGG::Vector3f m_prevPos; ///< The previous frame's position, used to calculate speed
    f32 m_velocity;          ///< Distance traveled since the previous frame

    /// @brief Whether @ref m_velocity needs to be recomputed from @ref m_curPos and @ref m_prevPos
    /// @details Kinoko-specific optimization. Few objects read @ref getCurrSpeed(), so @ref calc()
    /// defers computing @ref m_velocity until it is read.
    bool m_velocityPending;

    [[nodiscard]] static EGG::Vector3f CalcCubicBezierPos(f32 t, const RailSplineTransition &trans);
};

} // namespace Kinoko::Field
