#include "RailManager.hh"

#include "game/field/Rail.hh"

#include "game/system/CourseMap.hh"

namespace Kinoko::Field {

/// @addr{0x806F09C8}
/// @brief Creates the singleton instance of @ref RailManager and parses all rails from the
/// @ref System::CourseMap
/// @return A pointer to the newly created singleton instance of @ref RailManager
/// @pre Assumes that no singleton instance of @ref RailManager has been created yet.
RailManager *RailManager::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = EGG::egg_new<RailManager>();
    s_instance->createPaths();
    return s_instance;
}

/// @addr{0x806F0A4C}
/// @brief Destroys the singleton instance of the @ref RailManager
/// @pre Assumes that a singleton instance of @ref RailManager has already been created.
void RailManager::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    EGG::egg_delete(instance);
}

/// @addr{0x806F0A3C}
/// @brief Private default constructor
RailManager::RailManager() = default;

/// @addr{0x806F0A98}
/// @brief Private destructor
RailManager::~RailManager() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("RailManager instance not explicitly handled!");
    }

    for (auto *&rail : m_rails) {
        EGG::egg_delete(rail);
    }
}

/// @addr{0x806F0AD8}
/// @brief Parses all rails from the @ref System::CourseMap, distinguishing between linear and
/// curved rails
/// @details For a given @ref System::MapdataPointInfo, if setting 1 is set to 1, then the rail is
/// considered a curved rail and a @ref RailSpline object is created. Otherwise, it is considered a
/// linear rail and a @ref RailLine object is created.
void RailManager::createPaths() {
    auto *courseMap = System::CourseMap::Instance();
    u16 railCount = courseMap->getPointInfoCount();
    m_rails.reserve(railCount);

    for (u16 i = 0; i < railCount; ++i) {
        auto *pointInfo = courseMap->getPointInfo(i);
        bool isSpline = pointInfo->setting(0);

        if (isSpline) {
            m_rails.push_back(EGG::egg_new<RailSpline>(i, pointInfo));
        } else {
            m_rails.push_back(EGG::egg_new<RailLine>(i, pointInfo));
        }
    }
}

RailManager *RailManager::s_instance = nullptr;

} // namespace Kinoko::Field
