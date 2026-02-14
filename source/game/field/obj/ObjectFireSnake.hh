#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectProjectile.hh"

namespace Field {

class ObjectFireSnakeKid : public ObjectCollidable {
public:
    ObjectFireSnakeKid(const System::MapdataGeoObj &params);
    ~ObjectFireSnakeKid() override;

    /// @addr{0x806C2B60}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806C296C}
    void setTransform(const EGG::Matrix34f &mat) {
        m_flags.setBit(eFlags::Matrix);
        m_transform = mat;
        m_pos = mat.base(3);
    }
};

class ObjectFireSnake : public ObjectProjectile, virtual public StateManager {
public:
    ObjectFireSnake(const System::MapdataGeoObj &params);
    ~ObjectFireSnake() override;

    void init() override;
    void calc() override;

    /// @addr{0x806C2A5C}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    void initProjectile(const EGG::Vector3f &pos) override;
    void onLaunch() override;

    void enterDespawned();
    void enterFalling();
    void enterHighBounce();
    void enterRest();
    void enterBounce() {}
    void enterDespawning() {}

    void calcDespawned() {}
    void calcFalling();
    void calcHighBounce();
    void calcRest();
    void calcBounce();
    void calcDespawning() {}

protected:
    void calcChildren();

    EGG::Vector3f m_sunPos;
    EGG::Vector3f m_initialPos;
    EGG::Vector3f m_initRot;
    EGG::Vector3f m_visualPos;
    EGG::Vector3f m_bounceDir;
    u16 m_age; ///< How long the firesnake has been spawned
    s16 m_delayFrame;

private:
    void calcBounce(f32 initialVel);

    bool isCollisionEnabled() const {
        return m_currentStateId == 2 || m_currentStateId == 3 || m_currentStateId == 4;
    }

    std::array<ObjectFireSnakeKid *, 2> m_kids;
    const s16 m_maxAge; ///< Number of frames until the snake will disappear
    EGG::Vector3f m_xzSunDist;
    EGG::Vector3f m_fallAxis;
    f32 m_xzSpeed;
    u16 m_fallDuration;                              ///< How long the firesnake falls from the sun
    std::array<EGG::Matrix34f, 21> m_prevTransforms; ///< The last 21 transformation matrices

    static constexpr std::array<StateManagerEntry, 6> STATE_ENTRIES = {{
            {StateEntry<ObjectFireSnake, &ObjectFireSnake::enterDespawned,
                    &ObjectFireSnake::calcDespawned>(0)},
            {StateEntry<ObjectFireSnake, &ObjectFireSnake::enterFalling,
                    &ObjectFireSnake::calcFalling>(1)},
            {StateEntry<ObjectFireSnake, &ObjectFireSnake::enterHighBounce,
                    &ObjectFireSnake::calcHighBounce>(2)},
            {StateEntry<ObjectFireSnake, &ObjectFireSnake::enterRest, &ObjectFireSnake::calcRest>(
                    3)},
            {StateEntry<ObjectFireSnake, &ObjectFireSnake::enterBounce,
                    &ObjectFireSnake::calcBounce>(4)},
            {StateEntry<ObjectFireSnake, &ObjectFireSnake::enterDespawning,
                    &ObjectFireSnake::calcDespawning>(5)},
    }};

    static constexpr f32 GRAVITY = 3.0f;
};

} // namespace Field
