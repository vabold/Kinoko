#pragma once

#include <Types.hh>

namespace Kinoko {

namespace Host {

class Context;

} // namespace Host

namespace Field {

class Rail;

/// @brief Creates and exposes access to the rails from @ref System::CourseMap
/// @details For each rail, constructs the appropriate linear rail (@ref RailLine) or curved rail
/// (@ref RailSpline) object from the corresponding @ref System::MapdataPointInfo that describes the
/// rail's path.
class RailManager {
    /// @brief Grants access to the singleton so a @ref Host::Context can restore the instance's
    /// state on context switch
    friend class Host::Context;

public:
    /// @beginGetters

    /// @brief Fetches the rail at the given index
    /// @param idx The index of the rail to fetch
    /// @return A pointer to the rail at the given index in @ref m_rails
    /// @pre Requires that `idx` is a valid index within @ref m_rails
    [[nodiscard]] Rail *rail(size_t idx) {
        ASSERT(idx < m_rails.size());
        return m_rails[idx];
    }

    /// @brief Fetches the rail at the given index
    /// @param idx The index of the rail to fetch
    /// @return A const pointer to the rail at the given index in @ref m_rails
    /// @pre Requires that `idx` is a valid index within @ref m_rails
    [[nodiscard]] const Rail *rail(size_t idx) const {
        ASSERT(idx < m_rails.size());
        return m_rails[idx];
    }

    /// @endGetters

    /// @beginSingletonFuncs

    static RailManager *CreateInstance();
    static void DestroyInstance();

    /// @brief Returns the singleton instance of the @ref RailManager
    /// @return A pointer to the singleton instance of the @ref RailManager
    [[nodiscard]] static RailManager *Instance() {
        return s_instance;
    }

    /// @endSingletonFuncs

private:
    EGG_NEW_DELETE_FRIEND

    RailManager();
    ~RailManager();

    void createPaths();

    fixed_vector<Rail *> m_rails; ///< The rails parsed from @ref System::MapdataPointInfo

    /// @addr{0x809C22B0}
    /// @brief The pointer to the singleton instance of the class
    static RailManager *s_instance;
};

} // namespace Field

} // namespace Kinoko
