#pragma once

#include "game/field/obj/ObjectFireball.hh"

namespace Kinoko::Field {

/// @brief A rotating group of fireballs split into spokes.
/// @details The number of spokes is based on the fourth param setting. The first param setting
/// defines how many fireballs per spoke.
class ObjectFirebar final : public ObjectCollidable {
public:
    ObjectFirebar(const System::MapdataGeoObj &params);
    ~ObjectFirebar() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getKclName() const override;

private:
    const u32 m_spokes;                        ///< The number of fireball "segments"
    const f32 m_angSpeed;                      ///< Angular speed of the fireballs (per second)
    owning_span<ObjectFireball *> m_fireballs; ///< Array of pointers to underlying fireballs
    f32 m_currAngle;                           ///< Current angle of rotation modulo 360
    EGG::Vector3f m_axis;                      ///< Axis of rotation
    EGG::Vector3f m_initDir;                   ///< Initial tangent direction
};

} // namespace Kinoko::Field
