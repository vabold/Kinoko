#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Represents the rising water on GCN Peach Beach and Delfino Pier battle stage.
/// @details In the base game, there are 5 subclasses owned by ObjectPsea. In terms of collision,
/// the water's height is only a function of one of them. We can simplify in Kinoko and remove that
/// subclass entirely since its position can be easily represented without an instance of the class.
/// @note This object is used in an unusual way - the @ref ObjectDirector keeps a pointer to it and
/// uses it to determine the height of the rising water. @ref Kart::KartMove will then use that
/// height to determine whether the kart is submerged and should be slowed down. Because @ref
/// ObjectDirector holds a single pointer to @ref ObjectPsea, if there are multiple instances of
/// this object, then only the most-recently-constructed one will have an effect on the player's
/// speed or out-of-bounds trigger.
class ObjectPsea final : public ObjectCollidable {
public:
    ObjectPsea(const System::MapdataGeoObj &params);
    ~ObjectPsea() override;

    void calc() override;
    void loadGraphics() override;
    void createCollision() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    [[nodiscard]] f32 calcInitPosY() const;

    u16 m_frame;          ///< Frame within the sinusoidal cycle of the rising water
    const f32 m_initPosY; ///< Initial position after applying hard-coded offsets

    static constexpr s16 PERIOD = 1200; ///< Framecount of a full cycle

    /// @brief Angular frequency of the water level's oscillation in radians per frame
    static constexpr f32 ANG_FREQ = F_TAU / static_cast<f32>(PERIOD);
};

} // namespace Kinoko::Field
