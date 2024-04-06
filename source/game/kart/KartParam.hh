#pragma once

#include <egg/math/Vector.hh>

namespace Kart {

struct BSP {
    struct Hitbox {
        u16 enable;
        EGG::Vector3f position;
        f32 radius;
        u16 wallsOnly;
        u16 tireCollisionIdx;
    };
    static_assert(sizeof(Hitbox) == 0x18);

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
    static_assert(sizeof(Wheel) == 0x2c);

    BSP();
    BSP(EGG::RamStream &stream);

    void read(EGG::RamStream &stream);

    f32 initialYPos;
    std::array<Hitbox, 16> hitboxes;
    EGG::Vector3f cuboids[2];
    f32 angVel0Factor;
    f32 _1a0;
    std::array<Wheel, 4> wheels;
    f32 rumbleHeight;
    f32 rumbleSpeed;
};
static_assert(sizeof(BSP) == 0x25c);

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
    static_assert(sizeof(BikeDisp) == 0xB0);

    struct Stats {
        enum class Body {
            Four_Wheel_Kart = 0,       // Used by most karts
            Handle_Relative_Bike = 1,  // Used by most bikes
            Vehicle_Relative_Bike = 2, // Used by Quacker
            Three_Wheel_Kart = 3,      // Used by Blue Falcon
        };

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
        f32 _00c; // Unused
        f32 weight;
        f32 bumpDeviationLevel;
        f32 speed;
        f32 turningSpeed;
        f32 tilt;
        std::array<f32, 4> accelerationStandardA;
        std::array<f32, 3> accelerationStandardT;
        std::array<f32, 2> accelerationDriftA;
        std::array<f32, 1> accelerationDriftT;
        f32 handlingManualTightness;
        f32 handlingAutomaticTightness;
        f32 handlingReactivity;
        f32 driftManualTightness;
        f32 driftAutomaticTightness;
        f32 driftReactivity;
        f32 driftOutsideTargetAngle;
        f32 driftOutsideDecrement;
        u32 miniTurbo;
        std::array<f32, 32> kclSpeed;
        std::array<f32, 32> kclRot;
        f32 itemUnk170;
        f32 itemUnk174;
        f32 itemUnk178;
        f32 itemUnk17c;
        f32 maxNormalAcceleration;
        f32 megaScale;
        f32 wheelDistance;
    };
    static_assert(sizeof(Stats) == 0x18c);

    KartParam(Character character, Vehicle vehicle, u8 playerIdx);
    ~KartParam();

    const BSP &bsp() const;
    const Stats &stats() const;
    const BikeDisp &bikeDisp() const;
    u8 playerIdx() const;
    bool isBike() const;
    u16 suspCount() const;
    u16 tireCount() const;

    void setTireCount(u16 tireCount);
    void setSuspCount(u16 suspCount);

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
