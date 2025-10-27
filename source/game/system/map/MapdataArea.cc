#include "MapdataArea.hh"

#include "game/system/CourseMap.hh"

namespace System {

MapdataAreaBase::MapdataAreaBase(const SData *data, s16 index) : m_rawData(data), m_index(index) {
    EGG::RamStream stream = EGG::RamStream(data,
            CourseMap::Instance()->version() > 2200 ? sizeof(SData) : sizeof(SData) - 4);
    read(stream);

    m_index = -1;
    m_sqBoundingSphereRadius = 0.0f;
    m_ellipseAspectRatio = 0.0f;
    m_ellipseRadiusSq = 0.0f;
    m_dimensions = EGG::Vector3f::zero;
    m_right = EGG::Vector3f::zero;
    m_up = EGG::Vector3f::zero;
    m_forward = EGG::Vector3f::zero;
}

void MapdataAreaBase::read(EGG::Stream &stream) {
    m_shape = static_cast<Shape>(stream.read_s8());
    m_type = static_cast<Type>(stream.read_s8());
    m_cameraIdx = stream.read_s8();
    m_priority = stream.read_u8();
    m_position.read(stream);
    m_rotation.read(stream);
    m_scale.read(stream);

    for (auto &param : m_params) {
        param = stream.read_s16();
    }

    if (CourseMap::Instance()->version() > 2200) {
        m_railId = stream.read_s8();
        m_enemyId = stream.read_u8();
    }
}

/// @addr{0x80516168}
MapdataPointInfo *MapdataAreaBase::getPointInfo() const {
    // The rail ID doesn't exist in prior versions, so there's no point info
    auto *courseMap = CourseMap::Instance();
    if (courseMap->version() < 2200) {
        return nullptr;
    }

    return courseMap->getPointInfo(m_railId);
}

MapdataAreaBox::MapdataAreaBox(const SData *data, s16 index) : MapdataAreaBase(data, index) {
    m_dimensions.x = 0.5f * (10000.0f * m_scale.x);
    m_dimensions.y = 10000.0f * m_scale.y;
    m_dimensions.z = 0.5f * (10000.0f * m_scale.z);

    m_ellipseAspectRatio = 0.0f;
    m_ellipseRadiusSq = 0.0f;
    m_sqBoundingSphereRadius = m_dimensions.squaredLength();

    EGG::Quatf rotation = EGG::Quatf::FromRPY(DEG2RAD * m_rotation.x, DEG2RAD * m_rotation.y,
            DEG2RAD * m_rotation.z);

    m_right = rotation.rotateVector(EGG::Vector3f::ex);
    m_up = rotation.rotateVector(EGG::Vector3f::ey);
    m_forward = rotation.rotateVector(EGG::Vector3f::ez);
}

bool MapdataAreaBox::testImpl(const EGG::Vector3f &pos) const {
    EGG::Vector3f relPos = pos - m_position;

    f32 u = relPos.dot(m_up);
    if (u > m_dimensions.y || u < 0.0f) {
        return false;
    }

    f32 r = relPos.dot(m_right);
    if (r > m_dimensions.x || r < -m_dimensions.x) {
        return false;
    }

    f32 f = relPos.dot(m_forward);
    if (f > m_dimensions.z || f < -m_dimensions.z) {
        return false;
    }

    return true;
}

MapdataAreaCylinder::MapdataAreaCylinder(const SData *data, s16 index)
    : MapdataAreaBase(data, index) {
    m_dimensions = m_scale * 5000.0f;
    m_ellipseRadiusSq = m_dimensions.x * m_dimensions.x;
    m_sqBoundingSphereRadius = m_dimensions.x * m_dimensions.x + m_dimensions.z * m_dimensions.z;
    m_ellipseAspectRatio = m_scale.x / m_scale.z;

    EGG::Quatf rotation = EGG::Quatf::FromRPY(DEG2RAD * m_rotation.x, DEG2RAD * m_rotation.y,
            DEG2RAD * m_rotation.z);

    m_right = rotation.rotateVector(EGG::Vector3f::ex);
    m_up = rotation.rotateVector(EGG::Vector3f::ey);
    m_forward = rotation.rotateVector(EGG::Vector3f::ez);
}

bool MapdataAreaCylinder::testImpl(const EGG::Vector3f &pos) const {
    EGG::Vector3f relPos = pos - m_position;

    if (relPos.dot(m_up) > m_dimensions.y) {
        return false;
    }

    f32 f = m_ellipseAspectRatio * relPos.dot(m_forward);
    f32 r = relPos.dot(m_right);
    if (r * r + f * f < m_ellipseRadiusSq) {
        return false;
    }

    return true;
}

MapdataAreaAccessor::MapdataAreaAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataAreaBase, MapdataAreaBase::SData>(header) {
    init(reinterpret_cast<const MapdataAreaBase::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));

    m_sortedEntries =
            std::span<MapdataAreaBase *>(new MapdataAreaBase *[m_entryCount], m_entryCount);
}

MapdataAreaAccessor::~MapdataAreaAccessor() {
    delete m_sortedEntries.data();
}

void MapdataAreaAccessor::init(const MapdataAreaBase::SData *start, u16 count) {
    if (count != 0) {
        m_entryCount = count;
        m_entries = new MapdataAreaBase *[count];
    }

    for (u16 i = 0; i < count; ++i) {
        // Iterators will be wrong for versions < 2200, so we need to factor out the new members
        const MapdataAreaBase::SData *data = CourseMap::Instance()->version() > 2200 ?
                &start[i] :
                reinterpret_cast<const MapdataAreaBase::SData *>(
                        reinterpret_cast<uintptr_t>(start) +
                        i * (sizeof(MapdataAreaBase::SData) - 4));

        // Reading this is safe because 8-bit integers are immune to endianness issues
        MapdataAreaBase::Shape shape = static_cast<MapdataAreaBase::Shape>(data->shape);

        switch (shape) {
        case MapdataAreaBase::Shape::Box:
            m_entries[i] = new MapdataAreaBox(data, i);
            break;
        case MapdataAreaBase::Shape::Cylinder:
            m_entries[i] = new MapdataAreaCylinder(data, i);
            break;
        default:
            PANIC("Invalid area shape!");
            break;
        }
    }
}

void MapdataAreaAccessor::sort() {
    for (size_t i = 0; i < m_entryCount; ++i) {
        m_sortedEntries[i] = get(i);
    }

    for (size_t i = 1; i < m_entryCount; ++i) {
        size_t j = i;
        for (; j > 0 && m_sortedEntries[j - 1]->priority() < m_sortedEntries[i]->priority(); --j) {
            m_sortedEntries[j] = m_sortedEntries[j - 1];
        }
    }
}

} // namespace System
