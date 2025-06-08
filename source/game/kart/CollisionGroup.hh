#pragma once

#include "game/kart/KartParam.hh"

#include "game/field/KCollisionTypes.hh"

#include <egg/math/Matrix.hh>

#include <array>
#include <span>

/// @brief Pertains to kart-related functionality.
namespace Kart {

/// @brief Information about the current collision and its properties.
/// @see KCollisionTypes.hh
struct CollisionData {
    void reset();

    EGG::Vector3f tangentOff;
    EGG::Vector3f floorNrm;
    EGG::Vector3f wallNrm;
    EGG::Vector3f noBounceWallNrm;
    EGG::Vector3f vel;
    EGG::Vector3f relPos;
    EGG::Vector3f movement;
    EGG::Vector3f roadVelocity;
    f32 speedFactor;
    f32 rotFactor;
    Field::KCLTypeMask closestFloorFlags; ///< The KCL flag's @ref KColType.
    u32 closestFloorSettings;             ///< The KCL flag's "variant"
    s32 intensity;                        ///< The KCL flag's "wheel depth"
    f32 colPerpendicularity;

    bool bFloor; ///< Set if colliding with KCL which satisfies #KCL_TYPE_FLOOR
    bool bWall;  ///< Set if colliding with KCL which satisfies #KCL_TYPE_WALL
    bool bInvisibleWall;
    bool bWall3; ///< Set if colliding with #COL_TYPE_WALL_2
    bool bInvisibleWallOnly;
    bool bSoftWall;
    bool bTrickable;
    bool bHasRoadVel;
    bool bWallAtLeftCloser;
    bool bWallAtRightCloser;
};

/// @brief Represents a hitbox for the kart body or a wheel.
/// @details A hitbox's position information is directly used in the KCL collision check functions.
/// @nosubgrouping
class Hitbox {
public:
    Hitbox();
    ~Hitbox();

    void calc(f32 totalScale, f32 sinkDepth, const EGG::Vector3f &scale, const EGG::Quatf &rot,
            const EGG::Vector3f &pos);

    /// @beginSetters
    void reset();

    void setRadius(f32 radius) {
        m_radius = radius;
    }

    void setBspHitbox(const BSP::Hitbox *hitbox, bool owns = false) {
        m_ownsBSP = owns;
        m_bspHitbox = hitbox;
    }

    void setWorldPos(const EGG::Vector3f &pos) {
        m_worldPos = pos;
    }

    void setLastPos(const EGG::Vector3f &pos) {
        m_lastPos = pos;
    }

    void setLastPos(const EGG::Vector3f &scale, const EGG::Matrix34f &pose);
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] const BSP::Hitbox *bspHitbox() const {
        return m_bspHitbox;
    }

    [[nodiscard]] const EGG::Vector3f &worldPos() const {
        return m_worldPos;
    }

    [[nodiscard]] const EGG::Vector3f &lastPos() const {
        return m_lastPos;
    }

    [[nodiscard]] const EGG::Vector3f &relPos() const {
        return m_relPos;
    }

    [[nodiscard]] f32 radius() const {
        return m_radius;
    }
    /// @endGetters

private:
    const BSP::Hitbox *m_bspHitbox;
    f32 m_radius;
    EGG::Vector3f m_worldPos;
    EGG::Vector3f m_lastPos;
    EGG::Vector3f m_relPos;

    bool m_ownsBSP;
};

/// @brief Houses hitbox and collision info for an object (body or wheel).
/// @nosubgrouping
class CollisionGroup {
public:
    CollisionGroup();
    ~CollisionGroup();

    [[nodiscard]] f32 initHitboxes(const std::array<BSP::Hitbox, 16> &hitboxes);
    [[nodiscard]] f32 computeCollisionLimits();
    void createSingleHitbox(f32 radius, const EGG::Vector3f &relPos);

    /// @beginSetters
    void reset();
    void resetCollision();
    void setHitboxScale(f32 scale);
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] f32 boundingRadius() const {
        return m_boundingRadius;
    }

    [[nodiscard]] Hitbox &hitbox(u16 hitboxIdx) {
        return m_hitboxes[hitboxIdx];
    }

    [[nodiscard]] u16 hitboxCount() const {
        return m_hitboxes.size();
    }

    [[nodiscard]] CollisionData &collisionData() {
        return m_collisionData;
    }

    [[nodiscard]] const CollisionData &collisionData() const {
        return m_collisionData;
    }
    /// @endGetters

private:
    f32 m_boundingRadius;
    CollisionData m_collisionData;
    std::span<Hitbox> m_hitboxes;
    f32 m_hitboxScale;
};

} // namespace Kart
