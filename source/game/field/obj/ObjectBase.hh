#pragma once

#include "game/field/obj/ObjectId.hh"

#include <egg/core/BitFlag.hh>
#include <egg/math/Matrix.hh>

namespace Kinoko {

namespace Abstract::g3d {

class ResFile;

} // namespace Abstract::g3d

namespace Render {

enum class AnmType;
class DrawMdl;

} // namespace Render

namespace System {

class MapdataGeoObj;

} // namespace System

namespace Field {

class BoxColUnit;
class RailInterpolator;

/// @brief Abstract base class for all objects in the game
/// @details Objects maintain position, rotation, and scale vectors. Position and rotation are
/// combined into a 3x4 transformation matrix via @ref calcTransform(). Each object manages its own
/// @ref BoxColUnit (if it has collision) for GJK collision checks. If the object moves along a
/// defined path, it has a @ref RailInterpolator that evaluates the object's position along the rail
/// path. The class provides lifecycle hooks (@ref init() / @ref calc() / @ref load() / @ref
/// createCollision()) implemented by subclasses and virtual methods for collision/resource
/// management and object physics updates.
/// @note OPT: For Kinoko, we optimize performance by deferring updates to @ref m_transform until it
/// is actually fetched. This prevents expensive matrix operations from being performed
/// unnecessarily.
class ObjectBase {
public:
    /// @brief Tracks what properties have changed and require recalculation in the transform matrix
    enum class eFlags {
        Position = 0, ///< The object's position has changed
        Rotation = 1, ///< The object's rotation has changed
        Matrix = 2,   ///< The object's transformation matrix has changed
        Scale = 3,    ///< The object's scale has changed
    };

    /// @brief A bitfield of @ref eFlags that represents the state of the object
    typedef EGG::TBitFlag<u8, eFlags> Flags;

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

    virtual void init();
    virtual void calc();
    virtual void calcModel();
    virtual void loadGraphics();
    virtual void loadAnims();
    virtual void loadRail();
    [[nodiscard]] virtual const char *getResources() const;
    [[nodiscard]] virtual const char *getName() const;
    [[nodiscard]] virtual LoadFlags loadFlags() const;
    [[nodiscard]] virtual const char *getKclName() const;
    [[nodiscard]] virtual const EGG::Vector3f &getPosition() const;
    [[nodiscard]] virtual f32 getCollisionRadius() const;
    [[nodiscard]] virtual ObjectId id() const;

    /// @brief Loads resources, collision, and registers the object to the @ref ObjectDirector
    virtual void load() = 0;

    /// @brief Creates a collision object that inherits from @ref ObjectCollisionBase
    virtual void createCollision() = 0;

    /// @brief Updates the position, rotation, and scale of the collision object
    virtual void calcCollisionTransform() = 0;

    void resize(f32 radius, f32 maxSpeed);
    void unregisterCollision();
    void disableCollision() const;
    void enableCollision() const;

    /// @beginSetters

    /// @brief Sets the position of the object in world space and updates the position matrix flag
    /// @param pos The new position of the object in world space
    void setPos(const EGG::Vector3f &pos) {
        m_flags.setBit(eFlags::Position);
        m_pos = pos;
    }

    /// @brief Adds a vector to the object's position and updates the position matrix flag
    /// @param v The vector to add to the object's position
    void addPos(const EGG::Vector3f &v) {
        m_flags.setBit(eFlags::Position);
        m_pos += v;
    }

    /// @brief Subtracts a vector from the object's position and updates the position matrix flag
    /// @param v The vector to subtract from the object's position
    void subPos(const EGG::Vector3f &v) {
        m_flags.setBit(eFlags::Position);
        m_pos -= v;
    }

    /// @brief Sets the scale of the object and updates the scale flag
    /// @param scale The new scale of the object
    void setScale(const EGG::Vector3f &scale) {
        m_flags.setBit(eFlags::Scale);
        m_scale = scale;
    }

    /// @brief Sets the uniform scale of the object and updates the scale flag
    /// @param scale The new uniform scale of the object
    void setScale(f32 scale) {
        m_flags.setBit(eFlags::Scale);
        m_scale.set(scale);
    }

    /// @brief Sets the rotation of the object and updates the rotation flag
    /// @param rot The new rotation of the object
    void setRot(const EGG::Vector3f &rot) {
        m_flags.setBit(eFlags::Rotation);
        m_rot = rot;
    }

    /// @brief Sets the rotation of the object without updating the rotation flag
    /// @param rot The new rotation of the object
    void setRotNoFlag(const EGG::Vector3f &rot) {
        m_rot = rot;
    }

    /// @brief Adds a vector to the object's rotation and updates the rotation flag
    /// @param v The vector to add to the object's rotation
    void addRot(const EGG::Vector3f &v) {
        m_rotUpdated = true;
        m_flags.setBit(eFlags::Rotation);
        m_rot += v;
    }

    /// @brief Subtracts a vector from the object's rotation and updates the rotation flag
    /// @param v The vector to subtract from the object's rotation
    void subRot(const EGG::Vector3f &v) {
        m_rotUpdated = true;
        m_flags.setBit(eFlags::Rotation);
        m_rot -= v;
    }

    /// @addr{0x806C296C}
    /// @brief Sets the transformation matrix of the object and updates the matrix flag
    /// @param mat The new transformation matrix of the object
    void setTransform(const EGG::Matrix34f &mat) {
        m_rotUpdated = false;
        m_flags.setBit(eFlags::Matrix);
        m_pendingRot = PendingRotation::None;
        m_transform = mat;
        m_pos = mat.base(3);
    }

    /// @endSetters

    /// @beginGetters

    /// @brief Gets the @ref RailInterpolator associated with the object
    /// @return The @ref RailInterpolator for the object, @ref m_railInterpolator
    [[nodiscard]] const RailInterpolator *railInterpolator() const {
        return m_railInterpolator;
    }

    /// @brief Gets the position of the object in world space
    /// @return The position of the object in world space, @ref m_pos
    [[nodiscard]] const EGG::Vector3f &pos() const {
        return m_pos;
    }

    /// @brief Gets the scale of the object
    /// @return The scale of the object, @ref m_scale
    [[nodiscard]] const EGG::Vector3f &scale() const {
        return m_scale;
    }

    /// @brief Gets the rotation of the object
    /// @return The rotation of the object, @ref m_rot
    [[nodiscard]] const EGG::Vector3f &rot() const {
        return m_rot;
    }

    /// @brief Gets the transformation matrix of the object
    /// @return The transformation matrix of the object, @ref m_transform
    [[nodiscard]] const EGG::Matrix34f &transform() {
        resolvePendingRotation();
        return m_transform;
    }

    /// @addr{0x80680618}
    /// @brief Fetches the @ref BoxColUnit associated with the object
    /// @return The @ref BoxColUnit associated with the object, @ref m_boxColUnit
    /// @details This function is virtual in the base game, but no derived classes override it, so
    /// we can devirtualize in Kinoko.
    [[nodiscard]] const BoxColUnit *unit() const {
        return m_boxColUnit;
    }

    /// @endGetters

protected:
    void calcTransform();
    void linkAnims(const std::span<const char *> &names, const std::span<Render::AnmType> types);
    void calcRotLock();
    void setMatrixTangentTo(const EGG::Vector3f &up, const EGG::Vector3f &tangent);
    void setMatrixFromOrthonormalBasisAndPos(const EGG::Vector3f &v);

    static void SetRotTangentHorizontal(EGG::Matrix34f &mat, const EGG::Vector3f &up,
            const EGG::Vector3f &tangent);
    [[nodiscard]] static f32 CheckPointAgainstLineSegment(const EGG::Vector3f &point,
            const EGG::Vector3f &a, const EGG::Vector3f &b);
    [[nodiscard]] static EGG::Vector3f RotateXZByYaw(f32 angle, const EGG::Vector3f &v);
    [[nodiscard]] static EGG::Vector3f RotateAxisAngle(f32 angle, const EGG::Vector3f &axis,
            const EGG::Vector3f &v1);
    [[nodiscard]] static EGG::Matrix34f OrthonormalBasis(const EGG::Vector3f &v);
    [[nodiscard]] static EGG::Vector3f AdjustVecForward(f32 sidewaysScalar, f32 forwardScalar,
            f32 minSpeed, const EGG::Vector3f &src, EGG::Vector3f forward);
    [[nodiscard]] static f32 CalcParabolicDisplacement(f32 initVel, f32 accel, u32 frame);

    Render::DrawMdl *m_drawMdl;           ///< The draw model associated with the object
    Abstract::g3d::ResFile *m_resFile;    ///< The resource file associated with the object
    const ObjectId m_id;                  ///< The ID of the object
    RailInterpolator *m_railInterpolator; ///< The rail interpolator associated with the object
    BoxColUnit *m_boxColUnit;             ///< The box collision unit associated with the object
    const System::MapdataGeoObj *const m_mapObj; ///< The KMP object associated with the object

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

    Flags m_flags;              ///< The properties of the transform matrix to be recalculated
    EGG::Vector3f m_pos;        ///< The position of the object
    EGG::Vector3f m_scale;      ///< The scale of the object
    EGG::Vector3f m_rot;        ///< The rotation of the object (in radians)
    bool m_rotUpdated;          ///< Whether @ref m_rot is up-to-date
    EGG::Matrix34f m_transform; ///< The 3x4 matrix representing the object's position and rotation

    PendingRotation m_pendingRot; ///< Rotation of @ref m_transform yet to be computed
    EGG::Vector3f m_pendingWork0; ///< Up/forward vector depending on the @ref PendingRotation type
    EGG::Vector3f m_pendingWork1; ///< Tangent vector of the @ref PendingRotation
};

} // namespace Field

} // namespace Kinoko
