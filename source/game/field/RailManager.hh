#pragma once

#include "game/field/RailInterpolator.hh"

#include <vector>

namespace Field {

// TODO: Inherit EGG::Disposer
class RailManager {
public:
    /// @beginGetters
    const Rail *rail(size_t idx) const {
        ASSERT(idx < m_rails.size());
        return m_rails[idx];
    }
    /// @endGetters

    static RailManager *CreateInstance();
    static void DestroyInstance();

    static RailManager *Instance() {
        return s_instance;
    }

private:
    RailManager();
    ~RailManager();

    void createPaths();

    std::vector<Rail *> m_rails;
    std::span<RailInterpolator *> m_interpolators;
    u16 m_totalRails;
    u16 m_interpolatorTotal;
    u16 m_extraInterplatorCount;
    u16 m_pointCount;
    u16 m_cameraPointCount;
    u16 m_cameraCount;

    static RailManager *s_instance; ///< @addr{0x809C22B0}
};

} // namespace Field
