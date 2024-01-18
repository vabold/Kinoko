#include "ResourceManager.hh"

#include "game/system/RaceConfig.hh"

namespace System {

#define ARCHIVE_COUNT 2

static const char *const RESOURCE_PATHS[] = {
        "/Kinoko/Common",
        nullptr,
};

void *ResourceManager::getFile(const char *filename, size_t *size, s32 idx) {
    return m_archives[idx]->isLoaded() ? m_archives[idx]->getFile(filename, size) : nullptr;
}

void *ResourceManager::getBsp(Vehicle vehicle, size_t *size) {
    char buffer[32];

    const char *name = GetVehicleName(vehicle);
    snprintf(buffer, sizeof(buffer), "/bsp/%s.bsp", name);

    return m_archives[0]->isLoaded() ? m_archives[0]->getFile(buffer, size) : nullptr;
}

MultiDvdArchive *ResourceManager::load(s32 idx, const char *filename) {
    // Course has a dedicated load function, so we do not want it here
    assert(idx != 1);

    if (!filename) {
        filename = RESOURCE_PATHS[idx];
    }

    if (!m_archives[idx]->isLoaded() && filename) {
        m_archives[idx]->load(filename);
    }

    return m_archives[idx];
}

MultiDvdArchive *ResourceManager::load(Course courseId) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Kinoko/Course/%s", COURSE_NAMES[static_cast<s32>(courseId)]);
    m_archives[1]->load(buffer);
    return m_archives[1];
}

const char *ResourceManager::GetVehicleName(Vehicle vehicle) {
    return vehicle < Vehicle::Max ? VEHICLE_NAMES[static_cast<u8>(vehicle)] : nullptr;
}

ResourceManager *ResourceManager::CreateInstance() {
    assert(!s_instance);
    s_instance = new ResourceManager;
    return s_instance;
}

void ResourceManager::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

ResourceManager *ResourceManager::Instance() {
    return s_instance;
}

ResourceManager::ResourceManager() {
    m_archives = new MultiDvdArchive *[ARCHIVE_COUNT];
    for (u8 i = 0; i < ARCHIVE_COUNT; i++) {
        m_archives[i] = Create(i);
    }
}

ResourceManager::~ResourceManager() = default;

MultiDvdArchive *ResourceManager::Create(u8 i) {
    switch (i) {
    default:
        return new MultiDvdArchive;
    }
}

ResourceManager *ResourceManager::s_instance = nullptr;

} // namespace System
