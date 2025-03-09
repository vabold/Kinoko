#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/math/Vector.hh>
#include <egg/util/Stream.hh>

namespace System {

class MapdataArea {
public:
    enum class AreaType : u8 {
        Camera = 0x0,
        EnvEffect = 0x1,
        Fog = 0x2,
        MovingWater = 0x3,
        ForceRecalc = 0x4,
        MinimapCut = 0x5,
        Bloom = 0x6,
        Boo = 0x7,
        ObjGroup = 0x8,
        ObjDisable = 0x9,
        OutOfBounds = 0xa,
    };

    struct SData {
        bool isCylinder;
        AreaType type;
        u8 cameraIdx;
        u8 priority;
        EGG::Vector3f pos;
        EGG::Vector3f rot;
        EGG::Vector3f scale;
        u16 settings[2];
        u8 routeId;
        u8 enemyPointId;
    };
    STATIC_ASSERT(sizeof(SData) == 0x30);

    MapdataArea(const SData *data);

    void read(EGG::Stream &stream);

protected:
    bool m_isCylinder;
    AreaType m_type;
    u8 m_cameraIdx;
    u8 m_priority;
    EGG::Vector3f m_pos;
    EGG::Vector3f m_rot;
    EGG::Vector3f m_scale;
    std::array<u16, 2> m_settings;
    u8 m_routeId;
    u8 m_enemyPointId;
    EGG::Vector3f m_right;
    EGG::Vector3f m_up;
    EGG::Vector3f m_front;
    EGG::Vector3f m_dims;
    f32 m_ellipseXradiusSq;
    f32 m_ellipseAspectRatio;
    f32 m_boundingCircleRadius;
    s16 m_idx;

private:
    const SData *m_rawData;
};

class MapdataAreaBox : public MapdataArea {
public:
    MapdataAreaBox(const SData *data);
};

class MapdataAreaCylinder : public MapdataArea {
public:
    MapdataAreaCylinder(const SData *data);
};

class MapdataAreaAccessor : public MapdataAccessorBase<MapdataArea, MapdataArea::SData> {
public:
    MapdataAreaAccessor(const MapSectionHeader *header);
    ~MapdataAreaAccessor() override;

    void init(const MapdataArea::SData *start, u16 count);
};

} // namespace System
