#pragma once

#include "game/kart/KartObjectProxy.hh"

/// @brief Pertains to rendering the kart model.
namespace Render {

/// @brief Included in Kinoko because it mysteriously sets an angle member variable in KartBody.
class KartModel : Kart::KartObjectProxy {
public:
    KartModel();
    virtual ~KartModel();

    virtual void vf_1c();

    void init();
    void calc();

    void FUN_807CB198();
    void FUN_807CB530();
    void FUN_807C7828(u8 playerIdx, bool isBike);

private:
    bool m_somethingLeft;
    bool m_somethingRight;
    f32 _54;
    f32 _58;
    f32 _5c;
    f32 _64;
    bool m_isInsideDrift;
    f32 _2e8;
};

class KartModelKart : public KartModel {
public:
    KartModelKart();
    ~KartModelKart();
};

class KartModelBike : public KartModel {
public:
    KartModelBike();
    ~KartModelBike();
};

} // namespace Render
