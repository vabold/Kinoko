#pragma once

#include "game/field/obj/ObjectCollidable.hh"

#include "game/system/RaceManager.hh"

namespace Field {

class ObjectBirdLeader;
class ObjectBirdFollower;

/// @brief Represents a group of birds.
/// @details ObjectBird holds an @ref ObjectBirdLeader and a variable number of @ref
/// ObjectBirdFollower objects whose positions are offset from the leader's position.
class ObjectBird final : public ObjectCollidable {
public:
    ObjectBird(const System::MapdataGeoObj &params);
    ~ObjectBird() override;

    void calc() override;

    /// @addr{0x8077CCF4}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x8077CCF0}
    void loadGraphics() override {}

    /// @addr{0x8077CCE8}
    void createCollision() override {}

    /// @ADDR{0X8077CCE0}
    void loadRail() override {}

    [[nodiscard]] const ObjectBirdLeader *leader() const {
        return m_leader;
    }

    [[nodiscard]] const auto &followers() const {
        return m_followers;
    }

protected:
    ObjectBirdLeader *m_leader;
    std::span<ObjectBirdFollower *> m_followers;
};

/// @brief The main bird within an @ref ObjectBird. Other birds follow this leader.
class ObjectBirdLeader : public ObjectCollidable {
public:
    ObjectBirdLeader(const System::MapdataGeoObj &params, ObjectBird *bird);
    ~ObjectBirdLeader() override;

    void init() override;
    void calc() override;

    /// @addr{0x8077CCD4}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    void loadAnims() override;

    /// @brief Not overridden in the base game, but collision mode 0 will cause our assert to fail.
    void createCollision() override {}

protected:
    ObjectBird *m_bird;
};

/// @brief Represents all but one of the birds in an @ObjectBird group.
/// @details These birds initialize their position based off of the @ref ObjectBirdLeader. They
/// perform collision checks in their calc function to prevent them from flying through floors. We
/// have to implement this class because it can induce a collision transformation matrix update when
/// birds are in close proximity to a moving @ref ObjectKCL.
class ObjectBirdFollower final : public ObjectBirdLeader {
public:
    ObjectBirdFollower(const System::MapdataGeoObj &params, ObjectBird *bird, u32 idx);
    ~ObjectBirdFollower() override;

    void init() override;
    void calc() override;

private:
    void calcPos();

    const u32 m_idx;
    EGG::Vector3f m_velocity;
    f32 m_baseSpeed;
};

} // namespace Field
