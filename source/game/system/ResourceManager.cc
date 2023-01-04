#include "ResourceManager.hh"

namespace System {

#define ARCHIVE_COUNT 2

static const char *const RESOURCE_PATHS[] = {
        "/Kinoko/Common",
        nullptr,
};

void *ResourceManager::getFile(const char *filename, size_t *size, s32 idx) {
    return m_archives[idx]->isLoaded() ? m_archives[idx]->getFile(filename, size) : nullptr;
}

void *ResourceManager::getBsp(u8 playerIdx, size_t *size) {
    // TODO: This is reliant on RaceConfig
    (void)playerIdx;
    (void)size;
    return nullptr;
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

void ResourceManager::load(Course courseId) {
    static_cast<CourseArchive *>(m_archives[1])->load(courseId);
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
    case 1:
        return new CourseArchive;
    default:
        return new MultiDvdArchive;
    }
}

ResourceManager::CourseArchive::CourseArchive()
    : m_course(Course::SNES_Mario_Circuit_3), m_state(State::Cleared) {}

ResourceManager::CourseArchive::~CourseArchive() {
    clear();
}

void ResourceManager::CourseArchive::load(Course courseId) {
    char buffer[128];

    m_course = courseId;
    if (m_state == State::Loaded) {
        clear();
    }

    m_state = State::Loading;
    snprintf(buffer, sizeof(buffer), "Kinoko/Course/%s", COURSE_NAMES[static_cast<u8>(courseId)]);
    rip(buffer);
    if (rippedArchiveCount() == 0) {
        clear();
        m_state = State::Cleared;
    } else {
        m_state = State::Loaded;
    }
}

ResourceManager *ResourceManager::s_instance = nullptr;

} // namespace System
