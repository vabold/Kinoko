#pragma once

#include "game/kart/CollisionGroup.hh"
#include "game/kart/KartDynamics.hh"
#include "game/kart/KartParam.hh"

#include <egg/math/Matrix.hh>

namespace Kart {

/// @brief Manages the lifecycle of KartDynamics, handles moving floors and trick rotation.
/// @nosubgrouping
class KartPhysics {
public:
    KartPhysics(bool isBike);
    ~KartPhysics();

    void reset();
    void updatePose();

    void calc(f32 dt, f32 maxSpeed, const EGG::Vector3f &scale, bool air);

    /// @beginSetters
    void setPos(const EGG::Vector3f &pos) {
        m_pos = pos;
    }

    void setVelocity(const EGG::Vector3f &vel) {
        m_velocity = vel;
    }

    void set_fc(f32 val) {
        m_fc = val;
    }

    /// @addr{0x8059FC48}
    void composeStuntRot(const EGG::Quatf &rot) {
        m_instantaneousStuntRot *= rot;
    }

    /// @addr{0x8059FD0C}
    void composeExtraRot(const EGG::Quatf &rot) {
        m_instantaneousExtraRot *= rot;
    }

    /// @addr{0x8059FDD0}
    void composeDecayingStuntRot(const EGG::Quatf &rot) {
        m_decayingStuntRot *= rot;
    }

    /// @addr{0x8059FE94}
    void composeDecayingExtraRot(const EGG::Quatf &rot) {
        m_decayingExtraRot *= rot;
    }

    /// @addr{0x805A0050}
    void composeMovingObjVel(const EGG::Vector3f &v, f32 scalar) {
        m_movingObjVel += (v - m_movingObjVel) * scalar;
        dynamics()->setMovingObjVel(m_movingObjVel);
    }

    /// @addr{0x805A00D0}
    void composeDecayingMovingObjVel(f32 floorScalar, f32 airScalar, bool floor) {
        m_movingObjVel *= floor ? floorScalar : airScalar;
        dynamics()->setMovingObjVel(m_movingObjVel);
    }

    /// @addr{0x805A0410}
    void clearDecayingRot() {
        m_decayingStuntRot = EGG::Quatf::ident;
        m_decayingExtraRot = EGG::Quatf::ident;
    }
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] KartDynamics *dynamics() {
        return m_dynamics;
    }

    [[nodiscard]] const KartDynamics *dynamics() const {
        return m_dynamics;
    }

    [[nodiscard]] const EGG::Matrix34f &pose() const {
        return m_pose;
    }

    [[nodiscard]] CollisionGroup *hitboxGroup() {
        return m_hitboxGroup;
    }

    [[nodiscard]] const EGG::Vector3f &xAxis() const {
        return m_xAxis;
    }

    [[nodiscard]] const EGG::Vector3f &yAxis() const {
        return m_yAxis;
    }

    [[nodiscard]] const EGG::Vector3f &zAxis() const {
        return m_zAxis;
    }

    [[nodiscard]] const EGG::Vector3f &pos() const {
        return m_pos;
    }

    [[nodiscard]] f32 fc() const {
        return m_fc;
    }
    /// @endGetters

    [[nodiscard]] static KartPhysics *Create(const KartParam &param);

private:
    KartDynamics *m_dynamics;
    CollisionGroup *m_hitboxGroup;
    EGG::Vector3f m_pos;
    EGG::Quatf m_decayingStuntRot;
    EGG::Quatf m_instantaneousStuntRot;
    EGG::Quatf m_specialRot;
    /// @brief Rotation that occurs when landing from a trick.
    EGG::Quatf m_decayingExtraRot;
    EGG::Quatf m_instantaneousExtraRot;
    EGG::Quatf m_extraRot;
    EGG::Vector3f m_movingObjVel;
    EGG::Matrix34f m_pose;    ///< The kart's current rotation and position.
    EGG::Vector3f m_xAxis;    ///< The first column of the pose.
    EGG::Vector3f m_yAxis;    ///< The second column of the pose.
    EGG::Vector3f m_zAxis;    ///< The third column of the pose.
    EGG::Vector3f m_velocity; ///< Copied from KartDynamics.
    f32 m_fc;                 /// @rename
};

} // namespace Kart
