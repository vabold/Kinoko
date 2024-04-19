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
    EGG::Vector3f noBounceWallNrm;
    EGG::Vector3f vel;
    EGG::Vector3f relPos;
    EGG::Vector3f movement;
    f32 speedFactor;
    f32 rotFactor;
    Field::KCLTypeMask closestFloorFlags; ///< The KCL flag's @ref KColType.
    u32 closestFloorSettings;             ///< The KCL flag's "variant"
    s32 intensity;                        ///< The KCL flag's "wheel depth"

    bool bFloor; ///< Set if colliding with KCL which satisfies #KCL_TYPE_FLOOR
    bool bSoftWall;
    bool bTrickable;
};

/// @brief Represents a hitbox for the kart body or a wheel.
/// @details A hitbox's position information is directly used in the KCL collision check functions.
/// @nosubgrouping
class Hitbox {
public:
    Hitbox();
    ~Hitbox();

    void calc(f32 param_1, f32 totalScale, const EGG::Vector3f &scale, const EGG::Quatf &rot,
            const EGG::Vector3f &pos);

    /// @beginSetters
    void reset();
    void setRadius(f32 radius);
    void setBspHitbox(const BSP::Hitbox *hitbox, bool owns = false);
    void setWorldPos(const EGG::Vector3f &pos);
    void setLastPos(const EGG::Vector3f &pos);
    void setLastPos(const EGG::Vector3f &scale, const EGG::Matrix34f &pose);
    /// @endSetters

    /// @beginGetters
    const BSP::Hitbox *bspHitbox() const;
    const EGG::Vector3f &worldPos() const;
    const EGG::Vector3f &lastPos() const;
    const EGG::Vector3f &relPos() const;
    f32 radius() const;
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

    f32 initHitboxes(const std::array<BSP::Hitbox, 16> &hitboxes);
    f32 computeCollisionLimits();
    void createSingleHitbox(f32 radius, const EGG::Vector3f &relPos);

    /// @beginSetters
    void reset();
    void resetCollision();
    void setHitboxScale(f32 scale);
    /// @endSetters

    /// @beginGetters
    Hitbox &hitbox(u16 hitboxIdx);
    u16 hitboxCount() const;
    CollisionData &collisionData();
    const CollisionData &collisionData() const;
    /// @endGetters

private:
    f32 m_boundingRadius;
    CollisionData m_collisionData;
    std::span<Hitbox> m_hitboxes;
    f32 m_hitboxScale;
};

} // namespace Kart
