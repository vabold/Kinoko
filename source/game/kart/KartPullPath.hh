#pragma once

#include "game/kart/KartObjectProxy.hh"

#include "game/system/map/MapdataPointInfo.hh"

namespace Kart {

class KartPullPath;

/// @brief Tracks the kart's progress along the pull path.
/// @details This implementation merges the global and regional tracker into one class.
class KartPullPathTracker : KartObjectProxy {
public:
    enum class Type {
        Global,
        Regional,
    };

    KartPullPathTracker(KartPullPath *handle, Type type);
    ~KartPullPathTracker();

    void calc();

    void setCurrentIdx(s16 idx) {
        m_currentIdx = idx;
    }

    void setPointInfo(System::MapdataPointInfo *info) {
        m_pointInfo = info;
        m_currentIdx = 0;
    }

private:
    enum class SearchDirection {
        Current,
        Next,
        Previous,
    };

    void calcTrackerGlobal();
    void calcTrackerRegional();

    [[nodiscard]] f32 getDistance(const EGG::Vector3f &point, const EGG::Vector3f &dir) const;
    bool search(SearchDirection searchDirection, s16 &idx, EGG::Vector3f &point,
            EGG::Vector3f &dir) const;

    Type m_type; ///< Replaces inheritance for calc.
    s16 m_currentIdx;
    System::MapdataPointInfo *m_pointInfo;
    KartPullPath *m_handle;
};

/// @brief Manages areas pulling the kart along a given path.
/// @details This implementation merges the base class with the water derived class.
class KartPullPath : KartObjectProxy {
public:
    KartPullPath();
    ~KartPullPath();

    void init();
    void reset();
    void calc();
    void changePoint(s16 idx, f32 distance);

    /// @addr{0x80593E08}
    void resetDistance() {
        m_distance = -1.0f;
    }

    [[nodiscard]] s16 incomingIdx() const {
        return m_incomingIdx;
    }

    [[nodiscard]] const EGG::Vector3f &pullDirection() const {
        return m_pullDirection;
    }

    [[nodiscard]] f32 pullSpeed() const {
        return m_pullSpeed;
    }

    [[nodiscard]] f32 maxPullSpeed() const {
        return m_maxPullSpeed;
    }

    [[nodiscard]] f32 roadSpeedDecay() const {
        return m_roadSpeedDecay;
    }

private:
    bool calcArea();
    void calcPointChange();
    void calcTrackers();
    [[nodiscard]] EGG::Vector3f getPullUnitNormal() const;
    void setTrackerPointInfo(System::MapdataPointInfo *info);

    f32 m_distance;
    System::MapdataPointInfo *m_pointInfo;
    s16 m_incomingIdx;
    s16 m_currentIdx;
    EGG::Vector3f m_pullDirection;
    f32 m_pullSpeed;
    f32 m_maxPullSpeed;
    KartPullPathTracker m_globalTracker;
    KartPullPathTracker m_regionalTracker;
    f32 m_roadSpeedDecay;
    s16 m_areaId;
};

} // namespace Kart
