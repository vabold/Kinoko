#pragma once

#include "game/kart/KartMove.hh"

namespace Kart {

class KartObject;

/// @brief Hosts a few classes and the high level per-frame calc functions.
/// @nosubgrouping
class KartSub : KartObjectProxy {
public:
    KartSub();
    ~KartSub();

    void createSubsystems(bool isBike);
    void copyPointers(KartAccessor &pointers);

    void init();
    void initAABB(KartAccessor &accessor, KartObject *object);
    void initPhysicsValues();
    void resetPhysics();

    void calcPass0();
    void calcPass1();
    void resizeAABB(f32 radiusScale);
    void addFloor(const CollisionData &, bool);
    void updateSuspOvertravel(const EGG::Vector3f &suspOvertravel);
    void tryEndHWG();
    void calcMovingObj();

    /// @beginGetters
    [[nodiscard]] f32 someScale() {
        return m_someScale;
    }
    /// @endGetters

private:
    KartMove *m_move;
    KartAction *m_action;
    KartCollide *m_collide;
    KartState *m_state;
    EGG::Vector3f m_maxSuspOvertravel;
    EGG::Vector3f m_minSuspOvertravel;
    u16 m_floorCollisionCount;
    u16 m_movingObjCollisionCount;
    EGG::Vector3f m_objVel;
    s16 m_sideCollisionTimer;  ///< Number of frames to apply movement from wall collision.
    f32 m_colPerpendicularity; ///< Dot product between floor and colliding wall normals.
    f32 m_someScale;           /// @rename

    static constexpr f32 DT = 1.0f; ///< Delta time.
};

} // namespace Kart
