#pragma once

#include <egg/math/Matrix.hh>

namespace Kart {

/// @brief State management for most components of a kart's physics
/// @details Whenever another kart class is done with their calculations, they call to this class to
/// set the relevant variables. For example, KartMove::calcAcceleration() calculates acceleration
/// and subsequently sets the internal velocity in this class.
/// @nosubgrouping
class KartDynamics {
public:
    KartDynamics();
    virtual ~KartDynamics();

    virtual void forceUpright() {}
    virtual void stabilize();

    void init();

    void resetInternalVelocity() {
        m_intVel.setZero();
    }

    void setInertia(const EGG::Vector3f &m, const EGG::Vector3f &n);
    void setBspParams(f32 rotSpeed, const EGG::Vector3f &m, const EGG::Vector3f &n,
            bool skipInertia);

    void calc(f32 dt, f32 maxSpeed, bool air);
    void reset();

    void applySuspensionWrench(const EGG::Vector3f &p, const EGG::Vector3f &Flinear,
            const EGG::Vector3f &Frot, bool ignoreX);
    void applyWrenchScaled(const EGG::Vector3f &p, const EGG::Vector3f &f, f32 scale);

    /// @addr{0x805B6388}
    void addForce(const EGG::Vector3f &pos) {
        m_totalForce += pos;
    }

    /// @beginSetters
    void setPos(const EGG::Vector3f &pos) {
        m_pos = pos;
    }

    void setGravity(f32 gravity) {
        m_gravity = gravity;
    }

    void setMainRot(const EGG::Quatf &q) {
        m_mainRot = q;
    }

    void setFullRot(const EGG::Quatf &q) {
        m_fullRot = q;
    }

    void setSpecialRot(const EGG::Quatf &q) {
        m_specialRot = q;
    }

    void setExtraRot(const EGG::Quatf &q) {
        m_extraRot = q;
    }

    void setIntVel(const EGG::Vector3f &v) {
        m_intVel = v;
    }

    void setTop(const EGG::Vector3f &v) {
        m_top = v;
    }

    void setStabilizationFactor(f32 val) {
        m_stabilizationFactor = val;
    }

    void setTotalForce(const EGG::Vector3f &v) {
        m_totalForce = v;
    }

    void setExtVel(const EGG::Vector3f &v) {
        m_extVel = v;
    }

    void setAngVel0(const EGG::Vector3f &v) {
        m_angVel0 = v;
    }

    void setMovingObjVel(const EGG::Vector3f &v) {
        m_movingObjVel = v;
    }

    void setAngVel2(const EGG::Vector3f &v) {
        m_angVel2 = v;
    }

    void setAngVel0YFactor(f32 val) {
        m_angVel0YFactor = val;
    }

    void setTop_(const EGG::Vector3f &v) {
        m_top_ = v;
    }

    void setForceUpright(bool isSet) {
        m_forceUpright = isSet;
    }

    void setNoGravity(bool isSet) {
        m_noGravity = isSet;
    }

    void setKillExtVelY(bool isSet) {
        m_killExtVelY = isSet;
    }
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] const EGG::Matrix34f &invInertiaTensor() const {
        return m_invInertiaTensor;
    }

    [[nodiscard]] f32 angVel0Factor() const {
        return m_angVel0Factor;
    }

    [[nodiscard]] const EGG::Vector3f &pos() const {
        return m_pos;
    }

    [[nodiscard]] const EGG::Vector3f &velocity() const {
        return m_velocity;
    }

    [[nodiscard]] f32 gravity() const {
        return m_gravity;
    }

    [[nodiscard]] const EGG::Vector3f &intVel() const {
        return m_intVel;
    }

    [[nodiscard]] const EGG::Quatf &mainRot() const {
        return m_mainRot;
    }

    [[nodiscard]] const EGG::Quatf &fullRot() const {
        return m_fullRot;
    }

    [[nodiscard]] const EGG::Vector3f &totalForce() const {
        return m_totalForce;
    }

    [[nodiscard]] const EGG::Vector3f &extVel() const {
        return m_extVel;
    }

    [[nodiscard]] const EGG::Vector3f &angVel0() const {
        return m_angVel0;
    }

    [[nodiscard]] const EGG::Vector3f &movingObjVel() const {
        return m_movingObjVel;
    }

    [[nodiscard]] const EGG::Vector3f &angVel2() const {
        return m_angVel2;
    }

    [[nodiscard]] f32 speedFix() const {
        return m_speedFix;
    }
    /// @endGetters

protected:
    EGG::Matrix34f m_inertiaTensor;    ///< Resistance to rotational change, as a 3x3 matrix.
    EGG::Matrix34f m_invInertiaTensor; ///< The inverse of @ref m_inertiaTensor.
    f32 m_angVel0Factor;               ///< Scalar for damping angular velocity.
    EGG::Vector3f m_pos;               ///< The vehicle's position.
    EGG::Vector3f m_extVel;            ///< Velocity induced by collisions.
    EGG::Vector3f m_acceleration;      ///< Basically just @ref m_totalForce.
    EGG::Vector3f m_angVel0;           ///< Angular velocity from @ref m_totalTorque.
    EGG::Vector3f m_movingObjVel;      ///< Velocity from things like TF conveyers.
    EGG::Vector3f m_angVel1;           ///< @unused
    EGG::Vector3f m_movingRoadVel;     ///< Velocity from Koopa Cape water.
    EGG::Vector3f m_velocity;          ///< Sum of the linear velocities.
    f32 m_speedNorm;                   ///< Min of the max speed and @ref m_velocity magnitude.
    EGG::Vector3f m_angVel2;           ///< The main component of angular velocity.
    EGG::Quatf m_mainRot;              ///< Rotation based on the angular velocities.
    EGG::Quatf m_fullRot;              ///< The combination of the other rotations.
    EGG::Vector3f m_totalForce;        ///< Basically just gravity.
    EGG::Vector3f m_totalTorque;       ///< Torque from linear motion and rotation.
    EGG::Quatf m_specialRot;           ///< Rotation from trick animations. Copied from KartPhysics.
    EGG::Quatf m_extraRot;             ///< @unused
    f32 m_gravity;                     ///< Always -1.0f
    EGG::Vector3f m_intVel;            ///< What you typically consider to be the vehicle's speed.
    EGG::Vector3f m_top;               ///< The unit vector pointing up from the vehicle.
    f32 m_stabilizationFactor;         ///< Scalar for damping the main rotation.
    f32 m_speedFix;                    ///<
    EGG::Vector3f m_top_;              ///< Basically @ref m_top biased towards absolute up. @rename

    f32 m_angVel0YFactor; ///< Scalar for damping angular velocity.
    bool m_forceUpright;  ///< Specifies if we should return the vehicle to upwards orientation.
    bool m_noGravity;     ///< Disables gravity. Relevant when respawning.
    bool m_killExtVelY;   ///< Caps external velocity at 0.
};

/// @brief State management for most components of a bike's physics
/// @details Sharing the same members as KartDynamics, this class overrides some functions to
/// specifically handle bike physics.
class KartDynamicsBike : public KartDynamics {
public:
    KartDynamicsBike();
    ~KartDynamicsBike();

private:
    void forceUpright() override;
    void stabilize() override;
};

} // namespace Kart
