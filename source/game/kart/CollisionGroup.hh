#pragma once

#include "game/kart/KartParam.hh"

#include "game/field/KCollisionTypes.hh"

#include <egg/math/Matrix.hh>

#include <array>
#include <span>

namespace Kart {

struct CollisionData {
    void reset();

    EGG::Vector3f tangentOff;
    EGG::Vector3f floorNrm;
    EGG::Vector3f noBounceWallNrm;
    EGG::Vector3f vel;
    EGG::Vector3f relPos;
    EGG::Vector3f movement;
    f32 speedFactor;
    f32 rotFactor;
    Field::KCLTypeMask closestFloorFlags;
    u32 closestFloorSettings;
    s32 intensity;

    bool bFloor;
    bool bSoftWall;
};

class Hitbox {
public:
    Hitbox();
    ~Hitbox();

    void calc(f32 param_1, f32 totalScale, const EGG::Vector3f &scale, const EGG::Quatf &rot,
            const EGG::Vector3f &pos);

    void reset();
    void setScale(f32 scale);
    void setRadius(f32 radius);
    void setBspHitbox(const BSP::Hitbox *hitbox, bool owns = false);
    void setWorldPos(const EGG::Vector3f &pos);
    void setLastPos(const EGG::Vector3f &pos);
    void setLastPos(const EGG::Vector3f &scale, const EGG::Matrix34f &pose);

    const BSP::Hitbox *bspHitbox() const;
    const EGG::Vector3f &worldPos() const;
    const EGG::Vector3f &lastPos() const;
    const EGG::Vector3f &relPos() const;
    f32 radius() const;

private:
    const BSP::Hitbox *m_bspHitbox;
    f32 m_radius;
    EGG::Vector3f m_worldPos;
    EGG::Vector3f m_lastPos;
    EGG::Vector3f m_relPos;

    bool m_ownsBSP;
};

class CollisionGroup {
public:
    CollisionGroup();
    ~CollisionGroup();

    f32 initHitboxes(const std::array<BSP::Hitbox, 16> &hitboxes);
    f32 computeCollisionLimits();
    void createSingleHitbox(f32 radius, const EGG::Vector3f &relPos);

    void reset();
    void resetCollision();

    Hitbox &hitbox(u16 hitboxIdx);
    u16 hitboxCount() const;
    CollisionData &collisionData();
    const CollisionData &collisionData() const;

    void setHitboxScale(f32 scale);

private:
    f32 m_boundingRadius;
    CollisionData m_collisionData;
    std::span<Hitbox> m_hitboxes;
    f32 m_hitboxScale;
};

} // namespace Kart
