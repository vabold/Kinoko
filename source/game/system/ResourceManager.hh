#pragma once

#include "game/system/MultiDvdArchive.hh"

namespace System {

enum class ArchiveId {
    Core = 0,
    Course = 1,
};

class ResourceManager {
public:
    void *getFile(const char *filename, size_t *size, ArchiveId id);
    void *getBsp(Vehicle vehicle, size_t *size);
    MultiDvdArchive *load(Course courseId);
    MultiDvdArchive *load(s32 idx, const char *filename);

    static const char *GetVehicleName(Vehicle vehicle);

    static ResourceManager *CreateInstance();
    static void DestroyInstance();
    static ResourceManager *Instance();

private:
    ResourceManager();
    ~ResourceManager();

    // 0: Core archive
    // 1: Course archive
    MultiDvdArchive **m_archives;

    static MultiDvdArchive *Create(u8 i);
    static ResourceManager *s_instance;
};

} // namespace System
