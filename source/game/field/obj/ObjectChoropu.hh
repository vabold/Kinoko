#pragma once

#include "game/field/ObjectCollisionSphere.hh"
#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

#include "game/kart/KartCollide.hh"

#include <vector>

namespace Field {

class ObjectChoropuGround;
class ObjectChoropuHoll;

/// @brief Represents the MMM and rPG monty moles.
/// @details Each mole has an associated "holl" [sic]. Moles which move around (MMM) also have an
/// associated "ground" (the dirt trail).
class ObjectChoropu : public ObjectCollidable, public StateManager {
public:
    ObjectChoropu(const System::MapdataGeoObj &params);
    ~ObjectChoropu() override;

    void init() override;
    void calc() override;

    /// @addr{0x806BBE34}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

private:
    void enterStateStub();
    void enterDigging();
    void enterPeeking();
    void enterJumping();
    void calcStateStub();
    void calcDigging();
    void calcPeeking();
    void calcJumping();

    void calcGround();
    void calcGroundObjs();
    [[nodiscard]] EGG::Matrix34f calcInterpolatedPose(f32 t) const;

    std::span<ObjectChoropuGround *> m_groundObjs;
    ObjectChoropuHoll *m_objHoll;
    const s16 m_startFrameOffset;
    const s16 m_idleDuration;
    f32 m_groundHeight;
    const bool m_isStationary; ///< rPG moles don't move while MMM moles do
    EGG::Matrix34f m_transMat;
    EGG::Matrix34f m_railMat;
    f32 m_groundLength;

    static constexpr f32 RADIUS = 300.0f;
    static constexpr f32 MAX_GROUND_LEN = 3000.0f; ///< Max length of the dirt trail

    static constexpr std::array<StateManagerEntry, 5> STATE_ENTRIES = {{
            StateEntry<ObjectChoropu, &ObjectChoropu::enterDigging, &ObjectChoropu::calcDigging>(0),
            StateEntry<ObjectChoropu, &ObjectChoropu::enterPeeking, &ObjectChoropu::calcPeeking>(1),
            StateEntry<ObjectChoropu, &ObjectChoropu::enterStateStub,
                    &ObjectChoropu::calcStateStub>(2),
            StateEntry<ObjectChoropu, &ObjectChoropu::enterJumping, &ObjectChoropu::calcJumping>(3),
            StateEntry<ObjectChoropu, &ObjectChoropu::enterStateStub,
                    &ObjectChoropu::calcStateStub>(4),
    }};
};

class ObjectChoropuGround : public ObjectCollidable {
public:
    ObjectChoropuGround(const EGG::Vector3f &pos, const EGG::Vector3f &rot,
            const EGG::Vector3f &scale);
    ~ObjectChoropuGround() override;

    /// @addr{0x806BBEB0}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806B9164}
    Kart::Reaction onCollision(Kart::KartObject * /*kartObj*/, Kart::Reaction reactionOnKart,
            Kart::Reaction /*reactionOnObj*/, EGG::Vector3f &hitDepth) override {
        hitDepth.setZero();
        return reactionOnKart;
    }

    void calcPosAndMat(f32 height, const EGG::Matrix34f &mat);

    [[nodiscard]] f32 height() const {
        return m_height;
    }

private:
    f32 m_height;
};

class ObjectChoropuHoll : public ObjectCollidable {
public:
    ObjectChoropuHoll(const System::MapdataGeoObj &params);
    ~ObjectChoropuHoll() override;

    /// @addr{0x806B94A0}
    void init() override {
        resize(RADIUS, 0.0f);
    }

    /// @addr{0x806BBE4C}
    [[nodiscard]] const char *getName() const override {
        return "holl";
    }

    /// @addr{0x806BBE64}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806BBE58}
    [[nodiscard]] const char *getKclName() const override {
        return "holl";
    }

    /// @addr{0x806B9428}
    void createCollision() override {
        m_collision = new ObjectCollisionSphere(RADIUS, EGG::Vector3f::zero);
    }

    /// @addr{0x806BBE40}
    void loadRail() override {}

    /// @addr{0x806B9594}
    Kart::Reaction onCollision(Kart::KartObject * /*kartObj*/, Kart::Reaction /*reactionOnKart*/,
            Kart::Reaction /*reactionOnObj*/, EGG::Vector3f &hitDepth) override {
        hitDepth.setZero();
        return Kart::Reaction::Wall;
    }

private:
    static constexpr f32 RADIUS = 300.0f;
};

} // namespace Field
