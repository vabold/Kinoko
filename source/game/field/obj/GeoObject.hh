#pragma once

#include "game/field/obj/ObjectColMgr.hh"
#include "game/system/map/MapdataGeoObj.hh"
#include <egg/math/Matrix.hh>

namespace Field {

class GeoObject {
public:
    GeoObject(System::MapdataGeoObj *pMapDataGeoObj);
    GeoObject(u16 id, const EGG::Vector3f &position, const EGG::Vector3f &rotation,
            const EGG::Vector3f &scale);
    GeoObject(const char *name, const EGG::Vector3f &position, const EGG::Vector3f &rotation,
            const EGG::Vector3f &scale);
    virtual ~GeoObject();

    virtual u16 id() const;
    virtual const char *name() const;
    virtual const char *resources() const;
    virtual EGG::Vector3f position() const;

    /// @beginGetters
    virtual u32 directorIndex() const;
    /// @endGetters

    /// @beginSetters
    virtual void setDirectorIndex(s32 val);
    /// @endSetters

    virtual void init() = 0;
    virtual void loadCollision() = 0;
    virtual void updateCollision() = 0;
    // virtual void unregisterCollision();
    // virtual void disableCollision();
    // virtual void enableCollision();
    virtual f32 calcCollisionRadius();

    void initMembers();
    void updateMatrix();

protected:
    /// @brief Some unknown type, most likely part of an enum. 1 for item boxes, 2 for
    /// GeoObjectDrivable and its derived classes, 0 for everything else
    u16 m_type;

    /// @brief Part of a bitfield. When set, it states that on the next Update call, this object
    /// will update its transformation matrix with its current position.
    bool m_bMatrixShouldUpdatePosition; // flag 0x1

    /// @brief Part of a bitfield. When set, it states that on the next Update call, this object
    /// will update its transformation matrix with its current position and rotation.
    bool m_bMatrixShouldMakeRT; // flag 0x2

    EGG::Vector3f m_pos;
    EGG::Vector3f m_scale;
    EGG::Vector3f m_rot;
    const char *m_modelName;
    EGG::Matrix34f m_transformationMatrix;
    u16 m_id;

    // member is called globalobj in ghidra
    System::MapdataGeoObj *m_mapDataGeoObj;
    s32 m_directorIndex;
    // field 0xa4, is set to true in every ctor and never set to false, prob unused? added it just
    // in case
    bool m_isTangible;
};

struct GeoObjectArray {
    u16 m_count;
    GeoObject **m_array;
};

class GeoObjectDrivable : public GeoObject {
public:
    GeoObjectDrivable(System::MapdataGeoObj *pMapDataGeoObj);
    virtual ~GeoObjectDrivable();

    void init() override;

    virtual void initCollision() = 0;
    virtual void registerBoxColUnit(f32 radius);
};

class GeoObjectKCL : public GeoObjectDrivable {
public:
    GeoObjectKCL(System::MapdataGeoObj *pMapDataGeoObj);
    virtual ~GeoObjectKCL();

    void updateCollision() override;
    void loadCollision() override;
    f32 calcCollisionRadius() override;

    void initCollision() override;

    virtual EGG::Matrix34f &getUpdatedMatrix() = 0;
    virtual f32 getScaleY() const = 0;
    virtual f32 colRadiusAdditionalLength() const = 0;

private:
    ObjectColMgr *m_objectColMgr;
    EGG::Vector3f m_kclMidpoint;
    f32 m_bboxHalfSideLength;
};

} // namespace Field
