#include "RailManager.hh"

#include "game/system/CourseMap.hh"
#include "game/system/map/MapdataGeoObj.hh"

namespace Field {

/// @addr{0x806F09C8}
RailManager *RailManager::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new RailManager;
    s_instance->createPaths();
    return s_instance;
}

/// @addr{0x806F0A4C}
void RailManager::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

/// @addr{0x806F0A3C}
RailManager::RailManager() = default;

/// @addr{0x806F0A98}
RailManager::~RailManager() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("RailManager instance not explicitly handled!");
    }

    for (auto *&rail : m_rails) {
        delete rail;
    }

    for (auto *&interpolator : m_interpolators) {
        delete interpolator;
    }

    delete m_interpolators.data();
}

/// @addr{0x806F0AD8}
void RailManager::createPaths() {
    auto *courseMap = System::CourseMap::Instance();
    m_pointCount = courseMap->getPointInfoCount();
    m_extraInterplatorCount = 8;
    u16 geoCount = courseMap->getGeoObjCount();
    m_rails.reserve(m_pointCount + m_extraInterplatorCount);
    m_interpolatorTotal = m_pointCount;

    // For now, assume screenCount of 1 instead of calling into RaceScenario
    m_cameraPointCount = m_pointCount * 1;
    m_interpolatorTotal = m_pointCount * 1 + m_extraInterplatorCount;
    m_cameraCount = 1;

    // The base game indexes based on i and j, so we need to resize regardless of isObjectRoute.
    m_interpolators = std::span<RailInterpolator *>(new RailInterpolator *[m_interpolatorTotal],
            m_interpolatorTotal);

    // Avoid deallocation headache later
    for (auto *&interpolator : m_interpolators) {
        interpolator = nullptr;
    }

    for (u16 i = 0; i < m_pointCount; ++i) {
        bool isObjectRoute = false;
        auto *pointInfo = courseMap->getPointInfo(i);
        bool isSpline = pointInfo->setting(0);

        for (u16 j = 0; j < geoCount; ++j) {
            auto *geoObj = courseMap->getGeoObj(j);

            if (geoObj->pathId() != i) {
                continue;
            }

            if (isSpline) {
                m_rails.push_back(new RailSpline(i, pointInfo));
            } else {
                m_rails.push_back(new RailLine(i, pointInfo));
            }

            isObjectRoute = true;
            break;
        }

        if (isObjectRoute) {
            continue;
        }

        if (isSpline) {
            m_rails.push_back(new RailSpline(i, pointInfo));

            for (u16 j = 0; j < m_cameraCount; ++j) {
                m_interpolators[j + i * m_cameraCount] = new RailSmoothInterpolator(0.0f, i);
            }
        } else {
            m_rails.push_back(new RailLine(i, pointInfo));

            for (u16 j = 0; j < m_cameraCount; ++j) {
                m_interpolators[j + i * m_cameraCount] = new RailLinearInterpolator(0.0f, i);
            }
        }
    }
}

RailManager *RailManager::s_instance = nullptr; ///> @addr{0x809C22B0}

} // namespace Field
