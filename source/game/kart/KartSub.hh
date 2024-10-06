#pragma once

#include "game/kart/KartMove.hh"

namespace Kart {

/// @brief Hosts a few classes and the high level per-frame calc functions.
/// @nosubgrouping
class KartSub : KartObjectProxy {
public:
    KartSub();
    ~KartSub();

    void createSubsystems(bool isBike);
    void copyPointers(KartAccessor &pointers);

    void init();
    void initPhysicsValues();

    void calcPass0();
    void calcPass1();
    void updateSuspOvertravel(const EGG::Vector3f &suspOvertravel);

    /// @beginGetters
    [[nodiscard]] f32 someScale();
    /// @endGetters

private:
    void resetPhysics();
    void addFloor(const CollisionData &, bool);
    void tryEndHWG();

    KartMove *m_move;
    KartCollide *m_collide;
    KartState *m_state;
    EGG::Vector3f m_maxSuspOvertravel;
    EGG::Vector3f m_minSuspOvertravel;
    u16 m_floorCollisionCount;
    s16 m_sideCollisionTimer;  ///< Number of frames to apply movement from wall collision.
    f32 m_colPerpendicularity; ///< Dot product between floor and colliding wall normals.
    f32 m_someScale;           /// @rename

    static constexpr f32 DT = 1.0f; ///< Delta time.
};

} // namespace Kart
