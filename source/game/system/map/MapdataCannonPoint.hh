#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/math/Vector.hh>

namespace System {
class MapdataCannonPoint {
public:
    struct CannonParameter {
        f32 speed;
        f32 height;
        f32 decelFactor;
        f32 endDecel;
    };

    struct SData {
        EGG::Vector3f pos;
        EGG::Vector3f rot;
        u16 id;
        s16 propertyIdx;
    };

    MapdataCannonPoint(const SData *data);
    void read(EGG::Stream &stream);

    /// @beginGetters
    const EGG::Vector3f &pos() const;
    const EGG::Vector3f &rot() const;
    u16 id() const;
    const CannonParameter &parameters() const;
    /// @endGetters

    static constexpr std::array<CannonParameter, 3> CANNON_PARAMETERS = {{
            {500.0f, 0.0f, 6000.0f, -1.0f},
            {500.0f, 5000.0f, 6000.0f, -1.0f},
            {120.0f, 2000.0f, 1000.0f, 45.0f},
    }};

private:
    const SData *m_rawData;
    EGG::Vector3f m_pos;
    EGG::Vector3f m_rot;
    u16 m_id;
    s16 m_paramterIdx; ///< Index into the table of cannon properties. Used to determine speed,
                       ///< height, and decel
};

class MapdataCannonPointAccessor
    : public MapdataAccessorBase<MapdataCannonPoint, MapdataCannonPoint::SData> {
public:
    MapdataCannonPointAccessor(const MapSectionHeader *header);
    ~MapdataCannonPointAccessor() override;
};

} // namespace System
