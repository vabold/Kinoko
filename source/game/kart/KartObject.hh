#pragma once

#include "game/kart/KartBody.hh"
#include "game/kart/KartObjectProxy.hh"
#include "game/kart/KartParam.hh"
#include "game/kart/KartPhysics.hh"

namespace Kart {

class KartObject : public KartObjectProxy {
public:
    KartObject(KartParam *param);
    virtual ~KartObject();
    virtual KartBody *createBody(KartPhysics *physics);
    virtual void createTires() {}

    void init();
    void initImpl();
    void prepare();
    void prepareTiresAndSuspensions();

    void createSub();
    void createModel();

    void calcSub();
    void calc();

    static KartObject *Create(Character character, Vehicle vehicle, u8 playerIdx);

protected:
    KartAccessor m_pointers;
};

class KartObjectBike : public KartObject {
public:
    KartObjectBike(KartParam *param);
    ~KartObjectBike() override;
    KartBody *createBody(KartPhysics *physics) override;
    void createTires() override;
};

} // namespace Kart
