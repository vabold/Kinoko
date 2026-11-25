#include "ObjectTruckWagon.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"
#include "game/field/ObjectCollisionBase.hh"
#include "game/field/Rail.hh"
#include "game/field/RailInterpolator.hh"
#include "game/field/RailManager.hh"

#include "game/kart/KartObject.hh"
#include "game/kart/Reaction.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x806DFE9C}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes @ref m_active to `true`. Zeroes @ref m_vel, @ref m_lastVel, @ref m_up,
/// @ref m_tangent, and @ref m_pitch.
ObjectTruckWagonCart::ObjectTruckWagonCart(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      StateManager(this, STATE_ENTRIES),
      m_active(true),
      m_vel(EGG::Vector3f::zero),
      m_lastVel(EGG::Vector3f::zero),
      m_up(EGG::Vector3f::zero),
      m_tangent(EGG::Vector3f::zero),
      m_pitch(0.0f) {}

/// @addr{0x806E00F4}
/// @brief Default virtual destructor
ObjectTruckWagonCart::~ObjectTruckWagonCart() = default;

/// @addr{0x806E03E8}
/// @copybrief ObjectBase::calc()
/// @details Early returns if the minecart is currently not tangible. Otherwise, updates the
/// rail interpolator and sets the minecart's velocity accordingly. Evaluates the minecart's
/// state machine. Finally, refreshes the minecart's transformation matrix.
void ObjectTruckWagonCart::calc() {
    if (!m_active) {
        return;
    }

    calcRailAndVel();
    StateManager::calc();
    calcTransform();
}

/// @addr{0x806E2624}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details Applies a vertical offset to the minecart's collision transform based on whether it is
/// rolling on the floor or is suspended mid-air.
void ObjectTruckWagonCart::calcCollisionTransform() {
    constexpr f32 OFFSET_SUSPENDED = 500.0f;
    constexpr f32 OFFSET_ROLLING = 100.0f;

    auto *col = collision();
    if (!col || !m_active) {
        return;
    }

    f32 yOffset = m_currentStateId == 1 ? OFFSET_SUSPENDED : OFFSET_ROLLING;
    EGG::Matrix34f mat = EGG::Matrix34f::zero;
    mat.makeT(EGG::Vector3f(0.0f, yOffset, 0.0f));

    calcTransform();
    col->transform(transform().multiplyTo(mat), scale(), m_vel);
}

/// @addr{0x806E260C}
/// @copybrief ObjectBase::getName()
/// @return The name of the object, `TruckWagon`
const char *ObjectTruckWagonCart::getName() const {
    return "TruckWagon";
}

/// @addr{0x806E2744}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectTruckWagonCart::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806E2618}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the truck wagon, `TruckWagon`.
const char *ObjectTruckWagonCart::getKclName() const {
    return "TruckWagon";
}

/// @addr{0x806E0650}
/// @copybrief ObjectCollidable::onCollision()
/// @param kartObj The kart object involved in the collision
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @return @ref Kart::Reaction::WallAllSpeed if the kart's speed is below 50%, otherwise @ref
/// Kart::Reaction::LaunchSpin.
Kart::Reaction ObjectTruckWagonCart::onCollision(Kart::KartObject *kartObj,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    return kartObj->speedRatioCapped() < 0.5f ? Kart::Reaction::WallAllSpeed : reactionOnKart;
}

/// @brief Whether the minecart is currently tangible
/// @return The value of @ref m_active
bool ObjectTruckWagonCart::isActive() const {
    return m_active;
}

/// @addr{0x806E2060}
/// @brief Interface used by @ref ObjectTruckWagon to enable or disable a cart
/// @param isSet Whether the minecart should be spawned or despawned
void ObjectTruckWagonCart::setActive(bool isSet) {
    m_active = isSet;
}

/// @addr{0x806E0DF0}
/// @brief Runs every frame the minecart is rolling along the floor
/// @details This function's behavior diverges based on whether the minecart is between rail nodes 3
/// and 4.
///
/// If the minecart is between rail nodes 3 and 4, then the minecart will fall until it collides
/// with the floor. This special logic exists to avoid unnatural-looking movement in Wario's Gold
/// Mine when the minecart falls shortly after the spawner. When falling, the minecart's motion has
/// a downward speed of `15.0f`. Every frame the minecart is not colliding with the floor, an
/// additional downward gravitational force of `2.0f` is applied. If a collision did occur, the
/// Y-component of the minecart's @ref m_vel is reset to zero, the minecart is repositioned to rest
/// on the floor, and the minecart's orientation vectors (@ref m_up and @ref m_tangent) are
/// interpolated towards the floor's orientation and the rail interpolator's tangent direction.
/// Finally, the minecart's transformation matrix is updated to reflect its new position and
/// orientation.
///
/// If the minecart is between any other nodes, its position is directly set based on the rail
/// interpolator's current position. The minecart's orientation vectors (@ref m_up and @ref
/// m_tangent) are then interpolated towards the rail's pre-computed floor normal and the rail
/// interpolator's tangent direction, and the minecart's transformation matrix is updated
/// accordingly.
/// @warning It is not clear why @ref ObjectTruckWagon::ObjectTruckWagon() conditionally calls @ref
/// Rail::checkSphereFull() only if `rail->pointCount() > 40`, but as a result, minecart spawners
/// having a rail with fewer than 41 nodes will cause the game to crash on course load. @ref
/// Rail::checkSphereFull() allocates an array of floor normals for each node of the rail, which
/// this function will then index into. If this array is not allocated, then this will result in a
/// nullptr dereference.
void ObjectTruckWagonCart::calcRolling() {
    constexpr f32 RADIUS = 50.0f;
    constexpr f32 GRAVITY = 2.0f;
    constexpr f32 FALL_SPEED = 15.0f;
    constexpr f32 INTERP_RATE = 0.1f;

    const EGG::Vector3f &railPos = m_railInterpolator->curPos();
    if (m_railInterpolator->curPointIdx() != 3) {
        setPos(railPos);

        m_up = m_up.interpolate(INTERP_RATE,
                m_railInterpolator->floorNrm(m_railInterpolator->nextPointIdx()));
        m_tangent = m_tangent.interpolate(INTERP_RATE, m_railInterpolator->curTangentDir());
        m_up.normalise2();
        m_tangent.normalise2();

        setMatrixTangentTo(m_up, m_tangent);

        return;
    }

    setPos(EGG::Vector3f(railPos.x, pos().y - FALL_SPEED, railPos.z));

    CollisionInfo info;
    EGG::Vector3f colPos = pos() + EGG::Vector3f(0.0f, RADIUS, 0.0f);

    EGG::Vector3f floorNrm = m_up;
    EGG::Vector3f tangent = m_tangent;

    bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, colPos, EGG::Vector3f::inf,
            KCL_TYPE_FLOOR, &info, nullptr, 0);

    if (hasCol) {
        m_vel.y = 0.0f;
        addPos(info.tangentOff);

        if (info.floorDist > -std::numeric_limits<f32>::min()) {
            floorNrm = info.floorNrm;
        }

        tangent = m_railInterpolator->curTangentDir();
    } else {
        m_vel.y -= GRAVITY;
        setPos(EGG::Vector3f(pos().x, m_vel.y + pos().y, pos().z));
    }

    m_up = m_up.interpolate(INTERP_RATE, floorNrm);
    m_tangent = m_tangent.interpolate(INTERP_RATE, tangent);
    m_up.normalise2();
    m_tangent.normalise2();

    setMatrixTangentTo(m_up, m_tangent);
}

/// @addr{0x806E1370}
/// @brief Runs every frame that the minecart is suspended mid-air
/// @details Updates the minecart's @ref m_angVel based on its current pitch, the change in XZ-plane
/// velocity this frame, a spring stiffness factor, a pitch inertia factor, and a damping factor.
/// Increments @ref m_pitch based on the updated angular velocity. Interpolates @ref m_up towards
/// the up vector rotated based on the current pitch, and @ref m_tangent towards the rail's tangent
/// direction. Finally, updates the minecart's transformation matrix based on the interpolated
/// orientation vectors, updates the minecart's position based on the interpolated orientation
/// vectors with a vertical offset of `710.0f`, and caches the current @ref m_vel to @ref m_lastVel.
void ObjectTruckWagonCart::calcSuspended() {
    constexpr f32 INTERP_RATE = 0.1f;
    constexpr EGG::Vector3f INITIAL_OFFSET = EGG::Vector3f(0.0f, 710.0f, 0.0f);

    // Controls how strongly the cart tries to restore to neutral (damped harmonic oscillator)
    constexpr f32 SPRING_STIFFNESS = 3.9f;
    constexpr f32 PITCH_INERTIA = 1300.0f;
    constexpr f32 ANG_VEL_DECAY = 0.998f;

    EGG::Vector2f lastVelXZ = EGG::Vector2f(m_lastVel.x, m_lastVel.z);
    EGG::Vector2f velXZ = EGG::Vector2f(m_vel.x, m_vel.z);
    f32 velMagDiff = EGG::Mathf::sqrt((velXZ - lastVelXZ).dot());
    velMagDiff = velXZ.cross(lastVelXZ) > 0.0f ? -velMagDiff : velMagDiff;
    f32 cos = velMagDiff * EGG::Mathf::CosFIdx(RAD2FIDX * m_pitch);
    f32 sin = EGG::Mathf::SinFIdx(RAD2FIDX * m_pitch);
    m_angVel = (m_angVel + (cos + -SPRING_STIFFNESS * sin) / PITCH_INERTIA) * ANG_VEL_DECAY;
    m_pitch += m_angVel;

    EGG::Vector3f tanXZ = m_railInterpolator->curTangentDir();
    tanXZ.y = 0.0f;
    tanXZ.normalise2();
    EGG::Matrix34f mat;
    mat.setAxisRotation(m_pitch, tanXZ);
    mat.setBase(3, EGG::Vector3f::zero);

    m_up = m_up.interpolate(INTERP_RATE, mat.ps_multVector(EGG::Vector3f::ey));
    m_tangent = m_tangent.interpolate(INTERP_RATE, tanXZ);
    m_tangent.y = 0.0f;

    m_up.normalise2();
    m_tangent.normalise2();
    EGG::Vector3f cross = m_up.cross(m_tangent);
    cross.normalise2();

    mat.setBase(0, cross);
    mat.setBase(1, m_up);
    mat.setBase(2, m_tangent);
    mat.setBase(3, EGG::Vector3f::zero);

    EGG::Matrix34f transMat = mat;
    transMat.setBase(3, pos());
    setTransform(transMat);

    setPos(m_railInterpolator->curPos() + INITIAL_OFFSET - mat.ps_multVector(INITIAL_OFFSET));
    m_lastVel = m_vel;
}

/// @addr{0x806E1894}
/// @brief Runs every frame that the cart has @ref m_currentStateId equal to `2`
/// @todo It's not clear what the intended difference here is compared to rolling state
/// (@ref m_currentStateId equal to `0`)
void ObjectTruckWagonCart::calcState2() {
    calcRolling();
}

/// @addr{0x806E1DD0}
/// @brief Checks whether the minecart should transition to the suspended state where it is
/// lifted into the air
/// @details If the minecart is currently already in the suspended state, then early returns.
/// Otherwise, checks the current rail node's second setting. If it is set to 0, then the
/// minecart will return to the rolling state. If set to 1, then the minecart will transition to
/// the suspended state.
/// @note It is currently not known what @ref m_nextStateId `2` represents, but this function also
/// sets it if the rail point's setting is `2`.
void ObjectTruckWagonCart::checkRailPointState() {
    if (m_currentStateId == 1) {
        return;
    }

    u16 setting = m_railInterpolator->curPoint().setting[1];
    if (setting <= 2) {
        m_nextStateId = setting;
    }
}

/// @addr{0x806E1E34}
/// @brief Updates the rail interpolator and the minecart's velocity
/// @details Evaluates the rail interpolator for the current frame. If the minecart has reached
/// the end of a rail segment, calls @ref checkRailPointState() to see if the minecart should
/// transition from/to the rolling/suspended state. Otherwise, if the minecart has reached the
/// end of the rail, calls @ref deactivate() to despawn the minecart. Regardless, updates @ref
/// m_vel to reflect the rail's current velocity.
void ObjectTruckWagonCart::calcRailAndVel() {
    switch (m_railInterpolator->calc()) {
    case RailInterpolator::Status::SegmentEnd:
        checkRailPointState();
        break;
    case RailInterpolator::Status::ChangingDirection:
        deactivate();
        break;
    default:
        break;
    }

    m_vel.x = m_railInterpolator->currVel() * m_railInterpolator->curTangentDir().x;
    m_vel.z = m_railInterpolator->currVel() * m_railInterpolator->curTangentDir().z;
}

/// @addr{0x806E24FC}
/// @brief Runs when the cart is signaled by the spawner object to spawn
/// @details Resets the minecart to its initial state. Marks the minecart as active and loads
/// its collision.
void ObjectTruckWagonCart::activate() {
    reset(0);
    setActive(true);
    loadAABB(0.0f);
}

/// @addr{0x806E1F20}
/// @brief Runs when the cart is signaled by the spawner object to despawn
/// @details Resets the minecart to its initial state. Marks the minecart as inactive and
/// disables its collision. Finally, transitions the minecart to the rolling state so that it
/// moves along the rail once the minecart becomes active again.
void ObjectTruckWagonCart::deactivate() {
    reset(0);
    setActive(false);
    unregisterCollision();
    m_nextStateId = 0;
}

/// @addr{0x806E01C0}
/// @brief Resets the minecart to its initial state along the rail at the given rail point index
/// @param idx The index of the rail point to reset the minecart to
/// @details Initializes the minecart's rail interpolator to the specified rail point index and sets
/// @ref m_speed and @ref m_vel based on the rail's tangent direction at that point. Resets @ref
/// m_lastVel to the zero vector. If the minecart is not in the suspended state and the current rail
/// point's second setting is in the range `[0, 2]`, sets @ref m_nextStateId to that setting so that
/// the minecart transitions to the rolling, suspended, or "state2" state respectively. Finally, the
/// orientation vectors (@ref m_up and @ref m_tangent) are initialized to their default values, and
/// @ref m_pitch and @ref m_angVel are reset to zero.
void ObjectTruckWagonCart::reset(u32 idx) {
    m_railInterpolator->init(0.0f, idx);
    m_railInterpolator->setPerPointVelocities(true);

    setPos(m_railInterpolator->curPos());
    m_speed = m_railInterpolator->speed();
    m_vel = m_railInterpolator->curTangentDir() * m_speed;
    m_lastVel.setZero();

    if (m_currentStateId != 1) {
        u16 setting = m_railInterpolator->curPoint().setting[1];
        if (setting <= 2) {
            m_nextStateId = setting;
        }
    }

    m_up = EGG::Vector3f::ey;
    m_tangent = EGG::Vector3f::ez;
    m_pitch = 0.0f;
    m_angVel = 0.0f;
}

/// @addr{0x806E206C}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Sets @ref m_spawn2Frame based on param setting 2 and @ref m_spawn1Frame based on
/// param setting 3. Minecart spawners having param setting 4 set to a non-zero value spawn low LOD
/// minecarts in the background which do not have any tangible collision, thus we can ignore them in
/// Kinoko. Spawns 12 minecart objects and loads them. If the spawner's rail path contains 40 or
/// more segments, then calls @ref Rail::checkSphereFull() to cache floor normals at each node of
/// the rail.
/// @warning It is not clear why the `rail->pointCount() > 40` check is used, but as a result,
/// minecart spawners having a rail with fewer than 41 nodes will cause the game to crash on course
/// load. @ref Rail::checkSphereFull() allocates an array of floor normals for each node of the
/// rail, which @ref ObjectTruckWagonCart::calcRolling() will then index into. If this array is not
/// allocated, then this will result in a nullptr dereference.
ObjectTruckWagon::ObjectTruckWagon(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      m_spawn2Frame(static_cast<s32>(params.setting(1))),
      m_spawn1Frame(static_cast<s32>(params.setting(2))) {
    constexpr u32 CART_COUNT = 12;

    // For now, we don't care about low LOD minecarts since they don't have collision.
    if (params.setting(3) > 0) {
        return;
    }

    m_carts = owning_span<ObjectTruckWagonCart *>(CART_COUNT);

    for (auto *&cart : m_carts) {
        cart = EGG::egg_new<ObjectTruckWagonCart>(params);
        cart->load();
    }

    auto *rail = RailManager::Instance()->rail(params.pathId());
    ASSERT(rail);

    if (rail->pointCount() > 40) {
        rail->checkSphereFull();
    }
}

/// @addr{0x806E21EC}
/// @brief Default virtual destructor
ObjectTruckWagon::~ObjectTruckWagon() = default;

/// @addr{0x806E222C}
/// @copybrief ObjectBase::init()
/// @details Since we do not implement low LOD minecarts in Kinoko, we early return if the @ref
/// m_carts array is empty. For each minecart, ensures that the cart is deactivated before the
/// spawner's cycle begins. The second half of the minecarts are then activated and positioned along
/// the rail equidistantly. Finally, @ref m_cycleFrame and @ref m_curCartIdx are reset to zero.
void ObjectTruckWagon::init() {
    // If this spawner is for low LOD carts, then we need to skip init since the span is empty
    if (m_carts.empty()) {
        return;
    }

    for (auto *&cart : m_carts) {
        if (cart->isActive()) {
            cart->deactivate();
        }
    }

    u16 ptCount = m_carts[0]->railInterpolator()->pointCount();
    u32 cartCount = m_carts.size();
    u32 halfCount = m_carts.size() / 2;

    for (u32 i = halfCount; i < cartCount; ++i) {
        auto *&cart = m_carts[i];
        cart->reset(ptCount / halfCount * (cartCount - i));
        cart->setActive(true);
        cart->loadAABB(0.0f);
    }

    m_cycleFrame = 0;
    m_curCartIdx = 0;
}

/// @addr{0x806E23A4}
/// @copybrief ObjectBase::calc()
/// @details Checks to see if the current frame in the spawner's cycle corresponds to a kart
/// spawning. A minecart spawns at the start of the spawner's cycle and at @ref m_spawn2Frame frames
/// after the start of the cycle. The minecart corresponding to @ref m_curCartIdx is spawned via a
/// call to @ref ObjectTruckWagonCart::activate(). @ref m_curCartIdx is incremented modulo the total
/// number of carts. Regardless of whether a minecart spawned, @ref m_cycleFrame increments,
/// wrapping around once it exceeds the sum of @ref m_spawn2Frame and @ref m_spawn1Frame.
void ObjectTruckWagon::calc() {
    if (m_carts.empty()) {
        return;
    }

    if (m_cycleFrame == m_spawn2Frame || m_cycleFrame == m_spawn2Frame + m_spawn1Frame) {
        m_carts[m_curCartIdx]->activate();
        m_curCartIdx = (m_curCartIdx + 1) % m_carts.size();
    }

    m_cycleFrame = m_cycleFrame % (m_spawn2Frame + m_spawn1Frame) + 1;
}

/// @addr{0x806E24E8}
/// @copybrief ObjectBase::loadGraphics()
/// @details This is a no-op in the base game since the spawner does not have any graphics.
void ObjectTruckWagon::loadGraphics() {}

/// @addr{0x806E24E0}
/// @copybrief ObjectBase::createCollision()
/// @details No-op because the spawner itself does not have any collision.
void ObjectTruckWagon::createCollision() {}

/// @addr{0x806E24E4}
/// @copybrief ObjectBase::loadRail()
/// @details No-op because the spawner itself does not have any rail. The individual carts
/// manage their own positions along the rail.
void ObjectTruckWagon::loadRail() {}

/// @addr{0x806E24EC}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc and @ref eLoadFlags::Draw so that the object is
/// calculated every frame
ObjectBase::LoadFlags ObjectTruckWagon::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc, eLoadFlags::Draw);
}

} // namespace Kinoko::Field
