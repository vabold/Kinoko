#pragma once

#include "game/system/DvdArchive.hh"

const char *const SZS_EXTENSION = ".szs";

namespace System {

class MultiDvdArchive {
public:
    enum class Format {
        Double,
        Single,
        None,
    };

    MultiDvdArchive(u16 archiveCount = 1);
    ~MultiDvdArchive();

    void *getFile(const char *filename, size_t *size) const;
    void load(const char *filename);
    void load(const MultiDvdArchive *other);
    void rip(const char *filename);

    void clear();
    void unmount();

    bool isLoaded() const;
    u16 rippedArchiveCount() const;

private:
    DvdArchive *m_archives;
    void **m_fileStarts;
    size_t *m_fileSizes;
    char **m_suffixes;
    Format *m_formats;
    u16 m_archiveCount;
};

} // namespace System
