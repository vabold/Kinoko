#pragma once

#include <egg/math/Vector.hh>

namespace Kart {

/// @brief Houses hitbox and wheel positions, radii, and suspension info.
struct BSP {
    /// @brief Represents one of the many hitboxes that make up a vehicle.
    struct Hitbox {
        u16 enable; ///< Specifies if this is an active hitbox (since BSP always has 16).
        EGG::Vector3f position; ///< The relative position of the hitbox.
        f32 radius;
        u16 wallsOnly;
        u16 tireCollisionIdx;
    };
    STATIC_ASSERT(sizeof(Hitbox) == 0x18);

    /// @brief Info pertaining to the suspension, position, etc. of a wheel.
    struct Wheel {
        u16 enable;
        f32 springStiffness;
        f32 dampingFactor;
        f32 maxTravel;
        EGG::Vector3f relPosition;
        f32 xRot;
        f32 wheelRadius;
        f32 sphereRadius;
        u32 _28;
    };
    STATIC_ASSERT(sizeof(Wheel) == 0x2c);

    BSP();
    BSP(EGG::RamStream &stream);

    void read(EGG::RamStream &stream);

    f32 initialYPos;
    std::array<Hitbox, 16> hitboxes; ///< Array of vehicle hitboxes, not all of which are active.
    EGG::Vector3f cuboids[2];        ///< Mask cuboids for computing moment of inertia.
    f32 angVel0Factor;
    f32 _1a0;
    std::array<Wheel, 4> wheels;
    f32 rumbleHeight; ///< Max vertical distance of the vehicle body's rumble animation.
    u16 rumbleSpeed;  ///< Speed of the vehicle body's rumble animation.
};
STATIC_ASSERT(sizeof(BSP) == 0x25c);

/// @brief Houses stats regarding a given character/vehicle combo.
/// @nosubgrouping
class KartParam {
public:
    struct BikeDisp {
        BikeDisp();
        BikeDisp(EGG::RamStream &stream);

        void read(EGG::RamStream &stream);

        u8 _00[0x0c - 0x00];
        EGG::Vector3f m_handlePos;
        EGG::Vector3f m_handleRot;
        u8 _24[0xb0 - 0x24];
    };
    STATIC_ASSERT(sizeof(BikeDisp) == 0xB0);

    /// @brief Various character/vehicle-related handling and speed stats.
    struct Stats {
        /// @brief The body style of the vehicle. Basically the number of wheels.
        enum class Body {
            Four_Wheel_Kart = 0,       ///< Used by most karts
            Handle_Relative_Bike = 1,  ///< Used by most bikes
            Vehicle_Relative_Bike = 2, ///< Used by Quacker
            Three_Wheel_Kart = 3,      ///< Used by Blue Falcon
        };

        /// @brief The type of drift (inside/outside).
        enum class DriftType {
            Outside_Drift_Kart = 0,
            Outside_Drift_Bike = 1,
            Inside_Drift_Bike = 2,
        };

        Stats();
        Stats(EGG::RamStream &stream);

        void read(EGG::RamStream &stream);
        void applyCharacterBonus(EGG::RamStream &stream);

        Body body;
        DriftType driftType;
        WeightClass weightClass;
        f32 _00c; ///< @unused

        f32 weight; ///< Contrary to popular belief, this does not affect gravity.
        f32 bumpDeviationLevel;
        f32 speed;        ///< Base full speed of the character/vehicle combo.
        f32 turningSpeed; ///< Speed decrement percentage of the vehicle when handling.
        f32 tilt;
        std::array<f32, 4> accelerationStandardA;
        std::array<f32, 3> accelerationStandardT;
        std::array<f32, 2> accelerationDriftA;
        std::array<f32, 1> accelerationDriftT;
        f32 handlingManualTightness;    ///< Affects turn radius when manual and not drifting.
        f32 handlingAutomaticTightness; ///< Affects turn radius when auto and not drifting.
        f32 handlingReactivity;         ///< A weight applied to turn radius when not drifting.
        f32 driftManualTightness;       ///< Affects turn radius when manual drifting.
        f32 driftAutomaticTightness;    ///< Affects turn radius when automatic drifting.
        f32 driftReactivity;            ///< A weight applied to turn radius when drifting.
        f32 driftOutsideTargetAngle;
        f32 driftOutsideDecrement;
        u32 miniTurbo;                ///< The framecount duration of a charged mini-turbo.
        std::array<f32, 32> kclSpeed; ///< Speed multipliers, indexed using KCL attributes.
        std::array<f32, 32> kclRot;   ///< Rotation scalars, indexed using KCL attributes.
        f32 itemUnk170;               ///< @unused
        f32 itemUnk174;               ///< @unused
        f32 itemUnk178;               ///< @unused
        f32 itemUnk17c;               ///< @unused
        f32 maxNormalAcceleration;
        f32 megaScale;
        f32 wheelDistance;
    };
    STATIC_ASSERT(sizeof(Stats) == 0x18c);

    KartParam(Character character, Vehicle vehicle, u8 playerIdx);
    ~KartParam();

    /// @beginSetters
    void setTireCount(u16 tireCount) {
        m_tireCount = tireCount;
    }

    void setSuspCount(u16 suspCount) {
        m_suspCount = suspCount;
    }
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] const BSP &bsp() const {
        return m_bsp;
    }

    [[nodiscard]] const Stats &stats() const {
        return m_stats;
    }

    [[nodiscard]] const BikeDisp &bikeDisp() const {
        return m_bikeDisp;
    }

    [[nodiscard]] u8 playerIdx() const {
        return m_playerIdx;
    }

    [[nodiscard]] bool isBike() const {
        return m_isBike;
    }

    [[nodiscard]] bool isVehicleRelativeBike() const {
        return m_stats.body == Stats::Body::Vehicle_Relative_Bike;
    }

    [[nodiscard]] u16 suspCount() const {
        return m_suspCount;
    }

    [[nodiscard]] u16 tireCount() const {
        return m_tireCount;
    }
    /// @endGetters

private:
    void initStats(Character character, Vehicle vehicle);
    void initBikeDispParams(Vehicle vehicle);
    void initHitboxes(Vehicle vehicle);

    Stats m_stats;
    BikeDisp m_bikeDisp;
    BSP m_bsp;
    u8 m_playerIdx;
    bool m_isBike;
    u16 m_suspCount;
    u16 m_tireCount;
};

} // namespace Kart
