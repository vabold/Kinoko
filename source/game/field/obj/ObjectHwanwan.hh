#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartObject.hh"

namespace Field {

class ObjectHwanwan : public ObjectCollidable, public StateManager {
    friend class ObjectHwanwanManager;

public:
    ObjectHwanwan(const System::MapdataGeoObj &params);
    ~ObjectHwanwan() override;

    void init() override;
    void calc() override;

    /// @addr{0x806EC7B8
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806EC7AC}
    [[nodiscard]] const char *getKclName() const override {
        return "wanwan";
    }

    /// @addr{0x806EC7A8}
    void loadRail() override {}

    /// @addr{0x806E9CAC}
    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) override {
        return kartObj->speedRatioCapped() < 0.5f ? Kart::Reaction::WallAllSpeed : reactionOnKart;
    }

private:
    /// @addr{0x806E9E04}
    void enterState0() {}

    /// @addr{0x806E9FE0}
    void enterState1() {}

    /// @addr{0x806E9E10}
    void calcState0() {
        if (m_touchingGround) {
            m_bounceVel += EGG::Vector3f::ey * 12.0f;
        }
    }

    /// @addr{0x806E9FE4}
    void calcState1() {}

    void checkFloorCollision();
    void calcUp();

    const EGG::Vector3f m_initPos;
    EGG::Vector3f m_workPos;
    EGG::Vector3f m_extVel;
    EGG::Vector3f m_bounceVel;
    EGG::Vector3f m_tangent;
    EGG::Vector3f m_up;
    EGG::Vector3f m_targetUp;
    bool m_touchingGround;
    f32 m_initJumpVel;
    f32 m_targetY;

    static constexpr f32 DIAMETER = 400.0f;
    static constexpr EGG::Vector3f GRAVITY = EGG::Vector3f(0.0f, 2.5f, 0.0f);

    static constexpr std::array<StateManagerEntry, 2> STATE_ENTRIES = {{
            {StateEntry<ObjectHwanwan, &ObjectHwanwan::enterState0, &ObjectHwanwan::calcState0>(0)},
            {StateEntry<ObjectHwanwan, &ObjectHwanwan::enterState1, &ObjectHwanwan::calcState1>(1)},
    }};
};

class ObjectHwanwanManager : public ObjectCollidable {
public:
    ObjectHwanwanManager(const System::MapdataGeoObj &params);
    ~ObjectHwanwanManager() override;

    void init() override;
    void calc() override;

    /// @addr{0x806C69B8}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806C69B4}
    void loadGraphics() override {}

    /// @addr{0x806C69B0}
    void createCollision() override {}

private:
    void calcState();

    ObjectHwanwan *m_hwanwan;
};

} // namespace Field
