#include "CollisionGroup.hh"

namespace Kart {

/// @addr{0x805B821C}
void CollisionData::reset() {
    tangentOff.setZero();
    floorNrm.setZero();
    wallNrm.setZero();
    vel.setZero();
    relPos.setZero();
    movement.setZero();
    roadVelocity.setZero();
    speedFactor = 1.0f;
    rotFactor = 0.0f;
    closestFloorFlags = 0;
    closestFloorSettings = 0xffffffff;
    intensity = 0.0f;
    colPerpendicularity = 0.0f;

    bFloor = false;
    bWall = false;
    bInvisibleWall = false;
    bWall3 = false;
    bInvisibleWallOnly = false;
    bSoftWall = false;
    bTrickable = false;
    bHasRoadVel = false;
    bWallAtLeftCloser = false;
    bWallAtRightCloser = false;
}

/// @addr{0x805B7F48}
Hitbox::Hitbox() : m_bspHitbox(nullptr), m_ownsBSP(false) {}

/// @addr{0x805B8480}
Hitbox::~Hitbox() {
    if (m_ownsBSP) {
        delete m_bspHitbox;
    }
}

/// @brief Calculates the position of a given hitbox, both relative to the player and world
/// @addr{0x805B7FBC}
void Hitbox::calc(f32 totalScale, f32 sinkDepth, const EGG::Vector3f &scale, const EGG::Quatf &rot,
        const EGG::Vector3f &pos) {
    f32 fVar1 = 0.0f;
    if (scale.y < totalScale) {
        fVar1 = (totalScale - scale.y) * m_bspHitbox->radius;
    }

    EGG::Vector3f scaledPos = m_bspHitbox->position * scale;
    scaledPos.y = (m_bspHitbox->position.y + sinkDepth) * scale.y + fVar1;

    m_relPos = rot.rotateVector(scaledPos);
    m_worldPos = m_relPos + pos;
}

/// @addr{0x805B7F84}
void Hitbox::reset() {
    m_worldPos.setZero();
    m_lastPos.setZero();
    m_relPos.setZero();
}

/// @addr{0x805B80A8}
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

/// @addr{0x805B82BC}
CollisionGroup::CollisionGroup() : m_hitboxScale(1.0f) {
    m_collisionData.reset();
}

CollisionGroup::~CollisionGroup() {
    delete[] m_hitboxes.data();
}

/// @brief Initializes the hitbox array based on the KartParam's BSP hitboxes
/// @addr{0x805B84C0}
/// @details The BSP always contains 16 hitboxes, but only some of them are valid/enabled.
/// The game iterates the BSP::Hitbox array to see how many are enabled, allocates a Hitbox array of
/// that size, and then sets all the enabled BSP hitboxes.
/// @param hitboxes The hitboxes from KartParam.bin
/// @return The furthest point out of the hitboxes' spheres
f32 CollisionGroup::initHitboxes(const std::array<BSP::Hitbox, 16> &hitboxes) {
    u16 bspHitboxCount = 0;

    for (const auto &hitbox : hitboxes) {
        if (parse<u16>(hitbox.enable)) {
            ++bspHitboxCount;
        }
    }

    m_hitboxes = std::span<Hitbox>(new Hitbox[bspHitboxCount], bspHitboxCount);
    u16 hitboxIdx = 0;

    for (const auto &bspHitbox : hitboxes) {
        if (parse<u16>(bspHitbox.enable)) {
            m_hitboxes[hitboxIdx++].setBspHitbox(&bspHitbox);
        }
    }

    return computeCollisionLimits();
}

/// @brief Sets the bounding radius
/// @addr{0x805B883C}
/// @return The furthest point of all the hitboxes' spheres
f32 CollisionGroup::computeCollisionLimits() {
    EGG::Vector3f max = EGG::Vector3f::zero;

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

/// @brief Creates a hitbox to represent a tire
/// @addr{0x805B875C}
/// @param radius The radius of the tire
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

/// @addr{0x805B8330}
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

/// @addr{0x805B83D8}
void CollisionGroup::setHitboxScale(f32 scale) {
    m_hitboxScale = scale;

    for (auto &hitbox : m_hitboxes) {
        hitbox.setRadius(hitbox.bspHitbox()->radius * m_hitboxScale);
    }
}

} // namespace Kart
