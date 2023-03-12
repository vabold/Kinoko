#pragma once

#include <Common.hh>

namespace Kart {

class KartParam {
public:
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

        enum class WeightClass {
            Light = 0,
            Medium = 1,
            Heavy = 2,
        };

        Stats operator+(const Stats &ext);
        void read(Stats &raw);

        Body m_body;
        DriftType m_driftType;
        WeightClass m_weightClass;
        f32 _00c; // Unused
        f32 m_weight;
        f32 m_bumpDeviationLevel;
        f32 m_speed;
        f32 m_turningSpeed;
        f32 m_tilt;
        f32 m_accelerationStandardA[4];
        f32 m_accelerationStandardT[3];
        f32 m_accelerationDriftA[2];
        f32 m_accelerationDriftT[1];
        f32 m_handlingManualTightness;
        f32 m_handlingAutomaticTightness;
        f32 m_handlingReactivity;
        f32 m_driftManualTightness;
        f32 m_driftAutomaticTightness;
        f32 m_driftReactivity;
        f32 m_driftOutsideTargetAngle;
        f32 m_driftOutsideDecrement;
        u32 m_miniTurbo;
        f32 m_kclSpeed[32];
        f32 m_kclRot[32];
        f32 m_itemUnk170;
        f32 m_itemUnk174;
        f32 m_itemUnk178;
        f32 m_itemUnk17c;
        f32 m_maxNormalAcceleration;
        f32 m_megaScale;
        f32 m_wheelDistance;
    };

    static_assert(sizeof(Stats) == 0x18c);
    
    struct Col {
        struct Hibox {
            u16 m_enableFlag;
            u16  m_padding;
            EGG::Vector3f m_sphereCenterPos;
            f32  m_sphereRadius;
            u16 m_wallCollideOnly;
            u16 m_collisionIndex;
        };

        struct Wheel {
            u16 m_enableFlag;
            f32 m_suspensionFactorPos;      // "multiplies the distance between the current position and the bottommost position"
            f32 m_suspensionFactorSpeed;    // "multiplies the wheel speed on the vertical axis"
            f32 m_wheelDistanceVert;
            EGG::Vector3f m_wheelCenterPos;
            f32 m_wheelXrot;                // "always zero"
            f32 m_wheelRadius;
            f32 m_sphereRadiusCollision;
        };
    };
    
    static_assert(sizeof(Col) == 0x25c);
    
    KartParam(Character character, Vehicle vehicle);
    ~KartParam();

private:
    void initStats(Character character, Vehicle vehicle);
    void initHitboxes(Vehicle vehicle);

    Stats m_stats;
    Col m_Col;
};

class KartParamFileManager {
public:
    void clear();
    void init();
    void stats(KartParam::Stats &stats, Character character);
    void stats(KartParam::Stats &stats, Vehicle vehicle);

    static KartParamFileManager *CreateInstance();
    static void DestroyInstance();
    static KartParamFileManager *Instance();

private:
    struct ParamFile {
        u32 m_count;
        KartParam::Stats m_params[];
    };

    KartParamFileManager();
    ~KartParamFileManager();

    struct FileInfo {
        void clear();
        void load(const char *filename);

        void *m_file;
        size_t m_size;
    };

    bool validate();

    FileInfo m_kartParam;   // kartParam.bin
    FileInfo m_driverParam; // driverParam.bin

    static KartParamFileManager *s_instance;
};

} // namespace Kart
