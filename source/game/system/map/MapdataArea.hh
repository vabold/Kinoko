#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/math/Vector.hh>

namespace System {

class MapdataPointInfo;

class MapdataAreaBase {
public:
    struct SData {
        s8 shape;
        s8 type;
        s8 cameraIdx;
        u8 priority;
        EGG::Vector3f position;
        EGG::Vector3f rotation;
        EGG::Vector3f scale;
        s16 parameters[2];
        // Pre Revision 2200: End of structure
        s8 railId;
        u8 enemyId;
    };

    enum class Shape {
        Box = 0,
        Cylinder = 1,
    };

    enum class Type {
        MovingRoad = 3,
    };

    MapdataAreaBase(const SData *data, s16 index);
    void read(EGG::Stream &stream);

    virtual bool testImpl(const EGG::Vector3f &pos) const = 0;

    /// @addr{0x805160B0}
    bool test(const EGG::Vector3f &pos) const {
        return (m_position - pos).squaredLength() > m_sqBoundingSphereRadius ? testImpl(pos) :
                                                                               false;
    }

    MapdataPointInfo *getPointInfo() const;

    Type type() const {
        return m_type;
    }

    u8 priority() const {
        return m_priority;
    }

    s16 param(size_t i) const {
        ASSERT(i < m_params.size());
        return m_params[i];
    }

    s16 index() const {
        return m_index;
    }

protected:
    const SData *m_rawData;
    Shape m_shape;
    Type m_type;
    s8 m_cameraIdx;
    u8 m_priority;
    EGG::Vector3f m_position;
    EGG::Vector3f m_rotation;
    EGG::Vector3f m_scale;
    std::array<s16, 2> m_params;
    s8 m_railId;
    u8 m_enemyId;

    EGG::Vector3f m_right;
    EGG::Vector3f m_up;
    EGG::Vector3f m_forward;
    EGG::Vector3f m_dimensions;
    f32 m_ellipseRadiusSq;
    f32 m_ellipseAspectRatio;
    f32 m_sqBoundingSphereRadius; ///< Used to phase out intersection tests early.
    s16 m_index;
};

class MapdataAreaBox : public MapdataAreaBase {
public:
    MapdataAreaBox(const SData *data, s16 index);

    bool testImpl(const EGG::Vector3f &pos) const override;
};

class MapdataAreaCylinder : public MapdataAreaBase {
public:
    MapdataAreaCylinder(const SData *data, s16 index);

    bool testImpl(const EGG::Vector3f &pos) const override;
};

class MapdataAreaAccessor : public MapdataAccessorBase<MapdataAreaBase, MapdataAreaBase::SData> {
public:
    MapdataAreaAccessor(const MapSectionHeader *header);
    ~MapdataAreaAccessor() override;

    void init(const MapdataAreaBase::SData *start, u16 count);
    void sort();

    [[nodiscard]] MapdataAreaBase *getSorted(u16 i) const {
        ASSERT(m_sortedEntries.data());
        return i < m_entryCount ? m_sortedEntries[i] : nullptr;
    }

private:
    std::span<MapdataAreaBase *> m_sortedEntries;
};

} // namespace System
