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

private:
    KartMove *m_move;
    KartState *m_state;

    static constexpr f32 DT = 1.0f;
};

} // namespace Kart
