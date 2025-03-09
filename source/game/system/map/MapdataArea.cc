#include "MapdataArea.hh"

#include "game/system/CourseMap.hh"

#include <egg/math/Math.hh>
#include <egg/math/Quat.hh>

namespace System {

MapdataArea::MapdataArea(const SData *data) : m_rawData(data) {
    m_idx = -1;

    u8 *unsafeData = reinterpret_cast<u8 *>(const_cast<SData *>(data));
    EGG::RamStream stream = EGG::RamStream(unsafeData, sizeof(SData));
    read(stream);

    m_boundingCircleRadius = 0.0f;
    m_ellipseAspectRatio = 0.0f;
    m_ellipseXradiusSq = 0.0f;
    m_dims = EGG::Vector3f::zero;
    m_right = EGG::Vector3f::zero;
    m_up = EGG::Vector3f::zero;
    m_front = EGG::Vector3f::zero;
}

void MapdataArea::read(EGG::Stream &stream) {
    m_isCylinder = stream.read_u8();
    m_type = static_cast<AreaType>(stream.read_u8());
    m_cameraIdx = stream.read_u8();
    m_priority = stream.read_u8();
    m_pos.read(stream);
    m_rot.read(stream);
    m_scale.read(stream);
    m_settings[0] = stream.read_u16();
    m_settings[1] = stream.read_u16();
    m_routeId = stream.read_u8();
    m_enemyPointId = stream.read_u8();
}

MapdataAreaBox::MapdataAreaBox(const SData *data) : MapdataArea(data) {
    m_dims = 0.5f * 10000.0f * m_scale;
    m_boundingCircleRadius = m_dims.squaredLength();

    EGG::Quatf quat;
    quat.setRPY(m_rot * DEG2RAD);
    m_right = quat.rotateVector(EGG::Vector3f::ex);
    m_up = quat.rotateVector(EGG::Vector3f::ey);
    m_front = quat.rotateVector(EGG::Vector3f::ez);
}

MapdataAreaCylinder::MapdataAreaCylinder(const SData *data) : MapdataArea(data) {
    m_dims = 5000.0f * m_scale;
    m_ellipseXradiusSq = m_dims.x * m_dims.x;
    m_boundingCircleRadius = m_ellipseXradiusSq + m_dims.z * m_dims.z;

    EGG::Quatf quat;
    quat.setRPY(m_rot * DEG2RAD);
    m_right = quat.rotateVector(EGG::Vector3f::ex);
    m_up = quat.rotateVector(EGG::Vector3f::ey);
    m_front = quat.rotateVector(EGG::Vector3f::ez);
}

MapdataAreaAccessor::MapdataAreaAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataArea, MapdataArea::SData>(header) {
    init(reinterpret_cast<const MapdataArea::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));
}

MapdataAreaAccessor::~MapdataAreaAccessor() = default;

/// @brief SData size varies depending on course version.
/// The entries also vary depending on whether they are cylinders or not.
void MapdataAreaAccessor::init(const MapdataArea::SData *start, u16 count) {
    if (count != 0) {
        m_entryCount = count;
        m_entries = new MapdataArea *[count];
    }

    bool isOldVer = CourseMap::Instance()->version() < 2200;

    for (u16 i = 0; i < count; ++i) {
        const MapdataArea::SData *data = &start[i];

        // Older track versions do not have routeId or enemyPointId.
        if (isOldVer) [[unlikely]] {
            constexpr size_t oldSize = offsetof(MapdataArea::SData, MapdataArea::SData::routeId);

            data = reinterpret_cast<MapdataArea::SData *>(
                    reinterpret_cast<uintptr_t>(data) + oldSize);
        } else [[likely]] {
            ++data;
        }

        bool isCylinder = data->isCylinder;

        if (isCylinder) {
            m_entries[i] = new MapdataAreaCylinder(data);
        } else {
            m_entries[i] = new MapdataAreaBox(data);
        }
    }
}

} // namespace System
