#pragma once

#include "game/field/ObjectDirector.hh"
#include "game/field/obj/ObjectSniper.hh"

#include <string>

namespace Field {

class ObjectHeyhoShipManager final : public ObjectSniper {
public:
    ObjectHeyhoShipManager();
    ~ObjectHeyhoShipManager() override;

    void init() override;
    void calc() override;

private:
    std::span<f32> m_projectileAngles; ///< Angle that the particular projectile will fly at.
    std::span<EGG::Vector3f> m_objPos;
};

} // namespace Field
