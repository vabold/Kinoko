#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectPoihanaBase : public ObjectCollidable, virtual public StateManager {
public:
    ObjectPoihanaBase(const System::MapdataGeoObj &params);
    ~ObjectPoihanaBase() override;

    void init() override;

    /// @addr{0x807472F4}
    void calc() override {
        StateManager::calc();
    }

    /// @addr{0x8074815C}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

protected:
    enum class WalkState {
        NeedTarget = 0,
        HasTarget = 1,
    };

    WalkState m_walkState;
    EGG::Vector3f m_curRot;
    EGG::Vector3f m_up;
    EGG::Matrix34f m_workMat;
    EGG::Vector3f m_accel;
    EGG::Vector3f m_extVel;
    EGG::Vector3f m_vel;
    EGG::Vector3f m_floorNrm;
    f32 m_heightOffset; ///< Accounts for cataquack leg height
    f32 m_radius;
    EGG::Vector3f m_targetPos;
    EGG::Vector3f m_dir;
    EGG::Vector3f m_initPos;
    EGG::Vector3f m_targetVel; ///< Derived from m_targetPos and current position
};

/// @brief Represents the cataquacks on GCN Peach Beach.
class ObjectPoihana final : public ObjectPoihanaBase {
public:
    ObjectPoihana(const System::MapdataGeoObj &params);
    ~ObjectPoihana() override;

    void init() override;

    /// @addr{0x80748B70}
    void calc() override {
        ObjectPoihanaBase::calc();
    }

private:
    void enterStateStub() {}

    /// @addr{0x807498AC}
    void enterState0() {
        m_walkState = WalkState::NeedTarget;
    }

    void calcStateStub() {}

    /// @addr{0x80749924}
    void calcState0And1() {
        calcStep();
        calcPosAndCollision();
        calcOrthonormalBasis();
        calcPosAndTransform();
    }

    /// @addr{0x80747308}
    [[nodiscard]] EGG::Vector3f curPos() const {
        return m_workMat.base(3);
    }

    void calcCurRot(f32 t);
    void calcOrthonormalBasis();
    void calcCollision();
    void calcStep();
    void calcDir();
    void calcUp();

    /// @addr{0x80747324}
    [[nodiscard]] EGG::Vector3f collisionPos() const {
        return curPos() + m_floorNrm * m_heightOffset;
    }

    /// @addr{0x80747404}
    void calcPosAndTransform() {
        m_flags.setBit(eFlags::Matrix);
        m_transform = m_workMat;
        m_pos = m_workMat.base(3);
    }

    /// @addr{0x807476D8}
    void calcPosAndCollision() {
        m_extVel += m_accel;
        m_workMat.setBase(3, m_workMat.base(3) + (m_extVel + m_vel));
        calcCollision();
    }

    /// @addr{0x8074B0AC}
    [[nodiscard]] static EGG::Vector3f Project(const EGG::Vector3f &v0, const EGG::Vector3f &v1) {
        f32 scale = (v0.z * v1.z + (v0.x * v1.x + v0.y * v1.y)) /
                EGG::Mathf::fma(v1.z, v1.z, v1.x * v1.x + v1.y * v1.y);

        return v1 * scale;
    }

    static constexpr std::array<StateManagerEntry, 8> STATE_ENTRIES = {{
            {StateEntry<ObjectPoihana, &ObjectPoihana::enterState0, &ObjectPoihana::calcState0And1>(
                    0)},
            {StateEntry<ObjectPoihana, &ObjectPoihana::enterStateStub,
                    &ObjectPoihana::calcState0And1>(1)},
            {StateEntry<ObjectPoihana, &ObjectPoihana::enterStateStub,
                    &ObjectPoihana::calcStateStub>(2)},
            {StateEntry<ObjectPoihana, &ObjectPoihana::enterStateStub,
                    &ObjectPoihana::calcStateStub>(3)},
            {StateEntry<ObjectPoihana, &ObjectPoihana::enterStateStub,
                    &ObjectPoihana::calcStateStub>(4)},
            {StateEntry<ObjectPoihana, &ObjectPoihana::enterStateStub,
                    &ObjectPoihana::calcStateStub>(5)},
            {StateEntry<ObjectPoihana, &ObjectPoihana::enterStateStub,
                    &ObjectPoihana::calcStateStub>(6)},
            {StateEntry<ObjectPoihana, &ObjectPoihana::enterStateStub,
                    &ObjectPoihana::calcStateStub>(7)},
    }};
};

} // namespace Field
