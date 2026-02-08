#include "ObjectTruckWagon.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/Rail.hh"
#include "game/field/RailManager.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartObject.hh"

namespace Field {

/// @addr{0x806DFE9C}
ObjectTruckWagonCart::ObjectTruckWagonCart(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), StateManager(this, STATE_ENTRIES), m_active(true),
      m_vel(EGG::Vector3f::zero), m_lastVel(EGG::Vector3f::zero), m_up(EGG::Vector3f::zero),
      m_tangent(EGG::Vector3f::zero), m_pitch(0.0f) {}

/// @addr{0x806E00F4}
ObjectTruckWagonCart::~ObjectTruckWagonCart() = default;

/// @addr{0x806E03E8}
void ObjectTruckWagonCart::calc() {
    if (!m_active) {
        return;
    }

    switch (m_railInterpolator->calc()) {
    case RailInterpolator::Status::SegmentEnd:
        if (m_currentStateId != 1) {
            u16 setting = m_railInterpolator->curPoint().setting[1];
            if (setting <= 2) {
                m_nextStateId = setting;
            }
        }
        break;
    case RailInterpolator::Status::ChangingDirection:
        deactivate();
        break;
    default:
        break;
    }

    m_vel.x = m_railInterpolator->currVel() * m_railInterpolator->curTangentDir().x;
    m_vel.z = m_railInterpolator->currVel() * m_railInterpolator->curTangentDir().z;

    StateManager::calc();

    calcTransform();
}

/// @addr{0x806E2624}
void ObjectTruckWagonCart::calcCollisionTransform() {
    auto *col = collision();
    if (!col || !m_active) {
        return;
    }

    f32 yOffset = m_currentStateId == 1 ? 500.0f : 100.0f;
    EGG::Matrix34f mat = EGG::Matrix34f::zero;
    mat.makeT(EGG::Vector3f(0.0f, yOffset, 0.0f));

    calcTransform();
    col->transform(m_transform.multiplyTo(mat), m_scale, m_vel);
}

/// @addr{0x806E0650}
Kart::Reaction ObjectTruckWagonCart::onCollision(Kart::KartObject *kartObj,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    return kartObj->speedRatioCapped() < 0.5f ? Kart::Reaction::WallAllSpeed : reactionOnKart;
}

/// @addr{0x806E0DF0}
void ObjectTruckWagonCart::calcState0() {
    constexpr f32 RADIUS = 50.0f;
    constexpr f32 GRAVITY = 2.0f;
    constexpr f32 INITIAL_FALL_OFFSET = 15.0f;

    const EGG::Vector3f &railPos = m_railInterpolator->curPos();
    if (m_railInterpolator->curPointIdx() != 3) {
        m_pos = railPos;
        m_flags.setBit(eFlags::Position);

        m_up = Interpolate(0.1f, m_up,
                m_railInterpolator->floorNrm(m_railInterpolator->nextPointIdx()));
        m_tangent = Interpolate(0.1f, m_tangent, m_railInterpolator->curTangentDir());
        m_up.normalise2();
        m_tangent.normalise2();

        setMatrixTangentTo(m_up, m_tangent);

        return;
    }

    m_pos = EGG::Vector3f(railPos.x, m_pos.y - INITIAL_FALL_OFFSET, railPos.z);
    m_flags.setBit(eFlags::Position);

    CollisionInfo info;
    EGG::Vector3f pos = m_pos + EGG::Vector3f(0.0f, RADIUS, 0.0f);

    EGG::Vector3f floorNrm = m_up;
    EGG::Vector3f tangent = m_tangent;

    bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, pos, EGG::Vector3f::inf,
            KCL_TYPE_FLOOR, &info, nullptr, 0);

    if (hasCol) {
        m_vel.y = 0.0f;
        m_pos += info.tangentOff;
        m_flags.setBit(eFlags::Position);

        if (info.floorDist > -std::numeric_limits<f32>::min()) {
            floorNrm = info.floorNrm;
        }

        tangent = m_railInterpolator->curTangentDir();
    } else {
        m_flags.setBit(eFlags::Position);
        m_vel.y -= GRAVITY;
        m_pos.y = m_vel.y + m_pos.y;
    }

    m_up = Interpolate(0.1f, m_up, floorNrm);
    m_tangent = Interpolate(0.1f, m_tangent, tangent);
    m_up.normalise2();
    m_tangent.normalise2();

    setMatrixTangentTo(m_up, m_tangent);
}

/// @addr{0x806E1370}
void ObjectTruckWagonCart::calcState1() {
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

    m_up = Interpolate(0.1f, m_up, mat.ps_multVector(EGG::Vector3f::ey));
    m_tangent = Interpolate(0.1f, m_tangent, tanXZ);
    m_tangent.y = 0.0f;

    m_up.normalise2();
    m_tangent.normalise2();
    EGG::Vector3f cross = m_up.cross(m_tangent);
    cross.normalise2();

    mat.setBase(0, cross);
    mat.setBase(1, m_up);
    mat.setBase(2, m_tangent);
    mat.setBase(3, EGG::Vector3f::zero);

    m_flags.setBit(eFlags::Matrix);
    m_transform = mat;
    m_transform.setBase(3, m_pos);

    m_flags.setBit(eFlags::Position);
    m_pos = m_railInterpolator->curPos() + INITIAL_OFFSET - mat.ps_multVector(INITIAL_OFFSET);
    m_lastVel = m_vel;
}

/// @addr{0x806E01C0}
void ObjectTruckWagonCart::reset(u32 idx) {
    m_railInterpolator->init(0.0f, idx);
    m_railInterpolator->setPerPointVelocities(true);

    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);
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
ObjectTruckWagon::ObjectTruckWagon(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_spawn2Frame(static_cast<s32>(params.setting(1))),
      m_cycleDuration(static_cast<s32>(params.setting(2))) {
    constexpr u32 CART_COUNT = 12;

    // For now, we don't care about low LOD minecarts since they don't have collision.
    if (params.setting(3) > 0) {
        return;
    }

    m_carts = std::span<ObjectTruckWagonCart *>(new ObjectTruckWagonCart *[CART_COUNT], CART_COUNT);

    for (auto *&cart : m_carts) {
        cart = new ObjectTruckWagonCart(params);
        cart->load();
    }

    auto *rail = RailManager::Instance()->rail(params.pathId());
    ASSERT(rail);

    if (rail->pointCount() > 40) {
        rail->checkSphereFull();
    }
}

/// @addr{0x806E21EC}
ObjectTruckWagon::~ObjectTruckWagon() {
    delete[] m_carts.data();
}

/// @addr{0x806E222C}
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
void ObjectTruckWagon::calc() {
    if (m_carts.empty()) {
        return;
    }

    if (m_cycleFrame == m_spawn2Frame || m_cycleFrame == m_spawn2Frame + m_cycleDuration) {
        m_carts[m_curCartIdx]->activate();
        m_curCartIdx = (m_curCartIdx + 1) % m_carts.size();
    }

    m_cycleFrame = m_cycleFrame % (m_spawn2Frame + m_cycleDuration) + 1;
}

} // namespace Field
