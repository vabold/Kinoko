#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief The base class for penguins on N64 Sherbet Land
/// @details Penguins walk along a rail, rotating with smoothed interpolation to face the rail
/// tangent. In the base game, the cave penguins derive from this class, but they do not override
/// any physics-dependent virtual functions. For Kinoko, we simply construct these penguins as a
/// base class instance. This class also defines an enum class which describes the state of motion
/// of the penguin, which is used by the derived class @ref ObjectPenguinS to determine when the
/// penguin is sliding or walking.
class ObjectPenguin : public ObjectCollidable {
public:
    ObjectPenguin(const System::MapdataGeoObj &params);
    ~ObjectPenguin() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

protected:
    /// @brief The state of motion of the penguin
    enum class State {
        Walk = 0,      ///< Walking
        Dive = 1,      ///< Diving mid-air onto belly
        Slide = 2,     ///< Sliding full speed on belly
        SlideSlow = 3, ///< Sliding but slowing down
        StandUp = 4,   ///< Standing up from a slide
    };

    virtual void enterWalk();
    virtual void calcWalk();

    void calcRot();
    void calcPos();

    State m_state;         ///< The state of motion of the penguin
    EGG::Vector3f m_basis; ///< Orientation basis / forward direction
};

/// @brief Represents the penguins on N64 Sherbet Land that slide on their stomach
/// @details The durations of @ref State::Slide and @ref State::StandUp are determined by the
/// animation length of the corresponding CHR animation.
class ObjectPenguinS final : public ObjectPenguin {
public:
    ObjectPenguinS(const System::MapdataGeoObj &params);
    ~ObjectPenguinS() override;

    void init() override;
    void calc() override;
    void loadAnims() override;

    void enterWalk() override;
    void calcWalk() override;

private:
    void initAnmTimer();
    void calcRail();

    void enterDive();
    void enterSlide();
    void enterSlideSlow();
    void enterStandUp();

    void calcDive();
    void calcSlide();
    void calcStandUp();

    s32 m_anmTimer; ///< Frames remaining for the currently playing animation
};

} // namespace Kinoko::Field
