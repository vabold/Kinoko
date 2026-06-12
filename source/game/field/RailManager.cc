#include "RailManager.hh"

#include "game/system/CourseMap.hh"
#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

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
}

/// @addr{0x806F0AD8}
void RailManager::createPaths() {
    auto *courseMap = System::CourseMap::Instance();
    m_pointCount = courseMap->getPointInfoCount();
    m_extraInterplatorCount = 8;
    u16 geoCount = courseMap->getGeoObjCount();
    m_rails.reserve(m_pointCount + m_extraInterplatorCount);

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
        } else {
            m_rails.push_back(new RailLine(i, pointInfo));
        }
    }
}

RailManager *RailManager::s_instance = nullptr; ///> @addr{0x809C22B0}

} // namespace Kinoko::Field
