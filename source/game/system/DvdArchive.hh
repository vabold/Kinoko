#pragma once

#include <egg/core/Archive.hh>

namespace System {

class DvdArchive {
public:
    enum class State {
        Cleared = 0,
        Ripped = 1,
        Decompressed = 2,
        Mounted = 3,
    };

    DvdArchive();
    ~DvdArchive();

    void decompress();
    void *getFile(const char *filename, size_t *size) const;
    void load(const char *path, bool decompress_);
    void load(const DvdArchive *other);
    void load(void *fileStart, size_t fileSize, bool decompress_);
    void mount();
    void move();
    void rip(const char *path);

    void clear();
    void clearArchive();
    void clearFile();
    void unmount();

    [[nodiscard]] bool isLoaded() const {
        return m_state == State::Mounted;
    }

    [[nodiscard]] bool isRipped() const {
        return m_state == State::Ripped;
    }

private:
    EGG::Archive *m_archive;
    void *m_archiveStart;
    size_t m_archiveSize;
    void *m_fileStart;
    size_t m_fileSize;
    State m_state;
};

} // namespace System
