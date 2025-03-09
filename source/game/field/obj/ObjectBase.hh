#pragma once

#include "game/field/BoxColManager.hh"
#include "game/field/obj/ObjectId.hh"

#include "game/system/map/MapdataGeoObj.hh"

#include <egg/core/BitFlag.hh>
#include <egg/math/Matrix.hh>

namespace Field {

class ObjectBase {
public:
    enum class eDirtyFlags {
        Position = 0,
        Rotation = 1,
        Matrix = 2,
        Scale = 3,
    };
    typedef EGG::TBitFlag<u16, eDirtyFlags> DirtyFlags;

    ObjectBase(const System::MapdataGeoObj &params);
    virtual ~ObjectBase();

    virtual void init() {}
    virtual void calc() {}
    virtual void calcModel();
    virtual void load() = 0;
    virtual void createCollision() = 0;
    virtual void calcCollisionTransform() = 0;

    /// @addr{0x806BF434}
    [[nodiscard]] virtual u32 loadFlags() const {
        // TODO: This references LOD to determine load flags
        return 0;
    }

    /// @addr{0x80681598}
    [[nodiscard]] virtual const EGG::Vector3f &getPosition() const {
        return m_pos;
    }

    /// @addr{0x8080BDC0}
    [[nodiscard]] virtual f32 getCollisionRadius() const {
        return 100.0f;
    }

    /// @addr{0x80572574}
    [[nodiscard]] ObjectId id() const {
        return m_id;
    }

protected:
    void calcTransform();

    ObjectId m_id;
    BoxColUnit *m_boxColUnit;
    DirtyFlags m_dirtyFlags;
    EGG::Vector3f m_pos;
    EGG::Vector3f m_rot;
    EGG::Vector3f m_scale;
    EGG::Matrix34f m_transform;
};

} // namespace Field
