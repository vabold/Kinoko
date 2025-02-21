#include "ResourceManager.hh"

#include "game/system/RaceConfig.hh"

namespace System {

#define ARCHIVE_COUNT 2

static const char *const RESOURCE_PATHS[] = {
        "/Race/Common",
        nullptr,
};

/// @addr{0x805411FC}
void *ResourceManager::getFile(const char *filename, size_t *size, ArchiveId id) {
    s32 idx = static_cast<s32>(id);
    return m_archives[idx]->isLoaded() ? m_archives[idx]->getFile(filename, size) : nullptr;
}

/// @addr{0x805414A8}
void *ResourceManager::getBsp(Vehicle vehicle, size_t *size) {
    char buffer[32];

    const char *name = GetVehicleName(vehicle);
    snprintf(buffer, sizeof(buffer), "/bsp/%s.bsp", name);

    return m_archives[0]->isLoaded() ? m_archives[0]->getFile(buffer, size) : nullptr;
}

/// @addr{0x80540450}
MultiDvdArchive *ResourceManager::load(s32 idx, const char *filename) {
    // Course has a dedicated load function, so we do not want it here
    ASSERT(idx != 1);

    if (!filename) {
        filename = RESOURCE_PATHS[idx];
    }

    if (!m_archives[idx]->isLoaded() && filename) {
        m_archives[idx]->load(filename);
    }

    return m_archives[idx];
}

/// @addr{0x80540760}
MultiDvdArchive *ResourceManager::load(Course courseId) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Race/Course/%s", COURSE_NAMES[static_cast<s32>(courseId)]);
    m_archives[1]->load(buffer);
    return m_archives[1];
}

/// @addr{0x805411E4}
void ResourceManager::unmount(MultiDvdArchive *archive) {
    archive->unmount();
}

/// @addr{0x8053FC4C}
ResourceManager *ResourceManager::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new ResourceManager;
    return s_instance;
}

/// @addr{0x8053FC9C}
void ResourceManager::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

/// @addr{0x8053FCEC}
ResourceManager::ResourceManager() {
    m_archives = new MultiDvdArchive *[ARCHIVE_COUNT];
    for (u8 i = 0; i < ARCHIVE_COUNT; i++) {
        m_archives[i] = Create(i);
    }
}

/// @addr{0x8053FF1C}
ResourceManager::~ResourceManager() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("ResourceManager instance not explicitly handled!");
    }
}

/// @addr{Inlined in 0x8053FCEC}
MultiDvdArchive *ResourceManager::Create(u8 i) {
    switch (i) {
    default:
        return new MultiDvdArchive;
    }
}

ResourceManager *ResourceManager::s_instance = nullptr;

} // namespace System
