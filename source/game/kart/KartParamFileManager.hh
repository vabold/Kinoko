#pragma once

#include "game/kart/KartParam.hh"

namespace Kart {

class KartParamFileManager {
public:
    void clear();
    void init();
    EGG::RamStream getDriverStream(Character character) const;
    EGG::RamStream getVehicleStream(Vehicle vehicle) const;
    EGG::RamStream getHitboxStream(Vehicle vehicle) const;
    EGG::RamStream getBikeDispParamsStream(Vehicle vehicle) const;

    static KartParamFileManager *CreateInstance();
    static void DestroyInstance();
    static KartParamFileManager *Instance();

private:
    template <typename T>
    struct ParamFile {
        u32 count;
        T params[];
    };

    struct FileInfo {
        void clear();
        void load(const char *filename);

        void *file;
        size_t size;
    };

    KartParamFileManager();
    ~KartParamFileManager();

    bool validate() const;

    FileInfo m_kartParam;     // kartParam.bin
    FileInfo m_driverParam;   // driverParam.bin
    FileInfo m_bikeDispParam; // bikePartsDispParam.bin

    static KartParamFileManager *s_instance;
};

} // namespace Kart
