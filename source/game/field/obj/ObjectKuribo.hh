#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectKuribo;

template <>
class StateManager<ObjectKuribo> : public StateManagerBase<ObjectKuribo> {
public:
    StateManager(ObjectKuribo *obj);
    ~StateManager() override;

private:
    static const std::array<StateManagerEntry<ObjectKuribo>, 4> STATE_ENTRIES;
};

class ObjectKuribo : public ObjectCollidable, public StateManager<ObjectKuribo> {
public:
    ObjectKuribo(const System::MapdataGeoObj &params);
    ~ObjectKuribo() override;

    void init() override;
    void calc() override;
    [[nodiscard]] u32 loadFlags() const override;
    void loadAnims() override;

    void enterStateStub();
    void calcStateStub();
    void calcState0();
    void calcState1();

private:
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
};

} // namespace Field
