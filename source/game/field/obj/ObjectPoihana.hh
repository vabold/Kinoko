#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Base class for the Cataquacks on GCN Peach Beach
/// @details It is not clear why this is separate from @ref ObjectPoihana, as no other class is
/// derived from it. We separate them in Kinoko in order to more closely match the base game's code
/// structure.
class ObjectPoihanaBase : public ObjectCollidable, virtual protected StateManager {
public:
    ObjectPoihanaBase(const System::MapdataGeoObj &params);
    ~ObjectPoihanaBase() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

protected:
    /// @brief Whether the Cataquack has a target position to walk towards or needs one
    enum class WalkState {
        NeedTarget = 0, ///< The Cataquack needs to select a new target position to walk towards
        HasTarget = 1,  ///< The Cataquack has a target position to walk towards
    };

    WalkState m_walkState;     ///< Whether the Cataquack has a target to walk towards or needs one
    EGG::Vector3f m_up;        ///< Smoothed up vector for the Cataquack's orientation
    EGG::Vector3f m_forward;   ///< Smoothed forward vector for the Cataquack's orientation
    EGG::Matrix34f m_workMat;  ///< The current frame's transformation matrix
    EGG::Vector3f m_accel;     ///< Acceleration vector
    EGG::Vector3f m_extVel;    ///< Velocity incurred due to acceleration
    EGG::Vector3f m_vel;       ///< Smoothed velocity vector based on the target velocity
    EGG::Vector3f m_floorNrm;  ///< The normal of the floor surface the Cataquack is currently on
    f32 m_heightOffset;        ///< Accounts for Cataquack leg height
    f32 m_radius;              ///< Radius of the Cataquack's collision sphere
    EGG::Vector3f m_targetPos; ///< The position the Cataquack is currently walking towards
    EGG::Vector3f m_dir;       ///< Direction between the current position and the target position
    EGG::Vector3f m_initPos;   ///< Position of the Cataquack during initialization
    EGG::Vector3f m_targetVel; ///< Derived from m_targetPos and current position
};

/// @brief Represents the Cataquacks on GCN Peach Beach
/// @details Cataquacks walk towards a pseudo-random target position within 1000 to 2000 units of
/// their initial position. Once within a threshold of 200 units from the target position, the
/// Cataquack will compute a new target position.
class ObjectPoihana final : public ObjectPoihanaBase {
public:
    ObjectPoihana(const System::MapdataGeoObj &params);
    ~ObjectPoihana() override;

    void init() override;

private:
    void enterWalk();
    void calcWalk();
    [[nodiscard]] EGG::Vector3f curPos() const;
    void calcUp(f32 t);
    void calcOrthonormalBasis();
    void calcCollision();
    void calcStep();
    void calcDir();
    void calcForward();
    [[nodiscard]] EGG::Vector3f collisionPos() const;
    void calcPosAndCollision();

    [[nodiscard]] static EGG::Vector3f Project(const EGG::Vector3f &v0, const EGG::Vector3f &v1);

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 1> STATE_ENTRIES = {{
            {StateEntry<ObjectPoihana, &ObjectPoihana::enterWalk, &ObjectPoihana::calcWalk>(0)},
    }};
};

} // namespace Kinoko::Field
