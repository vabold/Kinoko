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
    void setPos(const EGG::Vector3f &pos);
    void setVelocity(const EGG::Vector3f &vel);
    void set_fc(f32 val);
    void composeStuntRot(const EGG::Quatf &rot);
    void composeDecayingRot(const EGG::Quatf &rot);
    void clearDecayingRot();
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] KartDynamics *dynamics();
    [[nodiscard]] const KartDynamics *dynamics() const;
    [[nodiscard]] const EGG::Matrix34f &pose() const;
    [[nodiscard]] CollisionGroup *hitboxGroup();
    [[nodiscard]] const EGG::Vector3f &xAxis() const;
    [[nodiscard]] const EGG::Vector3f &yAxis() const;
    [[nodiscard]] const EGG::Vector3f &zAxis() const;
    [[nodiscard]] const EGG::Vector3f &pos() const;
    [[nodiscard]] f32 fc() const;
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
    EGG::Matrix34f m_pose;    ///< The kart's current rotation and position.
    EGG::Vector3f m_xAxis;    ///< The first column of the pose.
    EGG::Vector3f m_yAxis;    ///< The second column of the pose.
    EGG::Vector3f m_zAxis;    ///< The third column of the pose.
    EGG::Vector3f m_velocity; ///< Copied from KartDynamics.
    f32 m_fc;                 /// @rename
};

} // namespace Kart
