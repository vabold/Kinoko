#pragma once

#include "source/game/system/MultiDvdArchive.hh"

namespace System {

class ResourceManager {
public:
    class CourseArchive : public MultiDvdArchive {
    public:
        enum class State {
            Cleared = 0,
            Loading = 1,
            Loaded = 2,
        };

        CourseArchive();
        ~CourseArchive();

        void load(Course courseId);

    private:
        Course m_course;
        State m_state;
    };

    void *getFile(const char *filename, size_t *size, s32 idx);
    void *getBsp(u8 playerIdx, size_t *size);
    void load(Course courseId);
    MultiDvdArchive *load(s32 idx, const char *filename);

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
