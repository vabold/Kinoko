/// TODO: Also need 806d2908

#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectHighwayManager;

class ObjectCarTGE : public ObjectCollidable, public StateManager {
public:
    enum class CarType {
        Normal = 0,
        Truck = 1,
        BombCar = 2, ///< Unused in time trials
    };

    ObjectCarTGE(const System::MapdataGeoObj &params);
    ~ObjectCarTGE() override;

    void init() override;
    void calc() override;

    /// @addr{0x806DA7AC}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806D68B0}
    [[nodiscard]] const char *getResources() const override {
        return m_carName;
    }

    /// @addr{0x806DA7A4}
    [[nodiscard]] const char *getKclName() const override {
        return m_mdlName;
    }

    void createCollision() override;
    void calcCollisionTransform() override;
    [[nodiscard]] f32 getCollisionRadius() const override;
    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;
    bool checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist) override;
    [[nodiscard]] const EGG::Vector3f &collisionCenter() const override;

    void setHighwayManager(const ObjectHighwayManager *highwayMgr) {
        m_highwayMgr = highwayMgr;
    }

    /// @addr{0x806D9A04}
    void reset() {
        m_squashed = false;
    }

    [[nodiscard]] bool squashed() const {
        return m_squashed;
    }

private:
    static constexpr f32 TOLL_BOOTH_ACCEL = 200.0f;

    void enterStateStub();
    void calcStateStub();
    void calcState1();
    void calcState2();

    void calcPos();

    const ObjectHighwayManager *m_highwayMgr;
    ObjectCollisionBase *m_auxCollision;
    const f32 m_highwayVel; ///< Speed while on the highway
    const f32 m_localVel;   ///< Speed while off the highway
    char m_carName[32];
    char m_mdlName[32];
    CarType m_carType; ///< Car, truck, or bomb car
    ObjectId m_dummyId;
    EGG::Vector3f m_scaledTangentDir;
    f32 m_currSpeed;
    EGG::Vector3f m_up;
    EGG::Vector3f m_tangent;
    bool m_squashed;
    bool m_hasAuxCollision;
    f32 m_hitAngle;

    static constexpr std::array<StateManagerEntry, 3> STATE_ENTRIES = {{
            {StateEntry<ObjectCarTGE, &ObjectCarTGE::enterStateStub, &ObjectCarTGE::calcStateStub>(
                    0)},
            {StateEntry<ObjectCarTGE, &ObjectCarTGE::enterStateStub, &ObjectCarTGE::calcState1>(1)},
            {StateEntry<ObjectCarTGE, &ObjectCarTGE::enterStateStub, &ObjectCarTGE::calcState2>(2)},
    }};
};

} // namespace Field
