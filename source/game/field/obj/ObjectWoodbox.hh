#pragma once

#include "game/field/obj/ObjectBreakable.hh"

namespace Field {

class ObjectWoodbox : public ObjectBreakable {
public:
    ObjectWoodbox(const System::MapdataGeoObj &params);
    ~ObjectWoodbox() override;

    void init() override;
    void enableCollision() override;
    void onRespawn() override;

    /// @addr{0x8077ED7C}
    char *ObjectWoodbox::getKclName() {
        return "ironbox"; // woodbox when not in TTs
    }

    void calcCollisionTransform();

private:
    void calcFloor();

    f32 m_initialHeight;
    f32 m_downwardsVelocity;
};

} // namespace Field