#pragma once

#include "game/kart/KartMove.hh"

namespace Kart {

class KartSub : public KartObjectProxy {
public:
    KartSub();

    void createSubsystems(bool isBike);
    void copyPointers(KartAccessor &pointers);

    void init();
    void initPhysicsValues();
    void resetPhysics();

    void calcPass0();
    void calcPass1();
    void FUN_805980d8();

    f32 someScale();

private:
    KartMove *m_move;
    KartCollide *m_collide;
    KartState *m_state;
    EGG::Vector3f m_maxSuspOvertravel;
    EGG::Vector3f m_minSuspOvertravel;
    u16 m_floorCollisionCount;
    f32 m_someScale;

    static constexpr f32 DT = 1.0f;
};

} // namespace Kart
