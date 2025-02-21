#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include "game/kart/KartMove.hh"

#include <egg/math/Vector.hh>

namespace System {

class MapdataCannonPoint {
public:
    struct SData {
        EGG::Vector3f pos;
        EGG::Vector3f rot;
        u16 id;
        s16 propertyIdx;
    };

    MapdataCannonPoint(const SData *data);
    void read(EGG::Stream &stream);

    /// @beginGetters
    const EGG::Vector3f &pos() const {
        return m_pos;
    }

    const EGG::Vector3f &rot() const {
        return m_rot;
    }

    u16 id() const {
        return m_id;
    }

    s16 parameterIdx() const {
        return m_parameterIdx;
    }
    /// @endGetters

private:
    const SData *m_rawData;
    EGG::Vector3f m_pos;
    EGG::Vector3f m_rot;
    u16 m_id;
    s16 m_parameterIdx; ///< Index into the table of cannon properties. Used to determine speed,
                        ///< height, and decel
};

class MapdataCannonPointAccessor
    : public MapdataAccessorBase<MapdataCannonPoint, MapdataCannonPoint::SData> {
public:
    MapdataCannonPointAccessor(const MapSectionHeader *header);
    ~MapdataCannonPointAccessor() override;
};

} // namespace System
