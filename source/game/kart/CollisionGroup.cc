#include "CollisionGroup.hh"

namespace Kart {

void CollisionData::reset() {
    tangentOff.setZero();
    floorNrm.setZero();
    vel.setZero();
    relPos.setZero();
    movement.setZero();
    speedFactor = 1.0f;
    rotFactor = 0.0f;
    closestFloorFlags = 0;
    closestFloorSettings = 0xffffffff;
    intensity = 0.0f;

    bFloor = false;
    bSoftWall = false;
}

Hitbox::Hitbox() : m_bspHitbox(nullptr), m_ownsBSP(false) {}

Hitbox::~Hitbox() {
    if (m_ownsBSP) {
        delete m_bspHitbox;
    }
}

void Hitbox::calc(f32 param_1, f32 totalScale, const EGG::Vector3f &scale, const EGG::Quatf &rot,
        const EGG::Vector3f &pos) {
    f32 fVar1 = 0.0f;
    if (scale.y < param_1) {
        fVar1 = (param_1 - scale.y) * m_bspHitbox->radius;
    }

    EGG::Vector3f scaledPos = m_bspHitbox->position * scale;
    scaledPos.y = (m_bspHitbox->position.y + totalScale) * scale.y + fVar1;

    m_relPos = rot.rotateVector(scaledPos);
    m_worldPos = m_relPos + pos;
}

void Hitbox::reset() {
    m_worldPos.setZero();
    m_lastPos.setZero();
    m_relPos.setZero();
}

void Hitbox::setRadius(f32 radius) {
    m_radius = radius;
}

void Hitbox::setBspHitbox(const BSP::Hitbox *hitbox, bool owns) {
    m_ownsBSP = owns;
    m_bspHitbox = hitbox;
}

void Hitbox::setWorldPos(const EGG::Vector3f &pos) {
    m_worldPos = pos;
}

void Hitbox::setLastPos(const EGG::Vector3f &pos) {
    m_lastPos = pos;
}

void Hitbox::setLastPos(const EGG::Vector3f &scale, const EGG::Matrix34f &pose) {
    f32 yScaleFactor = scale.y;
    EGG::Vector3f scaledPos = m_bspHitbox->position;
    scaledPos.x *= scale.x;
    scaledPos.z *= scale.z;

    if (scale.y != scale.z && scale.y < 1.0f) {
        scaledPos.y += (1.0f - scale.y) * m_radius;
        yScaleFactor = scale.z;
    }

    scaledPos.y *= yScaleFactor;
    m_lastPos = pose.ps_multVector(scaledPos);
}

const BSP::Hitbox *Hitbox::bspHitbox() const {
    return m_bspHitbox;
}

const EGG::Vector3f &Hitbox::worldPos() const {
    return m_worldPos;
}

const EGG::Vector3f &Hitbox::lastPos() const {
    return m_lastPos;
}

const EGG::Vector3f &Hitbox::relPos() const {
    return m_relPos;
}

f32 Hitbox::radius() const {
    return m_radius;
}

CollisionGroup::CollisionGroup() : m_hitboxScale(1.0f) {
    m_collisionData.reset();
}

CollisionGroup::~CollisionGroup() {
    delete[] m_hitboxes.data();
}

f32 CollisionGroup::initHitboxes(const std::array<BSP::Hitbox, 16> &hitboxes) {
    u16 bspHitboxCount = 0;

    for (const auto &hitbox : hitboxes) {
        if (parse<u16>(hitbox.enable)) {
            ++bspHitboxCount;
        }
    }

    m_hitboxes = std::span<Hitbox>(new Hitbox[bspHitboxCount], bspHitboxCount);

    for (u16 hitboxIdx = 0; hitboxIdx < m_hitboxes.size(); ++hitboxIdx) {
        m_hitboxes[hitboxIdx].setBspHitbox(&hitboxes[hitboxIdx]);
    }

    return computeCollisionLimits();
}

f32 CollisionGroup::computeCollisionLimits() {
    EGG::Vector3f max;

    for (const auto &hitbox : m_hitboxes) {
        const BSP::Hitbox *bspHitbox = hitbox.bspHitbox();

        if (bspHitbox->enable == 0) {
            continue;
        }

        max = max.maximize(bspHitbox->position.abs() + bspHitbox->radius);
    }

    // Get largest component of the vector
    f32 maxComponent = max.z;

    if (max.x <= max.y) {
        if (max.z < max.y) {
            maxComponent = max.y;
        }
    } else if (max.z < max.x) {
        maxComponent = max.x;
    }

    m_boundingRadius = maxComponent;

    return max.z * 0.5f;
}

void CollisionGroup::createSingleHitbox(f32 radius, const EGG::Vector3f &relPos) {
    m_hitboxes = std::span<Hitbox>(new Hitbox[1], 1);

    // TODO: Do we need for loop if this is just one?
    // And how exactly will we identify to free the BSP::Hitbox on destruction?
    for (auto &hitbox : m_hitboxes) {
        hitbox.reset();
        BSP::Hitbox *bspHitbox = new BSP::Hitbox;
        hitbox.setBspHitbox(bspHitbox, true);
        bspHitbox->position = relPos;
        bspHitbox->radius = radius;
        hitbox.setRadius(radius);
    }
    m_boundingRadius = radius;
}

void CollisionGroup::reset() {
    m_collisionData.reset();

    for (auto &hitbox : m_hitboxes) {
        hitbox.reset();
        hitbox.setRadius(hitbox.bspHitbox()->radius * m_hitboxScale);
    }
}

void CollisionGroup::resetCollision() {
    m_collisionData.reset();
}

Hitbox &CollisionGroup::hitbox(u16 hitboxIdx) {
    return m_hitboxes[hitboxIdx];
}

u16 CollisionGroup::hitboxCount() const {
    return m_hitboxes.size();
}

CollisionData &CollisionGroup::collisionData() {
    return m_collisionData;
}

const CollisionData &CollisionGroup::collisionData() const {
    return m_collisionData;
}

void CollisionGroup::setHitboxScale(f32 scale) {
    m_hitboxScale = scale;

    for (auto &hitbox : m_hitboxes) {
        hitbox.setRadius(hitbox.bspHitbox()->radius * m_hitboxScale);
    }
}

} // namespace Kart
