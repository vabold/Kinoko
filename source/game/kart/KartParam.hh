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

        Body m_body;
        DriftType m_driftType;
        WeightClass m_weightClass;
        f32 _00c;
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

    void clear();
    void init();

    static KartParam *CreateInstance();
    static void DestroyInstance();
    static KartParam *Instance();

private:
    KartParam();
    ~KartParam();

    struct FileInfo {
        void clear();
        void load(const char *filename);

        void *m_file;
        size_t m_size;
    };

    bool validate();

    FileInfo m_kartParam;   // kartParam.bin
    FileInfo m_driverParam; // driverParam.bin

    static KartParam *s_instance;
};

} // namespace Kart
