#include "ObjectBase.hh"

#include "game/field/BoxColManager.hh"
#include "game/field/ObjectDirector.hh"
#include "game/field/RailInterpolator.hh"

#include "game/render/DrawMdl.hh"

#include "game/system/CourseMap.hh"
#include "game/system/ResourceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

#include <abstract/g3d/ResFile.hh>

#include <cstring>

namespace Kinoko::Field {

/// @addr{0x8081F828}
/// @brief Constructor
/// @param params The parameters used to initialize the object
/// @details Initializes the draw model, resource file, and rail interpolator to `nullptr`. Sets the
/// object's @ref m_id based on the ID from the provided parameters. Caches a pointer to the
/// provided params and stores it in @ref m_mapObj for later access. Fetches the object's @ref
/// m_pos, @ref m_scale, and @ref m_rot from the provided parameters. Sets @ref m_rotUpdated to
/// `true` to indicate @ref m_rot is up-to-date. Initializes @ref m_transform to the identity
/// matrix. Finally, sets the position, rotation, and scale flags in @ref m_flags so that the
/// object's transformation matrix is updated on the next @ref calcTransform() call.
ObjectBase::ObjectBase(const System::MapdataGeoObj &params)
    : m_drawMdl(nullptr),
      m_resFile(nullptr),
      m_id(static_cast<ObjectId>(params.id())),
      m_railInterpolator(nullptr),
      m_mapObj(&params),
      m_pos(params.pos()),
      m_scale(params.scale()),
      m_rot(params.rot() * DEG2RAD),
      m_rotUpdated(true),
      m_transform(EGG::Matrix34f::ident),
      m_pendingRot(PendingRotation::None),
      m_pendingWork0(EGG::Vector3f::zero),
      m_pendingWork1(EGG::Vector3f::zero) {
    m_flags.setBit(eFlags::Position, eFlags::Rotation, eFlags::Scale);
}

/// @addr{0x8081FB04}
/// @brief Overloaded constructor that creates the object associated with the provided name
/// @param name The name of the object
/// @param pos The initial position of the object
/// @param rot The initial rotation of the object
/// @param scale The initial scale of the object
/// @details Initializes the draw model, resource file, and rail interpolator to `nullptr`. Sets
/// @ref m_pos, @ref m_scale, and @ref m_rot based on the provided parameters. Sets @ref
/// m_rotUpdated to `true` to indicate @ref m_rot is up-to-date. Initializes @ref m_transform to the
/// identity matrix. Sets the position, rotation, and scale flags in @ref m_flags so that the
/// object's transformation matrix is updated on the next @ref calcTransform() call. Finally, sets
/// the object's @ref m_id based on the ID fetched from the provided name.
ObjectBase::ObjectBase(const char *name, const EGG::Vector3f &pos, const EGG::Vector3f &rot,
        const EGG::Vector3f &scale)
    : m_drawMdl(nullptr),
      m_resFile(nullptr),
      m_id(ObjectDirector::Instance()->flowTable().getIdFromName(name)),
      m_railInterpolator(nullptr),
      m_mapObj(nullptr),
      m_pos(pos),
      m_scale(scale),
      m_rot(rot),
      m_rotUpdated(true),
      m_transform(EGG::Matrix34f::ident),
      m_pendingRot(PendingRotation::None),
      m_pendingWork0(EGG::Vector3f::zero),
      m_pendingWork1(EGG::Vector3f::zero) {
    m_flags.setBit(eFlags::Position, eFlags::Rotation, eFlags::Scale);
}

/// @addr{0x8067E3C4}
/// @brief Virtual destructor that deletes the @ref Abstract::g3d::ResFile, @ref
/// Render::DrawMdl, and @ref RailInterpolator instances associated with the object
ObjectBase::~ObjectBase() {
    EGG::egg_delete(m_resFile);
    EGG::egg_delete(m_drawMdl);
    EGG::egg_delete(m_railInterpolator);
}

/// @addr{0x8081FC68}
/// @brief Run once during race initialization
void ObjectBase::init() {}

/// @addr{0x806BF448}
/// @brief Runs once per frame if the @ref eLoadFlags::Calc flag is set in @ref loadFlags()
void ObjectBase::calc() {}

/// @addr{0x808217B8}
/// @brief Updates the object's model
/// @details The base implementation simply calls @ref calcTransform().
void ObjectBase::calcModel() {
    calcTransform();
}

/// @addr{0x8081FD10}
/// @brief Loads the resources associated with the object, if any
void ObjectBase::loadGraphics() {
    const char *name = getResources();
    if (strcmp(name, "-") == 0) {
        return;
    }

    char filename[128];
    snprintf(filename, sizeof(filename), "%s.brres", name);

    auto *resMgr = System::ResourceManager::Instance();
    const void *resFile = resMgr->getFile(filename, nullptr, System::ArchiveId::Course);
    if (resFile) {
        m_resFile = EGG::egg_new<Abstract::g3d::ResFile>(resFile);
        m_drawMdl = EGG::egg_new<Render::DrawMdl>();
    }
}

/// @addr{0x8082072C}
/// @brief Links associated animations from the object's resource file to the @ref
/// Render::DrawMdl, if any
void ObjectBase::loadAnims() {}

/// @addr{0x80820980}
/// @brief Loads the rail interpolator for the object
/// @details The associated rail is identified based on @ref System::MapdataGeoObj::pathId(). If
/// this is `-1`, then the object does not have a rail and the function early returns, leaving @ref
/// m_railInterpolator as `nullptr`. If the path's @ref System::MapdataPointInfo setting 1 is set to
/// zero, then constructs a @ref RailLinearInterpolator; otherwise, constructs a @ref
/// RailSmoothInterpolator.
void ObjectBase::loadRail() {
    if (!m_mapObj) {
        return;
    }

    s16 pathId = m_mapObj->pathId();

    if (pathId == -1) {
        return;
    }

    auto *point = System::CourseMap::Instance()->getPointInfo(pathId);
    f32 speed = static_cast<f32>(m_mapObj->setting(0));

    if (point->setting(0) == 0) {
        m_railInterpolator = EGG::egg_new<RailLinearInterpolator>(speed, pathId);
    } else {
        m_railInterpolator = EGG::egg_new<RailSmoothInterpolator>(speed, pathId);
    }
}

/// @addr{0x80680730}
/// @brief Fetches the name of the `brres` resource file associated with the object, if any
/// @return The name of the resource file, or `"-"` if none is associated
const char *ObjectBase::getResources() const {
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto *collisionSet = flowTable.set(flowTable.slot(id()));
    ASSERT(collisionSet);
    return collisionSet->resources;
}

/// @addr{0x80680784}
/// @brief Fetches the name of the object from the @ref ObjectFlowTable
/// @return The name of the object
const char *ObjectBase::getName() const {
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto *collisionSet = flowTable.set(flowTable.slot(id()));
    ASSERT(collisionSet);
    return collisionSet->name;
}

/// @addr{0x806BF434}
/// @brief Returns a bitmask indicating which lifecycle hooks should be called for the object
/// @return No flags set
/// @details In Kinoko, the only effective behavior of this bitmask is to determine whether
/// calc() should be called. The other bits are unused.
ObjectBase::LoadFlags ObjectBase::loadFlags() const {
    // TODO: This references LOD to determine load flags
    return LoadFlags();
}

/// @addr{0x806806DC}
/// @brief Fetches the name of the KCL resource associated with the object from the @ref
/// ObjectFlowTable
/// @return The name of the KCL resource associated with the object
const char *ObjectBase::getKclName() const {
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto *collisionSet = flowTable.set(flowTable.slot(id()));
    ASSERT(collisionSet);
    return collisionSet->resources;
}

/// @addr{0x80681598}
/// @brief The position of the object in world space
/// @return The position of the object in world space
const EGG::Vector3f &ObjectBase::getPosition() const {
    return m_pos;
}

/// @addr{0x8080BDC0}
/// @brief Collision radius to use for GJK checks
/// @return Default collision radius of `100.0f`
f32 ObjectBase::getCollisionRadius() const {
    return 100.0f;
}

/// @addr{0x80572574}
/// @brief Fetches the unique identifier of the object
/// @return The unique identifier of the object, @ref m_id
ObjectId ObjectBase::id() const {
    return m_id;
}

/// @addr{0x80821DB8}
/// @brief Resizes the @ref BoxColUnit associated with the object
/// @param radius The new radius of the collision box
/// @param maxSpeed The maximum speed for the collision box
/// @details This function is virtual in the base game, but no derived classes override it, so
/// we can devirtualize in Kinoko. This does not affect the actual collision geometry of the
/// object, only the BoxColUnit used for spatially indexed collision queries. This effectively
/// adjusts how close the player needs to be to the object before collision checks are
/// performed.
void ObjectBase::resize(f32 radius, f32 maxSpeed) {
    m_boxColUnit->resize(radius, maxSpeed);
}

/// @addr{0x80821DD8}
/// @brief Unregisters the @ref BoxColUnit associated with the object from the @ref BoxColManager
/// @details This function is virtual in the base game, but no derived classes override it, so
/// we can devirtualize in Kinoko.
void ObjectBase::unregisterCollision() {
    BoxColManager::Instance()->remove(m_boxColUnit);
}

/// @addr{0x80821DEC}
/// @brief Disables collision checks for this object in the @ref BoxColManager
/// @details This function is virtual in the base game, but no derived classes override it, so
/// we can devirtualize in Kinoko.
void ObjectBase::disableCollision() const {
    m_boxColUnit->m_flag.setBit(eBoxColFlag::Intangible);
}

/// @addr{0x80821E00}
/// @brief Re-enables collision checks for this object in the @ref BoxColManager
/// @details This function is virtual in the base game, but no derived classes override it, so
/// we can devirtualize in Kinoko.
void ObjectBase::enableCollision() const {
    m_boxColUnit->m_flag.resetBit(eBoxColFlag::Intangible);
}

/// @addr{0x80821640}
/// @brief Updates the transform matrix of the object based on its position and rotation
void ObjectBase::calcTransform() {
    if (m_flags.onBit(eFlags::Rotation)) {
        m_pendingRot = PendingRotation::None;
        m_transform.makeRT(m_rot, m_pos);
        m_flags.resetBit(eFlags::Rotation, eFlags::Position);
    } else if (m_flags.onBit(eFlags::Position)) {
        m_transform.setBase(3, m_pos);
        m_flags.setBit(eFlags::Matrix);
    }
}

/// @brief Refreshes the object's rotation vector based on its transform matrix
/// @details This is used when the object's transform matrix is modified directly, such as via
/// @ref setMatrixTangentTo().
void ObjectBase::calcRotLock() {
    if (!m_rotUpdated) {
        m_rotUpdated = true;
        resolvePendingRotation();
        m_rot = m_transform.calcRPY();
    }
}

/// @addr{0x80820EB8}
/// @brief Links animations from the object's resource file to the @ref Render::DrawMdl
/// @param names The names of the animations to link
/// @param types The types of the animations to link
void ObjectBase::linkAnims(const std::span<const char *> &names,
        const std::span<Render::AnmType> types) {
    if (!m_drawMdl) {
        return;
    }

    ASSERT(names.size() == types.size());

    for (size_t i = 0; i < names.size(); ++i) {
        m_drawMdl->linkAnims(i, m_resFile, names[i], types[i]);
    }
}

/// @addr{0x80821910}
/// @brief Sets the object's transformation matrix based on an up vector and a tangent vector
/// @param up The up vector to align the matrix with
/// @param tangent The tangent vector to align the matrix with
void ObjectBase::setMatrixTangentTo(const EGG::Vector3f &up, const EGG::Vector3f &tangent) {
    m_rotUpdated = false;
    m_flags.setBit(eFlags::Matrix);

    // Kinoko-specific: defer computing the rotation until the transform is read
    m_pendingRot = PendingRotation::TangentTo;
    m_pendingWork0 = up;
    m_pendingWork1 = tangent;
    m_transform.setBase(3, m_pos);
}

/// @addr{0x808218B0}
/// @brief Sets the transformation matrix based on an orthonormal basis and the object's position
/// @param v The vector to construct an orthonormal basis from
void ObjectBase::setMatrixFromOrthonormalBasisAndPos(const EGG::Vector3f &v) {
    m_flags.setBit(eFlags::Matrix);

    // Kinoko-specific: defer computing the rotation until the transform is read
    m_pendingRot = PendingRotation::OrthonormalBasis;
    m_pendingWork0 = v;
    m_transform.setBase(3, m_pos);
}

/// @brief Computes the rotation columns of @ref m_transform from the @ref PendingRotation, if any
/// @details This produces the same rotation as computing it immediately, since the rotation only
/// depends on the saved up and tangent vectors, and all other writes to @ref m_transform before
/// this point either only touch the translation column or discard the pending rotation.
void ObjectBase::resolvePendingRotation() {
    switch (m_pendingRot) {
    case PendingRotation::None:
        return;
    case PendingRotation::TangentTo:
        SetRotTangentHorizontal(m_transform, m_pendingWork0, m_pendingWork1);
        break;
    case PendingRotation::OrthonormalBasis: {
        EGG::Matrix34f basis = OrthonormalBasis(m_pendingWork0);
        m_transform.setBase(0, basis.base(0));
        m_transform.setBase(1, basis.base(1));
        m_transform.setBase(2, basis.base(2));
    } break;
    }

    m_pendingRot = PendingRotation::None;
}

/// @addr{0x806B41E0}
/// @brief Sets the rotation of a matrix based on an up vector and a tangent vector
/// @param mat The matrix to set the rotation for
/// @param up The up vector to align the matrix with
/// @param tangent The tangent vector to align the matrix with
void ObjectBase::SetRotTangentHorizontal(EGG::Matrix34f &mat, const EGG::Vector3f &up,
        const EGG::Vector3f &tangent) {
    EGG::Vector3f vec = tangent - up * tangent.dot(up);
    vec.normalise2();

    mat.setBase(0, up.cross(vec));
    mat.setBase(1, up);
    mat.setBase(2, vec);
}

/// @addr{0x806B38A8}
/// @brief Calculates on what side of line segment `ab` point lies.
/// @param point The point to check against the line segment
/// @param a The starting point of the line segment
/// @param b The ending point of the line segment
/// @return A positive value if point is on the left side of the line segment, negative if on the
/// right side, and 0 if on the line segment.
f32 ObjectBase::CheckPointAgainstLineSegment(const EGG::Vector3f &point, const EGG::Vector3f &a,
        const EGG::Vector3f &b) {
    return (b.x - a.x) * (point.z - a.z) - (point.x - a.x) * (b.z - a.z);
}

/// @addr{0x806B3900}
/// @brief Rotates a vector around the Y-axis and returns the XZ-plane portion of the vector.
/// @param angle The angle to rotate the vector by
/// @param v The vector to rotate
/// @return The rotated vector in the XZ-plane
EGG::Vector3f ObjectBase::RotateXZByYaw(f32 angle, const EGG::Vector3f &v) {
    f32 y = EGG::Mathf::SinFIdx(RAD2FIDX * (0.5f * angle));
    f32 w = EGG::Mathf::CosFIdx(RAD2FIDX * (0.5f * angle));
    EGG::Quatf quat = EGG::Quatf(w, 0.0f, y, 0.0f);
    return quat.rotateVector(EGG::Vector3f(v.x, 0.0f, v.z));
}

/// @addr{0x806B3AC4}
/// @brief Rotates a vector around an arbitrary axis by a given angle
/// @param angle The angle to rotate the vector by
/// @param axis The axis to rotate the vector around
/// @param v1 The vector to rotate
/// @return The rotated vector
EGG::Vector3f ObjectBase::RotateAxisAngle(f32 angle, const EGG::Vector3f &axis,
        const EGG::Vector3f &v1) {
    EGG::Matrix34f mat;
    mat.setBase(3, EGG::Vector3f::zero);
    mat.setAxisRotation(angle, axis);
    return mat.ps_multVector(v1);
}

/// @addr{0x806B3CA4}
/// @brief Creates an orthonormal basis from a given vector
/// @param v The vector to construct the orthonormal basis from
/// @return The resulting orthonormal basis as a matrix
EGG::Matrix34f ObjectBase::OrthonormalBasis(const EGG::Vector3f &v) {
    EGG::Vector3f z = v;

    if (EGG::Mathf::abs(z.y) < 0.001f) {
        z.y = 0.001f;
    }

    EGG::Vector3f h = EGG::Vector3f(v.x, 0.0f, v.z);
    h.normalise2();

    EGG::Vector3f x = (z.y > 0.0f) ? -h.cross(z) : h.cross(z);
    x.normalise2();

    EGG::Matrix34f mat;
    mat.setBase(3, EGG::Vector3f::zero);
    mat.setBase(0, x);
    mat.setBase(1, z.cross(x));
    mat.setBase(2, z);

    return mat;
}

/// @addr{0x807DE934}
/// @brief Adjusts a vector based on sideways and forward scalars, ensuring a minimum magnitude
/// @param sidewaysScalar The scalar to apply to the sideways component of the vector
/// @param forwardScalar The scalar to apply to the forward component of the vector
/// @param minSpeed The minimum allowed magnitude for the forward component
/// @param src The vector to adjust
/// @param forward The forward direction vector
/// @return The scaled vector
EGG::Vector3f ObjectBase::AdjustVecForward(f32 sidewaysScalar, f32 forwardScalar, f32 minSpeed,
        const EGG::Vector3f &src, EGG::Vector3f forward) {
    if (forward.y > 0.0f) {
        forward.y = 0.0f;
        auto [mag, tmp] = forward.ps_normalized();

        if (mag <= 0.0f) {
            return src;
        }

        forward = tmp;
    }

    EGG::Vector3f proj = forward * src.ps_dot(forward);
    EGG::Vector3f sideways = (src - proj) * sidewaysScalar;

    EGG::Vector3f newForward = proj * -forwardScalar;
    if (newForward.squaredLength() < minSpeed * minSpeed) {
        newForward = forward * minSpeed;
    }

    return sideways + newForward;
}

/// @addr{0x806B59A8}
/// @brief Solves the standard kinematic equation \f$y(t) = v_0\, t - \frac{1}{2} a t^{2}\f$
/// @param initVel The initial velocity
/// @param accel The acceleration
/// @param frame The frame number
/// @return The displacement at the given frame
f32 ObjectBase::CalcParabolicDisplacement(f32 initVel, f32 accel, u32 frame) {
    f32 t = static_cast<f32>(frame);
    return initVel * t - t * (0.5f * accel * t);
}

} // namespace Kinoko::Field
