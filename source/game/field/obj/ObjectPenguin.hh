#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief The base class for penguins on N64 Sherbet Land.
/// @details In the base game, the cave penguins derive from this class, but they do not override
/// any physics-dependent virtual functions. For Kinoko, we simply construct these penguins as a
/// base class instance. The cave penguins walk along a rail, always facing perpendicular to the
/// tangent line.
class ObjectPenguin : public ObjectCollidable {
public:
    ObjectPenguin(const System::MapdataGeoObj &params);
    ~ObjectPenguin() override;

    void init() override;

    /// @addr{0x80775764}
    void calc() override {
        if (m_state == State::Walk) {
            calcWalk();
        }
    }

    /// @addr{0x80777324}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x807757A0}
    virtual void calcWalk() {
        m_railInterpolator->calc();
        calcPos();
        calcRot();
    }

    void calcRot();

    /// @addr{0x80775C2C}
    virtual void calcPos() {
        setPos(m_railInterpolator->curPos());
    }

    virtual void enterWalk();

protected:
    enum class State {
        Walk = 0,
        Dive = 1,
        Slider = 2,
        SliderSlow = 3,
        StandUp = 4,
    };

    State m_state;
    EGG::Vector3f m_basis;
};

/// @brief Represents the penguins on N64 Sherbet Land that slide on their stomach.
class ObjectPenguinS final : public ObjectPenguin {
public:
    ObjectPenguinS(const System::MapdataGeoObj &params);
    ~ObjectPenguinS() override;

    void init() override;
    void calc() override;

    void loadAnims() override;

    /// @addr{0x8077637C}
    void calcWalk() override {
        calcRail();
        calcPos();
        calcRot();
    }

    /// @addr{0x80776AF0}
    void enterWalk() override {
        m_state = State::Walk;
    }

private:
    /// @addr{0x807763D0}
    void calcDive() {
        calcWalk();
    }

    void calcSlider();
    void calcStandUp();
    void calcRail();

    s32 m_anmTimer;
};

} // namespace Field
