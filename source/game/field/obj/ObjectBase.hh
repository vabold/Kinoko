#pragma once

#include "game/field/BoxColManager.hh"
#include "game/field/RailInterpolator.hh"
#include "game/field/obj/ObjectId.hh"

#include "game/render/DrawMdl.hh"

#include "game/system/map/MapdataGeoObj.hh"

#include <egg/core/BitFlag.hh>
#include <egg/math/Matrix.hh>

namespace Kinoko::Field {

class ObjectBase {
public:
    enum class eFlags {
        Position = 0,
        Rotation = 1,
        Matrix = 2,
        Scale = 3,
    };
    typedef EGG::TBitFlag<u16, eFlags> Flags;

    /// @brief Describes whether the object needs to be updated every frame
    /// @details For the current purposes of Kinoko, we really only care about `Calc`. The others
    /// are effectively unused.
    enum class eLoadFlags {
        Calc = 0,         ///< The state of the object should be recalculated every frame
        Draw = 1,         ///< The object should be re-drawn every frame
        AlwaysRecalc = 2, ///< The object's @ref BoxColUnit will always be recalculated
    };

    /// @brief A bitfield of @ref eLoadFlags that represents the update requirements of the object
    typedef EGG::TBitFlag<u8, eLoadFlags> LoadFlags;

    ObjectBase(const System::MapdataGeoObj &params);
    ObjectBase(const char *name, const EGG::Vector3f &pos, const EGG::Vector3f &rot,
            const EGG::Vector3f &scale);
    virtual ~ObjectBase();

    virtual void init() {}

    virtual void calc() {}

    virtual void calcModel();
    virtual void load() = 0;
    [[nodiscard]] virtual const char *getResources() const;
    virtual void loadGraphics();

    virtual void loadAnims() {}

    virtual void createCollision() = 0;
    virtual void loadRail();
    virtual void calcCollisionTransform() = 0;

    [[nodiscard]] virtual const char *getName() const;

    /// @addr{0x806BF434}
    /// @brief Returns a bitmask indicating which lifecycle hooks should be called for the object
    /// @return No flags set by default
    /// @details In Kinoko, the only effective behavior of this bitmask is to determine whether
    /// calc() should be called. The other bits are unused.
    [[nodiscard]] virtual LoadFlags loadFlags() const {
        // TODO: This references LOD to determine load flags
        return LoadFlags();
    }

    [[nodiscard]] virtual const char *getKclName() const;

    /// @addr{0x80821DB8}
    virtual void resize(f32 radius, f32 maxSpeed) {
        m_boxColUnit->resize(radius, maxSpeed);
    }

    /// @addr{0x80821DD8}
    virtual void unregisterCollision() {
        BoxColManager::Instance()->remove(m_boxColUnit);
    }

    /// @addr{0x80821DEC}
    virtual void disableCollision() const {
        m_boxColUnit->m_flag.setBit(eBoxColFlag::Intangible);
    }

    /// @addr{0x80821E00}
    virtual void enableCollision() const {
        m_boxColUnit->m_flag.resetBit(eBoxColFlag::Intangible);
    }

    /// @addr{0x80680618}
    virtual const BoxColUnit *getUnit() const {
        return m_boxColUnit;
    }

    [[nodiscard]] const RailInterpolator *railInterpolator() const {
        return m_railInterpolator;
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
    [[nodiscard]] virtual ObjectId id() const {
        return m_id;
    }

    void setPos(const EGG::Vector3f &pos) {
        m_flags.setBit(eFlags::Position);
        m_pos = pos;
    }

    void addPos(const EGG::Vector3f &v) {
        m_flags.setBit(eFlags::Position);
        m_pos += v;
    }

    void subPos(const EGG::Vector3f &v) {
        m_flags.setBit(eFlags::Position);
        m_pos -= v;
    }

    void setScale(const EGG::Vector3f &scale) {
        m_flags.setBit(eFlags::Scale);
        m_scale = scale;
    }

    void setScale(f32 scale) {
        m_flags.setBit(eFlags::Scale);
        m_scale.set(scale);
    }

    void setRot(const EGG::Vector3f &rot) {
        m_flags.setBit(eFlags::Rotation);
        m_rot = rot;
    }

    void setRotNoFlag(const EGG::Vector3f &rot) {
        m_rot = rot;
    }

    void addRot(const EGG::Vector3f &v) {
        m_rotLock = true;
        m_flags.setBit(eFlags::Rotation);
        m_rot += v;
    }

    void subRot(const EGG::Vector3f &v) {
        m_rotLock = true;
        m_flags.setBit(eFlags::Rotation);
        m_rot -= v;
    }

    /// @addr{0x806C296C}
    void setTransform(const EGG::Matrix34f &mat) {
        m_rotLock = false;
        m_flags.setBit(eFlags::Matrix);
        m_pendingRot = PendingRotation::None;
        m_transform = mat;
        m_pos = mat.base(3);
    }

    [[nodiscard]] const EGG::Vector3f &pos() const {
        return m_pos;
    }

    [[nodiscard]] const EGG::Vector3f &scale() const {
        return m_scale;
    }

    [[nodiscard]] const EGG::Vector3f &rot() const {
        return m_rot;
    }

    [[nodiscard]] const EGG::Matrix34f &transform() {
        resolvePendingRotation();
        return m_transform;
    }

protected:
    void calcTransform();
    void calcRotLock();
    void linkAnims(const std::span<const char *> &names, const std::span<Render::AnmType> types);
    void setMatrixTangentTo(const EGG::Vector3f &up, const EGG::Vector3f &tangent);
    void setMatrixFromOrthonormalBasisAndPos(const EGG::Vector3f &v);

    [[nodiscard]] static f32 CheckPointAgainstLineSegment(const EGG::Vector3f &point,
            const EGG::Vector3f &a, const EGG::Vector3f &b);
    [[nodiscard]] static EGG::Vector3f RotateXZByYaw(f32 angle, const EGG::Vector3f &v);
    [[nodiscard]] static EGG::Vector3f RotateAxisAngle(f32 angle, const EGG::Vector3f &axis,
            const EGG::Vector3f &v1);
    static void SetRotTangentHorizontal(EGG::Matrix34f &mat, const EGG::Vector3f &up,
            const EGG::Vector3f &tangent);
    [[nodiscard]] static EGG::Matrix34f OrthonormalBasis(const EGG::Vector3f &v);
    [[nodiscard]] static EGG::Vector3f AdjustVecForward(f32 sidewaysScalar, f32 forwardScalar,
            f32 minSpeed, const EGG::Vector3f &src, EGG::Vector3f forward);

    /// @addr{0x806B59A8}
    /// @brief Solves the standard kinematic equation \f$y(t) = v_0\, t - \frac{1}{2} a t^{2}\f$
    [[nodiscard]] static f32 CalcParabolicDisplacement(f32 initVel, f32 accel, u32 frame) {
        f32 t = static_cast<f32>(frame);
        return initVel * t - t * (0.5f * accel * t);
    }

    Render::DrawMdl *m_drawMdl;
    Abstract::g3d::ResFile *m_resFile;
    ObjectId m_id;
    RailInterpolator *m_railInterpolator;
    BoxColUnit *m_boxColUnit;
    const System::MapdataGeoObj *m_mapObj;

private:
    /// @brief The source of a rotation for @ref m_transform which has not been computed yet
    /// @details Kinoko-specific optimization. Many objects recompute their rotation every frame,
    /// but it is usually only read when checking collision against the kart, so the rotation
    /// columns of @ref m_transform are only computed when the matrix is next read.
    enum class PendingRotation {
        None,             ///< @ref m_transform is up-to-date
        TangentTo,        ///< @ref SetRotTangentHorizontal() with the pending up and tangent
        OrthonormalBasis, ///< @ref OrthonormalBasis() of the pending forward vector
    };

    void resolvePendingRotation();

    Flags m_flags;
    EGG::Vector3f m_pos;
    EGG::Vector3f m_scale;
    EGG::Vector3f m_rot;
    bool m_rotLock;
    EGG::Matrix34f m_transform;

    PendingRotation m_pendingRot; ///< Rotation of @ref m_transform yet to be computed
    EGG::Vector3f m_pendingWork0; ///< Up/forward vector depending on the @ref PendingRotation type
    EGG::Vector3f m_pendingWork1; ///< Tangent vector of the @ref PendingRotation
};

} // namespace Kinoko::Field
