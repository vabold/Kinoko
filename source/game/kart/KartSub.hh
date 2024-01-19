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

private:
    KartMove *m_move;
};

} // namespace Kart
