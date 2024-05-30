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
    void resetPhysics();

    void calcPass0();
    void calcPass1();
    void addFloor(const CollisionData &, bool);
    void updateSuspOvertravel(const EGG::Vector3f &suspOvertravel);

    /// @beginGetters
    f32 someScale();
    /// @endGetters

private:
    KartMove *m_move;
    KartCollide *m_collide;
    KartState *m_state;
    EGG::Vector3f m_maxSuspOvertravel;
    EGG::Vector3f m_minSuspOvertravel;
    u16 m_floorCollisionCount;
    f32 m_someScale; /// @rename

    static constexpr f32 DT = 1.0f; ///< Delta time.
};

} // namespace Kart
