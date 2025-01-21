#pragma once

#include "game/field/RailInterpolator.hh"

#include <vector>

namespace Field {

class RailManager {
public:
    /// @beginGetters
    const Rail *rail(size_t idx) const;
    /// @endGetters

    static RailManager *CreateInstance();
    static void DestroyInstance();
    static RailManager *Instance();

private:
    RailManager();
    ~RailManager();

    void createPaths();

    std::vector<Rail *> m_rails;
    std::vector<RailInterpolator *> m_interpolators;
    u16 m_totalRails;
    u16 m_interpolatorTotal;
    u16 m_extraInterplatorCount;
    u16 m_pointCount;
    u16 m_cameraPointCount;
    u16 m_cameraCount;

    static RailManager *s_instance; ///< @addr{0x809C22B0}
};

} // namespace Field
