#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectKuribo : public ObjectCollidable, public StateManager {
public:
    ObjectKuribo(const System::MapdataGeoObj &params);
    ~ObjectKuribo() override;

    void init() override;
    void calc() override;

    /// @addr{0x806DD2C8}
    [[nodiscard]] u32 loadFlags() const override {
        return 3;
    }

    void loadAnims() override;

private:
    void enterStateStub();
    void calcStateStub();
    void calcStateReroute();
    void calcStateWalk();

    void calcAnim();
    void calcRot();
    void checkSphereFull();

    f32 m_speedStep;
    f32 m_animStep;
    EGG::Vector3f m_origin;
    f32 m_maxAnimTimer;
    u32 m_frameCount;
    f32 m_currSpeed;
    EGG::Vector3f m_rot;
    EGG::Vector3f m_floorNrm;
    f32 m_animTimer;

    static constexpr std::array<StateManagerEntry, 4> STATE_ENTRIES = {{
            {StateEntry<ObjectKuribo, &ObjectKuribo::enterStateStub,
                    &ObjectKuribo::calcStateReroute>(0)},
            {StateEntry<ObjectKuribo, &ObjectKuribo::enterStateStub, &ObjectKuribo::calcStateWalk>(
                    1)},
            {StateEntry<ObjectKuribo, &ObjectKuribo::enterStateStub, &ObjectKuribo::calcStateStub>(
                    2)},
            {StateEntry<ObjectKuribo, &ObjectKuribo::enterStateStub, &ObjectKuribo::calcStateStub>(
                    3)},
    }};
};

} // namespace Field
