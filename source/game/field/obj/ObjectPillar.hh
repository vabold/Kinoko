
#pragma once

#include "game/field/obj/ObjectCollidable.hh"
#include "game/field/obj/ObjectKCL.hh"

namespace Kinoko::Field {

/// @brief The stationary base of the Dry Dry Ruins pillars
/// @details Simply acts as wall collision.
class ObjectPillarBase final : public ObjectKCL {
public:
    ObjectPillarBase(const System::MapdataGeoObj &params);
    ~ObjectPillarBase() override;

    [[nodiscard]] const char *getKclName() const override;
};

/// @brief Represents the part of the Dry Dry Ruins pillar that falls
/// @details Acts as a wall before the pillar starts to fall, acts as a hazard while falling, and
/// disables once the pillar has fallen.
class ObjectPillarC final : public ObjectCollidable {
public:
    ObjectPillarC(const System::MapdataGeoObj &params);
    ~ObjectPillarC() override;

    void calcCollisionTransform() override;
    [[nodiscard]] f32 getCollisionRadius() const override;
    [[nodiscard]] ObjectId id() const override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

private:
    const u32 m_fallStart; ///< The number of frames before the pillar will start to fall
};

/// @brief Represents the entirety of a pillar that falls on Dry Dry Ruins
/// @details There are three parts that comprise a pillar: the upright/falling pillar collision
/// (@ref ObjectPillarC), the pillar base collision (@ref ObjectPillarBase), and the trickable
/// pillar KCL once it has fallen (represented with this parent class). The pillar starts to fall
/// after @ref m_fallStart frames. The duration of the fall depends on @ref m_targetRot.
class ObjectPillar final : public ObjectKCL {
public:
    ObjectPillar(const System::MapdataGeoObj &params);
    ~ObjectPillar() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] f32 colRadiusAdditionalLength() const override;
    [[nodiscard]] const EGG::Matrix34f &getUpdatedMatrix(u32 timeOffset) override;

private:
    /// @brief The state of the pillar in terms of whether it is falling or not
    enum class State {
        Upright = 0, ///< The pillar is upright and has not yet started to fall
        Break = 1,   ///< The pillar is falling and can be collided with as a hazard
        Ground = 2,  ///< The pillar has finished falling and can now be driven over
    };

    [[nodiscard]] f32 calcRot(s32 frame) const;

    State m_state;               ///< Falling state of the pillar
    const u32 m_fallStart;       ///< The number of frames before the pillar will start to fall
    const f32 m_targetRot;       ///< How much the pillar rotates during the fall, in radians
    const f32 m_initRotX;        ///< Initial X-axis rotation of the pillar
    EGG::Vector3f m_initRot;     ///< Initial rotation of the pillar
    ObjectPillarBase *m_base;    ///< Stationary portion of pillar
    ObjectPillarC *m_collidable; ///< Wall and hazard collision of the upright/falling pillar
    EGG::Matrix34f m_workMat;    ///< Rotation and translation matrix
    s32 m_groundFrame;           ///< Frame the pillar has finished falling
};

} // namespace Kinoko::Field
