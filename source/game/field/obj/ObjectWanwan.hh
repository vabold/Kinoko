#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief The wooden stake that the chain chomp is chained to.
class ObjectWanwanPile final : public ObjectCollidable {
public:
    /// @addr{Inlined in 0x806E4224}
    ObjectWanwanPile(const EGG::Vector3f &pos, const EGG::Vector3f &rot, const EGG::Vector3f &scale)
        : ObjectCollidable("pile", pos, rot, scale) {}

    /// @addr{0x806E9568}
    ~ObjectWanwanPile() override = default;

    /// @addr{0x806E9560}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806E9554}
    [[nodiscard]] const char *getResources() const override {
        return "wanwan";
    }

    /// @addr{0x806E9548}
    [[nodiscard]] const char *getKclName() const override {
        return "pile";
    }
};

/// @brief Represents Chain Chomps chained to a stake that attack in a limited arc.
class ObjectWanwan final : public ObjectCollidable, public StateManager {
public:
    ObjectWanwan(const System::MapdataGeoObj &params);
    ~ObjectWanwan() override;

    void init() override;
    void calc() override;

    /// @addr{0x806E94BC}
    [[nodiscard]] u32 loadFlags() const override {
        return 3;
    }

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

private:
    void enterWait();
    void enterAttack();
    void enterBack();

    void calcWait();
    void calcAttack();
    void calcBack();

    void calcPos();
    void calcCollision();
    void calcMat();
    void calcChainAttachMat();
    void calcSpeed();
    void calcBounce();

    /// @addr{0x806E79E4}
    void calcWanderTimer() {
        calcWanderEnd();
    }

    /// @addr{0x806E7B7C}
    void calcWanderEnd() {
        if (m_wanderTimer++ >= m_idleDuration) {
            m_nextStateId = 1;
        }
    }

    /// @addr{0x806E7BA4}
    void calcChain() {
        if (m_chainAttachMat.base(3).squaredLength() > std::numeric_limits<f32>::epsilon()) {
            calcChainAttachPos(m_chainAttachMat);
        }
    }

    void calcTangent(f32 t);
    void calcUp(f32 t);
    void calcRandomTarget();
    void initTransformKeyframes();
    void calcAttackPos();
    void calcChainAttachPos(EGG::Matrix34f mat);

    /// @addr{0x806B38A8}
    [[nodiscard]] static f32 CrossXZ(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            const EGG::Vector3f &v2) {
        return (v2.x - v1.x) * (v0.z - v1.z) - (v0.x - v1.x) * (v2.z - v1.z);
    }

    static void SampleHermiteInterp(f32 start, f32 end, f32 startTangent, f32 endTangent,
            std::span<f32> dst);

    EGG::Vector3f m_vel;
    EGG::Vector3f m_accel;
    f32 m_speed;
    f32 m_pitch;
    EGG::Vector3f m_tangent;
    EGG::Vector3f m_up;
    EGG::Vector3f m_targetUp;
    std::array<EGG::Matrix34f, 15> m_transformKeyframes;
    const f32 m_chainLength;
    const f32 m_attackDistance;
    const f32 m_attackArcTargetX;
    const f32 m_attackArcTargetZ;
    u32 m_chainCount;
    EGG::Vector3f m_chainAttachPos; ///< Where the chain attaches to the Chain Chomp
    EGG::Vector3f m_initPos;
    EGG::Vector3f m_anchor; ///< Effectively the position of the wooden stake
    EGG::Vector3f m_attackArcCenter;
    bool m_touchingFloor;
    bool m_chainTaut;       ///< Chain Chomp cannot move further because the chain is fully taut
    u32 m_frame;            ///< Counts up every frame
    EGG::Vector3f m_target; ///< Where the chain chomp is currently moving towards
    EGG::Vector3f m_targetDir;
    bool m_retarget;    ///< Tracks whether the target position has changed in the wait state
    u32 m_wanderTimer;  ///< How long the chain chomp has been wandering for
    bool m_attackStill; ///< True when lurched forward and stationary
    u32 m_idleDuration;
    f32 m_attackArc; ///< Half of size of the arc the chain chomp can lurch within (in degrees)
    EGG::Vector3f m_backDir;

    /// @brief Represents the position and rotation of where the Chain Chomp attaches to the chain.
    /// @details This is normally housed in the DrawMdl's ScnObj, but for the purposes of time trial
    /// physics, it can be defined here for simplicity.
    EGG::Matrix34f m_chainAttachMat;

    static constexpr f32 SCALE = 2.0f;
    static constexpr EGG::Vector3f GRAVITY = EGG::Vector3f(0.0f, 2.5f, 0.0f);
    static constexpr f32 CHAIN_LENGTH = 135.0f;

    static constexpr std::array<StateManagerEntry, 3> STATE_ENTRIES = {{
            {StateEntry<ObjectWanwan, &ObjectWanwan::enterWait, &ObjectWanwan::calcWait>(0)},
            {StateEntry<ObjectWanwan, &ObjectWanwan::enterAttack, &ObjectWanwan::calcAttack>(1)},
            {StateEntry<ObjectWanwan, &ObjectWanwan::enterBack, &ObjectWanwan::calcBack>(2)},
    }};
};

} // namespace Field
