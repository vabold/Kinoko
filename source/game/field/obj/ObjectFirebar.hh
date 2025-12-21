#pragma once

#include "game/field/obj/ObjectCollidable.hh"
#include "game/field/obj/ObjectFireball.hh"

namespace Field {

class ObjectFirebar : public ObjectCollidable {
public:
    ObjectFirebar(const System::MapdataGeoObj &params);
    ~ObjectFirebar() override;

    void init() override;
    void calc() override;

    /// @addr{0x807687D8}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x808CE358}
    [[nodiscard]] const char *getKclName() const override {
        return id() == ObjectId::WLFirebarGC ? "WLfirebarGC" : "koopaFirebar";
    }

private:
    std::span<ObjectFireball *> m_fireballs;
    const f32 m_angSpeed;
    f32 m_degAngle;
    EGG::Vector3f m_axis;
    EGG::Vector3f m_initDir;
};

} // namespace Field
