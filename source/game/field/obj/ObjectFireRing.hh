#pragma once

#include "game/field/obj/ObjectCollidable.hh"
#include "game/field/obj/ObjectFireball.hh"

namespace Field {

class ObjectFireRing : public ObjectCollidable {
public:
    ObjectFireRing(const System::MapdataGeoObj &params);
    ~ObjectFireRing() override;

    void init() override;
    void calc() override;

    /// @addr{0x80768740}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x80768734}
    void createCollision() override {}

private:
    std::span<ObjectFireball *> m_fireballs;
    const f32 m_angSpeed;
    f32 m_degAngle;
    EGG::Vector3f m_axis;
    EGG::Vector3f m_initDir;
    f32 m_radiusScale;
    f32 m_phase;
};

} // namespace Field
