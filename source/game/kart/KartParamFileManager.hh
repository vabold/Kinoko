#pragma once

#include "game/kart/KartParam.hh"

namespace Kart {

/// @brief Abstraction for the process of retrieving kart parameters from files.
/// @details This has been modified from the base game in order to perform validation and make the
/// class accessible as a singleton.
class KartParamFileManager {
public:
    void clear();
    void init();
    [[nodiscard]] EGG::RamStream getDriverStream(Character character) const;
    [[nodiscard]] EGG::RamStream getVehicleStream(Vehicle vehicle) const;
    [[nodiscard]] EGG::RamStream getHitboxStream(Vehicle vehicle) const;
    [[nodiscard]] EGG::RamStream getBikeDispParamsStream(Vehicle vehicle) const;

    static KartParamFileManager *CreateInstance();
    static void DestroyInstance();
    [[nodiscard]] static KartParamFileManager *Instance();

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

    [[nodiscard]] bool validate() const;

    FileInfo m_kartParam;     // kartParam.bin
    FileInfo m_driverParam;   // driverParam.bin
    FileInfo m_bikeDispParam; // bikePartsDispParam.bin

    static KartParamFileManager *s_instance;
};

} // namespace Kart
