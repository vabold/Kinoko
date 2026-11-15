#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

class ObjectBirdLeader;
class ObjectBirdFollower;

/// @brief Represents a group of birds.
/// @details this class holds an @ref ObjectBirdLeader and a variable number of @ref
/// ObjectBirdFollower objects whose positions are offset from the leader's position.
/// @note This object must be implemented in Kinoko to prevent certain ghost replays from desyncing
/// on Toad's Factory. @ref ObjectBirdFollower::calc() performs a collision check between the birds
/// and the environment to prevent them from flying through floors. Because of this, birds flying
/// near @ref ObjectCrane instances can cause the crane's transformation matrix to update.
class ObjectBird final : public ObjectCollidable {
public:
    ObjectBird(const System::MapdataGeoObj &params);
    ~ObjectBird() override;

    void calc() override;

    void loadGraphics() override;
    void createCollision() override;
    void loadRail() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

    /// @beginGetters

    /// @brief Gets a pointer to the leader of the flock of birds
    /// @return A const pointer to the @ref ObjectBirdLeader of the flock, @ref m_leader
    [[nodiscard]] const ObjectBirdLeader *leader() const {
        return m_leader;
    }

    /// @brief Exposes the follower birds so that @ref ObjectBirdFollower can access them
    /// @return A const ref to the collection of follower birds, @ref m_followers
    [[nodiscard]] const auto &followers() const {
        return m_followers;
    }

    /// @endGetters

protected:
    ObjectBirdLeader *m_leader; ///< The main bird in the flock; other birds follow this leader
    owning_span<ObjectBirdFollower *> m_followers; ///< The other birds that follow the leader
};

/// @brief The main bird within an @ref ObjectBird flock. Other birds follow this leader.
class ObjectBirdLeader : public ObjectCollidable {
public:
    ObjectBirdLeader(const System::MapdataGeoObj &params, const ObjectBird *bird);
    ~ObjectBirdLeader() override;

    void init() override;
    void calc() override;
    void loadAnims() override;
    void createCollision() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

protected:
    const ObjectBird *const m_bird; ///< The @ref ObjectBird instance that this leader belongs to
};

/// @brief Represents all but one of the birds in an @ref ObjectBird group.
/// @details These birds initialize their position based off of the @ref ObjectBirdLeader. They
/// perform collision checks in their calc function to prevent them from flying through floors.
/// @note This object must be implemented in Kinoko to prevent certain ghost replays from desyncing
/// on Toad's Factory. @ref ObjectBirdFollower::calc() performs a collision check between the birds
/// and the environment to prevent them from flying through floors. Because of this, birds flying
/// near @ref ObjectCrane instances can cause the crane's transformation matrix to update.
class ObjectBirdFollower final : public ObjectBirdLeader {
public:
    ObjectBirdFollower(const System::MapdataGeoObj &params, const ObjectBird *bird, u32 idx);
    ~ObjectBirdFollower() override;

    void init() override;
    void calc() override;

private:
    void calcPos();

    const u32 m_idx;          ///< Index of this follower in the flock
    EGG::Vector3f m_velocity; ///< The current velocity of the follower bird
    const f32 m_baseSpeed;    ///< The base speed of the follower bird
};

} // namespace Kinoko::Field
