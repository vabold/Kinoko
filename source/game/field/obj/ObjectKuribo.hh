#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectKuribo;

class ObjectKuribo : public ObjectCollidable, public StateManager<ObjectKuribo> {
    friend StateManager<ObjectKuribo>;

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
    EGG::Vector3f interpolate(f32 scale, const EGG::Vector3f &v0, const EGG::Vector3f &v1) const;

    f32 m_speedStep;
    f32 m_animStep;
    EGG::Vector3f m_origin;
    f32 m_maxAnimTimer;
    u32 m_frameCount;
    f32 m_currSpeed;
    EGG::Vector3f m_rot;
    EGG::Vector3f m_floorNrm;
    f32 m_animTimer;

    static constexpr std::array<StateManagerEntry<ObjectKuribo>, 4> STATE_ENTRIES = {{
            {0, &ObjectKuribo::enterStateStub, &ObjectKuribo::calcStateReroute},
            {1, &ObjectKuribo::enterStateStub, &ObjectKuribo::calcStateWalk},
            {2, &ObjectKuribo::enterStateStub, &ObjectKuribo::calcStateStub},
            {3, &ObjectKuribo::enterStateStub, &ObjectKuribo::calcStateStub},
    }};
};

} // namespace Field
