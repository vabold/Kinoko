#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/math/Vector.hh>

namespace System {

/// @brief This class represents the course's respawn positions.
class MapdataJugemPoint {
public:
    struct SData {
        EGG::Vector3f pos;
        EGG::Vector3f rot;
        u8 _18[0x1c - 0x18];
    };
    static_assert(sizeof(SData) == 0x1c);

    MapdataJugemPoint(const SData *data);
    void read(EGG::Stream &stream);

    /// @beginGetters
    const EGG::Vector3f &pos() const {
        return m_pos;
    }

    const EGG::Vector3f &rot() const {
        return m_rot;
    }
    /// @endGetters

private:
    const SData *m_rawData;
    EGG::Vector3f m_pos;
    EGG::Vector3f m_rot;
};

class MapdataJugemPointAccessor
    : public MapdataAccessorBase<MapdataJugemPoint, MapdataJugemPoint::SData> {
public:
    MapdataJugemPointAccessor(const MapSectionHeader *header);
    ~MapdataJugemPointAccessor() override;
};

} // namespace System
