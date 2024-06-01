#pragma once

#include "game/system/MultiDvdArchive.hh"

namespace System {

enum class ArchiveId {
    Core = 0,
    Course = 1,
};

/// @addr{0x809BD738}
/// @brief Highest level abstraction for archive management and subsequent file retrieval.
/// @details ResourceManager is responsible for loading and unloading archives. For example, it is
/// used by Field::CourseColMgr to load the KCL collision file from a particular course archive.
class ResourceManager {
public:
    void *getFile(const char *filename, size_t *size, ArchiveId id);
    void *getBsp(Vehicle vehicle, size_t *size);
    [[nodiscard]] MultiDvdArchive *load(Course courseId);
    [[nodiscard]] MultiDvdArchive *load(s32 idx, const char *filename);
    void unmount(MultiDvdArchive *archive);

    [[nodiscard]] static const char *GetVehicleName(Vehicle vehicle);

    static ResourceManager *CreateInstance();
    static void DestroyInstance();
    [[nodiscard]] static ResourceManager *Instance();

private:
    ResourceManager();
    ~ResourceManager();

    // 0: Core archive
    // 1: Course archive
    MultiDvdArchive **m_archives;

    [[nodiscard]] static MultiDvdArchive *Create(u8 i);
    static ResourceManager *s_instance; ///< @addr{0x809BD738}
};

} // namespace System
