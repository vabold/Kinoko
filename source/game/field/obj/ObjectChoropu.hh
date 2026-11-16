#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko {

namespace Kart {

enum class Reaction;

} // namespace Kart

namespace Field {

class ObjectChoropuGround;
class ObjectChoropuHoll;

/// @brief Represents the Moo Moo Meadows and DS Peach Gardens monty moles.
/// @details Each mole has an associated @ref ObjectChoropuHoll that it pops out from. Moles which
/// move around (MMM) also have an associated @ref ObjectChoropuGround (the dirt trail).
class ObjectChoropu final : public ObjectCollidable, private StateManager {
public:
    ObjectChoropu(const System::MapdataGeoObj &params);
    ~ObjectChoropu() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

private:
    void enterDigging();
    void enterPeeking();
    void enterJumping();

    void calcDigging();
    void calcPeeking();
    void calcJumping();

    void calcGround();
    void calcGroundObjs();
    [[nodiscard]] EGG::Matrix34f calcInterpolatedPose(f32 t) const;
    [[nodiscard]] f32 calcJumpHeight() const;

    owning_span<ObjectChoropuGround *> m_groundObjs; ///< Dirt trail segments behind moles on MMM
    ObjectChoropuHoll *m_objHoll;                    ///< The hole the mole emerges from
    const s16 m_startFrameOffset; ///< Initial delay before the mole starts its behavior cycle
    const u16 m_idleDuration;     ///< Frames that stationary moles stay underground before peeking
    f32 m_groundHeight; ///< Height of a single dirt trail cylinder. Used to space out segments.
    const bool m_isStationary; ///< rPG moles don't move while MMM moles do
    EGG::Matrix34f m_initRt;   ///< Initial transform for stationary moles
    EGG::Vector3f m_railPos;   ///< Moving mole's current position along the rail
    f32 m_groundLength;        ///< Cumulative length of dirt trail behind moving monty moles on MMM

    static constexpr f32 RADIUS = 300.0f; ///< Radius of the dirt trail segments' collision sphere
    static constexpr f32 MAX_GROUND_LEN = 3000.0f; ///< Max length of the dirt trail

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 5> STATE_ENTRIES = {{
            StateEntry<ObjectChoropu, &ObjectChoropu::enterDigging, &ObjectChoropu::calcDigging>(0),
            StateEntry<ObjectChoropu, &ObjectChoropu::enterPeeking, &ObjectChoropu::calcPeeking>(1),
            StateEntry<ObjectChoropu, nullptr, nullptr>(2),
            StateEntry<ObjectChoropu, &ObjectChoropu::enterJumping, &ObjectChoropu::calcJumping>(3),
            StateEntry<ObjectChoropu, nullptr, nullptr>(4),
    }};
};

/// @brief A dirt trail segment left behind by the moving monty moles on Moo Moo Meadows
class ObjectChoropuGround final : public ObjectCollidable {
public:
    ObjectChoropuGround(const EGG::Vector3f &pos, const EGG::Vector3f &rot,
            const EGG::Vector3f &scale);
    ~ObjectChoropuGround() override;

    [[nodiscard]] LoadFlags loadFlags() const override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    void calcPosAndMat(f32 height, const EGG::Matrix34f &mat);

    /// @beginGetters

    /// @brief Gets the current height of the dirt trail segment
    /// @return The current height of the dirt trail segment, @ref m_height
    [[nodiscard]] f32 height() const {
        return m_height;
    }

    /// @endGetters

private:
    f32 m_height; ///< Height of the dirt trail segment. Used to space out segments.
};

/// @brief The hole that the monty mole pops out from
class ObjectChoropuHoll final : public ObjectCollidable {
public:
    ObjectChoropuHoll(const System::MapdataGeoObj &params);
    ~ObjectChoropuHoll() override;

    void init() override;
    void createCollision() override;
    void loadRail() override;
    [[nodiscard]] const char *getName() const override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getKclName() const override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

private:
    static constexpr f32 RADIUS = 300.0f; ///< Radius of the collision sphere for the mole's hole
};

} // namespace Field

} // namespace Kinoko
