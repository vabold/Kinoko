#include "ObjectHanachan.hh"

#include "game/field/ObjectCollisionSphere.hh"
#include "game/field/ObjectDirector.hh"
#include "game/field/RailInterpolator.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x806F2FE8}
/// @brief Constructor
/// @param linkDistances The distances between consecutive links in the chain
/// @details Constructs an array of @ref SphereLink objects for each segment in the chain and
/// initializes their link lengths based on the provided distances, as well as setting the links'
/// next and prev pointers.
HanachanChainManager::HanachanChainManager(const std::span<const f32> &linkDistances) {
    size_t count = linkDistances.size() + 1;

    m_links = owning_span<Field::SphereLink>(count);
    m_links[0].initLinkLen(linkDistances[0]);

    for (size_t i = 1; i < count - 1; ++i) {
        m_links[i].initLinkLen(linkDistances[i]);
        m_links[i - 1].setNext(&m_links[i]);
        m_links[i].setPrev(&m_links[i - 1]);
    }

    auto &last = m_links.back();
    auto &secondToLast = m_links[count - 2];
    last.initLinkLen(0.0f);
    secondToLast.setNext(&last);
    last.setPrev(&secondToLast);
}

/// @addr{0x806F31F4}
/// @brief Default destructor
HanachanChainManager::~HanachanChainManager() = default;

/// @addr{0x806F3370}
/// @brief Initializes all @ref SphereLink objects in the chain
void HanachanChainManager::init() {
    for (auto &link : m_links) {
        link.init();
    }
}

/// @addr{0x806F49BC}
/// @brief Applies spring force and updates positions for all links in the chain, enforcing
/// constraints when the chain is taut. Also checks for floor collision.
/// @details Runs two iterations of the spring force and position calculations to reduce oscillation
/// and prevent links from stretching past their maximum length.
void HanachanChainManager::calc() {
    for (u32 i = 0; i < 2; ++i) {
        for (auto &link : m_links) {
            link.calc();
        }

        for (auto &link : m_links) {
            link.calcPos();
        }
    }

    for (size_t i = 0; i < m_links.size() - 1; ++i) {
        EGG::Vector3f delta = m_links[i].pos() - m_links[i + 1].pos();
        f32 len = m_links[i].linkLen();

        if (delta.squaredLength() - len * len > 0.0f) {
            calcConstraints();
            break;
        }
    }

    for (auto &link : m_links) {
        link.checkCollision();
    }
}

/// @addr{0x806F5290}
/// @brief Calculates the constraints for all SphereLink objects in the chain, ensuring that
/// they do not stretch beyond their maximum link length
void HanachanChainManager::calcConstraints() {
    for (size_t i = 1; i < m_links.size(); ++i) {
        m_links[i].calcConstraints(1.0f);
    }
}

/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
ObjectHanachanPart::ObjectHanachanPart(const System::MapdataGeoObj &params)
    : ObjectCollidable(params) {}

/// @copybrief ObjectCollidable::ObjectCollidable(const char *, const EGG::Vector3f &,
/// const EGG::Vector3f &, const EGG::Vector3f &)
/// @param name The name of the object
/// @param pos The position of the object
/// @param rot The rotation of the object
/// @param scale The scale of the object
ObjectHanachanPart::ObjectHanachanPart(const char *name, const EGG::Vector3f &pos,
        const EGG::Vector3f &rot, const EGG::Vector3f &scale)
    : ObjectCollidable(name, pos, rot, scale) {}

/// @addr{0x806C7E68}
/// @brief Default virtual destructor
ObjectHanachanPart::~ObjectHanachanPart() = default;

/// @addr{0x806CCACC}
/// @copybrief ObjectBase::loadRail()
/// @details no-op because the head and body segments are managed by the main @ref
/// ObjectHanachan class.
void ObjectHanachanPart::loadRail() {}

/// @addr{0x806CCAD0}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectHanachanPart::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806CCB88}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the Wiggler head, `hanachan`
const char *ObjectHanachanPart::getKclName() const {
    return "hanachan";
}

/// @addr{0x806CA5E4}
/// @brief Sets the transform matrix of the object based on its position, up vector, and
/// tangent
/// @param pos The position of the object
/// @param up The up vector of the object
/// @param tangent The tangent vector of the object
void ObjectHanachanPart::calcTransformFromUpAndTangent(const EGG::Vector3f &pos,
        const EGG::Vector3f &up, const EGG::Vector3f &tangent) {
    EGG::Matrix34f mat;
    SetRotTangentHorizontal(mat, up, tangent);
    mat.setBase(3, pos);
    setTransform(mat);
}

/// @addr{0x806C7D74}
/// @copydoc ObjectHanachanPart::ObjectHanachanPart(const char *, const EGG::Vector3f &,
/// const EGG::Vector3f &, const EGG::Vector3f &)
/// @details Initializes @ref m_lastPos to the zero vector.
ObjectHanachanHead::ObjectHanachanHead(const char *name, const EGG::Vector3f &pos,
        const EGG::Vector3f &rot, const EGG::Vector3f &scale)
    : ObjectHanachanPart(name, pos, rot, scale), m_lastPos(EGG::Vector3f::zero) {}

/// @addr{0x806CCB94}
/// @brief Default virtual destructor
ObjectHanachanHead::~ObjectHanachanHead() = default;

/// @addr{0x806C818C}
/// @copybrief ObjectBase::createCollision()
/// @details Creates a collision sphere for the head segment with a radius of `150.0f` units,
/// centered at @ref collisionCenter().
void ObjectHanachanHead::createCollision() {
    m_collision = EGG::egg_new<ObjectCollisionSphere>(150.0f, collisionCenter());
}

/// @addr{0x806C8450}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details Applies a forward and upwards offset so that the collision sphere sits above and in
/// front of the head's origin. Updates the @ref ObjColMgr transformation matrix accordingly.
/// Finally, caches the head's position to @ref m_lastPos.
void ObjectHanachanHead::calcCollisionTransform() {
    calcTransform();

    EGG::Matrix34f trans = transform();
    EGG::Vector3f scaledUp = trans.base(1) * 330.0f * scale().y;
    EGG::Vector3f scaledForward = trans.base(2) * 100.0f * scale().y;
    trans.setBase(3, pos() + scaledUp + scaledForward);
    EGG::Vector3f speed = pos() - m_lastPos;

    m_collision->transform(trans, scale(), speed);
    m_lastPos = pos();
}

/// @brief Overloaded constructor
/// @param params The parameters used to initialize the object
/// @param mdlName The name of the model corresponding to this body segment
/// @details Caches the model name to @ref m_mdlName. Initializes @ref m_lastSegment to `false`
/// and @ref m_lastPos to the zero vector.
ObjectHanachanBody::ObjectHanachanBody(const System::MapdataGeoObj &params, const char *mdlName)
    : ObjectHanachanPart(params),
      m_mdlName(mdlName),
      m_lastSegment(false),
      m_lastPos(EGG::Vector3f::zero) {}

/// @brief Overloaded constructor
/// @param name The name of the object
/// @param pos The initial position of the object
/// @param rot The initial rotation of the object
/// @param scale The initial scale of the object
/// @param mdlName The name of the model corresponding to this body segment
/// @details Caches the model name to @ref m_mdlName. Initializes @ref m_lastSegment to `false`
/// and @ref m_lastPos to the zero vector.
ObjectHanachanBody::ObjectHanachanBody(const char *name, const EGG::Vector3f &pos,
        const EGG::Vector3f &rot, const EGG::Vector3f &scale, const char *mdlName)
    : ObjectHanachanPart(name, pos, rot, scale),
      m_mdlName(mdlName),
      m_lastSegment(false),
      m_lastPos(EGG::Vector3f::zero) {}

/// @addr{0x806CCAD8}
/// @brief Default virtual destructor
ObjectHanachanBody::~ObjectHanachanBody() = default;

/// @addr{0x806C8908}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details Uses base class implementation for all segments except the last. For the last segment,
/// applies a forward offset so that the collision sphere sits in front of the body part's origin.
void ObjectHanachanBody::calcCollisionTransform() {
    if (!m_lastSegment) {
        ObjectCollidable::calcCollisionTransform();
        return;
    }

    EGG::Matrix34f trans = transform();
    trans.setBase(3, pos() + trans.base(2) * 80.0f * scale().y);
    EGG::Vector3f posDelta = pos() - m_lastPos;

    m_collision->transform(trans, scale(), posDelta);
    m_lastPos = pos();
}

/// @addr{0x806CCB80}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the Wiggler body segment, `hanachan_body1`, `hanachan_body2`,
/// `hanachan_body3`, or `hanachan_body4`.
const char *ObjectHanachanBody::getKclName() const {
    return m_mdlName;
}

/// @addr{0x806C8A5C}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Constructs the @ref HanachanChainManager using body part distances defined by @ref
/// BODY_PART_DISTANCES. Initializes @ref m_walkSpeed based on param setting 1. Constructs the head
/// and body part objects and scales them up to 3x. After loading all objects, resizes all collision
/// spheres (except the head) to reflect the 3x scale. Finally, caches the distance between each
/// body part and the head to @ref m_partDisplacement.
ObjectHanachan::ObjectHanachan(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      StateManager(this, STATE_ENTRIES),
      m_chain(BODY_PART_DISTANCES),
      m_walkSpeed(static_cast<f32>(static_cast<s16>(params.setting(0)))) {
    constexpr f32 SCALE = 3.0f;
    constexpr EGG::Vector3f SCALE_VEC = EGG::Vector3f(SCALE, SCALE, SCALE);

    auto *&head = headPart();
    head = EGG::egg_new<ObjectHanachanHead>("BossHanachanHead", EGG::Vector3f::zero,
            EGG::Vector3f::ez, EGG::Vector3f::ez);
    head->setScale(SCALE_VEC);

    const auto &mdlNames = ObjectHanachanBody::MDL_NAMES;
    size_t mdlNameCount = ObjectHanachanBody::MDL_NAMES.size();
    auto parts = bodyParts();
    for (size_t i = 0; i < parts.size(); ++i) {
        auto *&part = parts[i];
        part = EGG::egg_new<ObjectHanachanBody>(params, mdlNames[i % mdlNameCount]);
        part->setScale(SCALE_VEC);
    }

    head->load();

    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    for (size_t i = 0; i < parts.size(); ++i) {
        auto *&part = parts[i];
        part->load();

        const auto *collisionSet = flowTable.set(flowTable.slot(part->id()));
        f32 radius = SCALE * static_cast<f32>(parse<s16>(collisionSet->params.sphere.radius));
        part->resize(radius, 0.0f);
    }

    m_partDisplacement[0] = 0.0f;
    for (size_t i = 1; i < m_partDisplacement.size(); ++i) {
        m_partDisplacement[i] = m_partDisplacement[i - 1] + BODY_PART_DISTANCES[i - 1];
    }
}

/// @addr{0x806C9598}
/// @brief Default virtual destructor
ObjectHanachan::~ObjectHanachan() = default;

/// @addr{0x806C9630}
/// @copybrief ObjectBase::init()
/// @details Initializes the Wiggler's rail, body, and chain. Sets the initial state variables,
/// including the sway amplitude, misalignment frame, and rail alignment. Marks the last body
/// segment.
void ObjectHanachan::init() {
    initRail();
    initBody();

    m_still = false;
    m_swayAmplitude = INIT_SWAY_AMPLITUDE;
    m_leftMisalignFrame = 0;
    m_prevRailTangent = EGG::Vector3f::ez;
    m_railAlignment = RailAlignment::Unknown;
    m_prevRailAlignment = RailAlignment::Unknown;

    reinterpret_cast<ObjectHanachanBody *>(m_parts.back())->m_lastSegment = true;

    initChain();
}

/// @addr{0x806C9860}
/// @copybrief ObjectBase::calc()
/// @details Updates the rail interpolator, evaluates the Wiggler's state machine, and updates
/// the Wiggler's body segments' positions.
void ObjectHanachan::calc() {
    calcRail();
    StateManager::calc();
    calcBody();
}

/// @addr{0x806CC9F8}
/// @copybrief ObjectBase::loadGraphics()
/// @details This is a no-op in the base game.
void ObjectHanachan::loadGraphics() {}

/// @addr{0x806CC9F4}
/// @copybrief ObjectBase::createCollision()
/// @details Does not create collision since the body parts have their own collision spheres
void ObjectHanachan::createCollision() {}

/// @addr{0x806CC9FC}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectHanachan::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806C9BC0}
/// @brief Runs once when the Wiggler begins walking
/// @details Sets the rail's velocity to @ref m_walkSpeed, sets @ref m_swayAmplitude to @ref
/// INIT_SWAY_AMPLITUDE, sets @ref m_still to `false`, and resets @ref m_leftMisalignFrame to
/// zero.
void ObjectHanachan::enterWalk() {
    setRailVel();
    m_swayAmplitude = INIT_SWAY_AMPLITUDE;
    m_still = false;
    m_leftMisalignFrame = 0;
}

/// @addr{0x806C9D38}
/// @brief Runs every frame when the Wiggler is walking along its rail
/// @details If the Wiggler should stop moving (designated by @ref m_still), then it sets the rail
/// interpolator's speed to zero and transitions to the waiting state. Regardless, it updates the
/// rail alignment, clears the chain, calculates the rail alignment motion, updates the chain, and
/// resets the sway amplitude to @ref INIT_SWAY_AMPLITUDE.
void ObjectHanachan::calcWalk() {
    if (m_still) {
        m_railInterpolator->setSpeed(0.0f);
        m_nextStateId = 1;
    }

    m_prevRailAlignment = m_railAlignment;
    m_railAlignment = calcRailAlignment();

    clearChain();
    calcRailAlignmentMotion();
    m_chain.calc();

    m_swayAmplitude = INIT_SWAY_AMPLITUDE;
}

/// @addr{0x806C9F98}
/// @brief Runs every frame when the Wiggler is standing still
/// @details If the Wiggler has been standing still for @ref m_stillDuration frames, then
/// transitions the Wiggler to the walking state. Otherwise, stops all body segments from
/// moving. Calculates the lateral sway of all body segments and updates their positions
/// accordingly.
void ObjectHanachan::calcWait() {
    if (shouldStartMoving()) {
        m_nextStateId = 0;
    }

    clearChain();
    calcSway();
    m_chain.calc();
}

/// @addr{0x806CA24C}
/// @brief Called when the Wiggler reaches the end of a rail segment
/// @details If the current rail point's first setting is non-zero, then the Wiggler will wait
/// at that point for that number of frames
void ObjectHanachan::onSegmentEnd() {
    u16 setting = m_railInterpolator->curPoint().setting[0];
    if (setting != 0) {
        m_still = true;
        m_stillDuration = setting;
    }
}

/// @addr{0x806CA6CC}
/// @brief Initializes the rail and its velocity
/// @details Initializes the rail interpolator to its starting position and sets its speed to
/// @ref m_walkSpeed.
void ObjectHanachan::initRail() {
    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setSpeed(m_walkSpeed);
}

/// @addr{0x806CA27C}
/// @brief Caches the last frame's rail tangent and updates the rail interpolator
/// @details Stores the current rail tangent direction in @ref m_prevRailTangent and then
/// updates the rail interpolator. If the end of the current rail segment is reached, calls
/// @ref onSegmentEnd().
void ObjectHanachan::calcRail() {
    m_prevRailTangent = m_railInterpolator->curTangentDir();

    if (m_railInterpolator->calc() == RailInterpolator::Status::SegmentEnd) {
        onSegmentEnd();
    }
}

/// @addr{0x806CA72C}
/// @brief Initializes the positions of the Wiggler's body parts based on the initial rail
/// position
/// @details Sets the head's position to the current rail interpolator position. For each body
/// segment, sets its position based on the previous segment's position and the current rail
/// tangent direction, and aligns its matrix tangent to the rail.
void ObjectHanachan::initBody() {
    headPart()->setPos(m_railInterpolator->curPos());

    const EGG::Vector3f &curTanDir = m_railInterpolator->curTangentDir();
    for (size_t i = 1; i < m_parts.size(); ++i) {
        m_parts[i]->setPos(m_parts[i - 1]->pos() - curTanDir * BODY_PART_DISTANCES[i - 1]);
        m_parts[i]->setMatrixTangentTo(EGG::Vector3f::ey, curTanDir);
    }
}

/// @addr{0x806CA2F0}
/// @brief Updates the transforms of the Wiggler's body parts based on the chain link positions
/// @details Linearly interpolates the head's forward direction towards the current rail tangent
/// direction. All other body parts' transforms are updated to reflect the exact direction between
/// it and the preceding chain link.
void ObjectHanachan::calcBody() {
    auto *&head = headPart();
    head->calcTransform();
    EGG::Vector3f forward = head->transform().base(2);
    EGG::Vector3f dir = forward.interpolate(0.1f, m_railInterpolator->curTangentDir());
    head->calcTransformFromUpAndTangent(m_chain.pos(0), m_chain.up(0), dir);

    auto parts = bodyParts();
    for (size_t i = 0; i < parts.size(); ++i) {
        const EGG::Vector3f &pos = m_chain.pos(i + 1);
        dir = m_chain.pos(i) - pos;
        dir.normalise2();
        parts[i]->calcTransformFromUpAndTangent(pos, m_chain.up(i + 1), dir);
    }
}

/// @addr{0x806CA9AC}
/// @brief Initializes the positions of the chain link objects based on the parts' initial
/// positions
void ObjectHanachan::initChain() {
    m_chain.init();

    for (size_t i = 0; i < m_parts.size(); ++i) {
        m_chain.setPos(i, m_parts[i]->pos());
    }
}

/// @addr{0x806CAAD0}
/// @brief Resets the position, velocity, and spring force for the first link in the chain (the
/// Wiggler's head)
void ObjectHanachan::clearChain() {
    m_chain.setPos(0, m_railInterpolator->curPos());
    m_chain.setVel(0, EGG::Vector3f::zero);
    m_chain.addSpringForce(0, EGG::Vector3f::ey * SphereLink::GRAVITY);
}

/// @addr{0x806CAB5C}
/// @brief Applies a lateral adjustment to the wiggler's body part segments if the Wiggler has moved
/// out of alignment of the rail.
/// @details Checks if the Wiggler has become misaligned this frame and caches the current frame to
/// @ref m_leftMisalignFrame if so. If @ref m_leftMisalignFrame is set and fewer than `80` frames
/// have elapsed since then, applies a larger lateral motion via @ref calcFastLateralMotion() with a
/// phase based on the number of frames since the misalignment occurred. Otherwise, applies the
/// default lateral motion via @ref calcDefaultLateralMotion().
/// @note In the base game, this function checks for both left and right misalignments of the rail
/// relative to the Wiggler. However, on Maple Treeway, the Wigglers always move counter-clockwise.
/// Thus, we only need to implement logic to handle @ref RailAlignment::MisalignedLeft.
void ObjectHanachan::calcRailAlignmentMotion() {
    constexpr u16 MISALIGNMENT_CORRECTION_DURATION = 80;

    bool becameUnaligned = m_prevRailAlignment == RailAlignment::Aligned &&
            m_railAlignment != RailAlignment::Aligned;

    if (becameUnaligned && m_railAlignment == RailAlignment::MisalignedLeft) {
        m_leftMisalignFrame = m_currentFrame;
    }

    if (m_leftMisalignFrame != 0 && m_currentFrame >= m_leftMisalignFrame &&
            m_currentFrame <
                    static_cast<u32>(m_leftMisalignFrame + MISALIGNMENT_CORRECTION_DURATION)) {
        calcFastLateralMotion(m_currentFrame - m_leftMisalignFrame);
    } else {
        calcDefaultLateralMotion();
    }
}

/// @addr{0x806CB67C}
/// @brief Updates the sway amplitude and calculates the lateral sway motion
/// @details Decays the sway amplitude by `0.25f`, and clamps it to zero if it becomes smaller
/// than `1.0f`. Finally, applies a sinusoidal sway to the body segments using the updated
/// amplitude.
void ObjectHanachan::calcSway() {
    constexpr f32 SWAY_AMPLITUDE_DELTA = 0.25f;

    if (m_swayAmplitude >= 0.0f) {
        m_swayAmplitude -= SWAY_AMPLITUDE_DELTA;
    } else {
        m_swayAmplitude += SWAY_AMPLITUDE_DELTA;
    }

    if (EGG::Mathf::abs(m_swayAmplitude) <= 1.0f) {
        m_swayAmplitude = 0.0f;
    }

    calcDefaultLateralMotion();
}

/// @addr{0x806CAC74}
/// @brief Applies a lateral motion when walking and standing still
/// @details The lateral motion has an amplitude of @ref m_swayAmplitude, a period of `50.0f`, a
/// wavelength of `4000.0f`, and a phase of @ref m_currentFrame.
void ObjectHanachan::calcDefaultLateralMotion() {
    constexpr f32 PERIOD = 50.0f;
    constexpr f32 WAVELENGTH = 4000.0f;

    calcLateralMotion(m_swayAmplitude, PERIOD, WAVELENGTH, m_currentFrame);
}

/// @addr{0x806CAC94}
/// @brief Applies a large lateral motion when the Wiggler is misaligned from the rail
/// @param frame The current frame used to calculate the lateral motion
/// @details The lateral motion has an amplitude of `25.0f`, a period of `300.0f`, a wavelength
/// of `5500.0f`, and a phase of the provided `frame`.
void ObjectHanachan::calcFastLateralMotion(s32 frame) {
    constexpr f32 AMPLITUDE = 25.0f;
    constexpr f32 PERIOD = 300.0f;
    constexpr f32 WAVELENGTH = 5500.0f;

    calcLateralMotion(AMPLITUDE, PERIOD, WAVELENGTH, static_cast<s16>(frame));
}

/// @addr{0x806CACD0}
/// @brief Calculates sinusoidal lateral sway motion for the Wiggler's body parts
/// @param amplitude The amplitude of the lateral sway
/// @param period The period of the lateral sway
/// @param wavelength The wavelength of the lateral sway
/// @param frame The current frame
/// @details For each body part \f$i\f$ with displacement \f$d_i\f$ (@ref m_partDisplacement) from
/// the head, the phase is
/// \f[ \phi_i(t) = 2\pi \left(\frac{t}{T} - \frac{d_i}{\lambda}\right) \f]
/// where \f$t\f$ is `frame`, \f$T\f$ is `period`, and \f$\lambda\f$ is `wavelength`. The lateral
/// offset applied to the part's chain link position is
/// \f[ \Delta \vec{p}_i(t) = A \sin(\phi_i(t)) \, \hat{r}_i \f]
/// where \f$A\f$ is `amplitude` and \f$\hat{r}_i\f$ is the part's `right` unit vector. The
/// corresponding link velocity is the time derivative of this offset,
/// \f[
/// \vec{v}_i(t) = \frac{d}{dt} \Delta \vec{p}_i(t) = \frac{2\pi A}{T} \cos(\phi_i(t)) \, \hat{r}_i
/// \f]
void ObjectHanachan::calcLateralMotion(f32 amplitude, f32 period, f32 wavelength, s16 frame) {
    f32 velAmplitude = F_TAU * amplitude / period;

    auto parts = bodyParts();
    for (size_t i = 0; i < parts.size(); ++i) {
        auto *&part = parts[i];
        f32 phase =
                F_TAU * (static_cast<f32>(frame) / period - m_partDisplacement[i + 1] / wavelength);
        part->calcTransform();
        EGG::Vector3f right = RotateXZByYaw(HALF_PI, part->transform().base(2));
        EGG::Vector3f posOffset = right * (amplitude * EGG::Mathf::SinFIdx(RAD2FIDX * phase));
        m_chain.setPos(i + 1, m_parts[i + 1]->pos() + posOffset);
        m_chain.setVel(i + 1, right * (velAmplitude * EGG::Mathf::CosFIdx(RAD2FIDX * phase)));
    }
}

/// @addr{0x806CBE2C}
/// @brief Sets the rail interpolator's velocity to the Wiggler's movement speed
void ObjectHanachan::setRailVel() {
    m_railInterpolator->setSpeed(m_walkSpeed);
}

/// @addr{0x806CAFB8}
/// @brief Calculates whether the Wiggler is misaligned with the rail
/// @return The current rail alignment of the Wiggler, which can be `RailAlignment::Aligned`,
/// `RailAlignment::MisalignedLeft`, or `RailAlignment::MisalignedRight`
/// @details Essentially, this function checks if the rail is straight/unchanged from the Wiggler's
/// perspective. If the dot product of the XZ components of the current and previous rail tangents
/// is greater than or equal to `0.9995f`, the rail is considered aligned. Otherwise, the cross
/// product determines if the misalignment is to the left or right.
ObjectHanachan::RailAlignment ObjectHanachan::calcRailAlignment() const {
    constexpr f32 EPSILON = 0.9995f;

    EGG::Vector3f curTanDir = m_railInterpolator->curTangentDir();
    EGG::Vector2f railTan = EGG::Vector2f(curTanDir.x, curTanDir.z);
    EGG::Vector2f prevTan = EGG::Vector2f(m_prevRailTangent.x, m_prevRailTangent.z);
    railTan.normalise2();
    prevTan.normalise2();

    if (railTan.dot(prevTan) >= EPSILON) {
        return RailAlignment::Aligned;
    }

    return prevTan.cross(railTan) < 0.0f ? RailAlignment::MisalignedLeft :
                                           RailAlignment::MisalignedRight;
}

/// @addr{0x806CAF9C}
/// @brief Determines if the Wiggler should start moving based on the current frame
/// @return `true` if the Wiggler should start moving, `false` otherwise
bool ObjectHanachan::shouldStartMoving() const {
    return m_currentFrame > m_stillDuration;
}

} // namespace Kinoko::Field
