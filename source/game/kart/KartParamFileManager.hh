#pragma once

#include "game/kart/KartParam.hh"

namespace Kart {

class KartParamFileManager {
public:
    void clear();
    void init();
    EGG::RamStream getDriverStream(Character character) const;
    EGG::RamStream getVehicleStream(Vehicle vehicle) const;

    static KartParamFileManager *CreateInstance();
    static void DestroyInstance();
    static KartParamFileManager *Instance();

private:
    struct ParamFile {
        u32 m_count;
        KartParam::Stats m_params[];
    };

    struct FileInfo {
        void clear();
        void load(const char *filename);

        void *m_file;
        size_t m_size;
    };

    KartParamFileManager();
    ~KartParamFileManager();

    bool validate() const;

    FileInfo m_kartParam;   // kartParam.bin
    FileInfo m_driverParam; // driverParam.bin

    static KartParamFileManager *s_instance;
};

} // namespace Kart
