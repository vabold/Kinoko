#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Represents the Pokies on Dry Dry Ruins and DS Desert Hills
/// @details Pokies walk back and forth along their rail. They also perform collision checks against
/// the floor and KCL objects, including the Dry Dry Ruins sandcones, so that they don't clip
/// inside the sandcones.
class ObjectSanbo final : public ObjectCollidable {
public:
    ObjectSanbo(const System::MapdataGeoObj &params);
    ~ObjectSanbo() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    [[nodiscard]] EGG::Vector3f calcInitForward() const;

    void calcMove();
    void checkSphere();

    bool m_standstill;                 ///< Whether or not the Pokey is currently idle
    u32 m_stillTimer;                  ///< Frames before the Pokey will start to walk
    f32 m_yVel;                        ///< Falling speed
    EGG::Vector3f m_up;                ///< Upwards direction of the Pokey
    const EGG::Vector3f m_initForward; ///< Initial forward direction of the Pokey
};

} // namespace Kinoko::Field
