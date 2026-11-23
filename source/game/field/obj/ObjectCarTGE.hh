#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

class ObjectHighwayManager;

/// @brief Represents a vehicle (car, truck, or bomb car) on Moonview Highway
/// @details Interfaces with @ref ObjectHighwayManager to enforce a squish cooldown for the player.
/// Bomb cars are not present in Time Trial mode and are thus not implemented in Kinoko. Each
/// vehicle has two GJK collision primitives in order for the collision detection to better reflect
/// the shape of the vehicle.
class ObjectCarTGE final : public ObjectCollidable, private StateManager {
public:
    /// @brief The type of vehicle represented by the object
    enum class CarType {
        Normal = 0,  ///< A regular car
        Truck = 1,   ///< A truck
        BombCar = 2, ///< Bob-omb car, unused in time trials
    };

    ObjectCarTGE(const System::MapdataGeoObj &params);
    ~ObjectCarTGE() override;

    void init() override;
    void calc() override;
    void createCollision() override;
    void calcCollisionTransform() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getResources() const override;
    [[nodiscard]] const char *getKclName() const override;
    [[nodiscard]] f32 getCollisionRadius() const override;
    [[nodiscard]] const EGG::Vector3f &collisionCenter() const override;
    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;
    bool checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist) override;

    /// @beginSetters

    /// @brief Sets the @ref ObjectHighwayManager associated with this object
    /// @param highwayMgr The highway manager to oversee this object
    void setHighwayManager(const ObjectHighwayManager *highwayMgr) {
        m_highwayMgr = highwayMgr;
    }

    /// @addr{0x806D9A04}
    /// @brief Resets @ref m_squashed to `false`
    /// @details This is called by @ref ObjectHighwayManager on all vehicles immediately after a
    /// squash with any one car occurs, so that it can enforce the squish cooldown for the player.
    void reset() {
        m_squashed = false;
    }

    /// @endSetters

    /// @beginGetters

    /// @brief Checks if this vehicle has squashed the player this frame
    /// @return The value of @ref m_squashed
    [[nodiscard]] bool squashed() const {
        return m_squashed;
    }

    /// @endGetters

private:
    void calcSpeedup();
    void calcSlowdown();
    void calcPos();
    void calcState();

    const ObjectHighwayManager *m_highwayMgr; ///< Manager that handles squish cooldowns
    ObjectCollisionBase *m_auxCollision; ///< Secondary collision cylinder for more accurate shape
    const f32 m_highwaySpeed;            ///< Speed while on the highway
    const f32 m_localSpeed;              ///< Speed while off the highway
    char m_carName[32];                  ///< Resource (.brres) name
    char m_mdlName[32];                  ///< Model/KCL name
    CarType m_carType;                   ///< Car, truck, or bomb car
    ObjectId m_dummyId;                  ///< Dummy id (CarBody or KartTruck) used for hit reaction
    EGG::Vector3f m_vel;                 ///< Rail tangent scaled by current speed
    f32 m_currSpeed;                     ///< Current speed of the vehicle along the rail
    EGG::Vector3f m_tangent;             ///< Smoothed forward direction along the rail
    bool m_squashed;                     ///< Set if this vehicle squashed player in last 200 frames
    bool m_hasAuxCollision; ///< Set when a collision was the result of @ref m_auxCollision
    f32 m_hitAngle; ///< Angular threshold for determining if player should be launched or squished

    /// @brief Rate of speed increase when a vehicle enters and exits the highway
    static constexpr f32 TOLL_BOOTH_ACCEL = 200.0f;

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 3> STATE_ENTRIES = {{
            {StateEntry<ObjectCarTGE, nullptr, nullptr>(0)},
            {StateEntry<ObjectCarTGE, nullptr, &ObjectCarTGE::calcSpeedup>(1)},
            {StateEntry<ObjectCarTGE, nullptr, &ObjectCarTGE::calcSlowdown>(2)},
    }};
};

} // namespace Kinoko::Field
