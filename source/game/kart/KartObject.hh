#pragma once

#include "source/game/kart/KartObjectProxy.hh"
#include "source/game/kart/KartParam.hh"

namespace Kart {

class KartObject : public KartObjectProxy {
public:
    KartObject(KartParam *param);
    virtual ~KartObject();
    virtual void createComponents();

    static KartObject *Create(Character character, Vehicle vehicle);

protected:
    KartParam *m_param;
    KartAccessor m_pointers;
};

class KartObjectBike : public KartObject {
public:
    KartObjectBike(KartParam *param);
    ~KartObjectBike() override;
    void createComponents() override;
};

} // namespace Kart
