#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectCrab final : public ObjectCollidable {
public:
    ObjectCrab(const System::MapdataGeoObj &params);
    ~ObjectCrab() override;

    void init() override;
    void calc() override;

    /// @addr{0x808864DC}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

private:
    enum class State {
        Walking = 0,
        Still = 1,
        Deactivated = 2,
        Resurfacing = 3,
    };

    enum class StatePhase {
        Start = 0,
        Middle = 1,
        End = 2,
    };

    enum class StateResult {
        Walking = 0,
        Middle = 1,
        BeginWalking = 2,
    };

    bool calcRail();
    StateResult calcState();

    void calcCurRot(const EGG::Vector3f &rot);
    void calcTransMat(const EGG::Vector3f &rot);

    const f32 m_vel;
    u32 m_stillDuration;
    u32 m_stillFrame;
    bool m_still;
    const bool m_backwards;
    EGG::Vector3f m_curRot;
    State m_state;
    StatePhase m_statePhase;

    // In the base game, there is a m_timer member variable at offset 0xcc which ends up resulting
    // in the calc function being run only once during the intro timer.
    bool m_introCalc;

    static constexpr f32 YAW = F_PI / 2.0f;
    STATIC_ASSERT(YAW == 1.5707964f);
    static constexpr EGG::Vector3f INIT_ROT = EGG::Vector3f(0.0f, YAW, 0.0f);

    static constexpr u32 COUNT = 3;
};

} // namespace Field
