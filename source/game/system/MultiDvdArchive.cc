#include "MultiDvdArchive.hh"

#include <cstring>

namespace System {

static size_t SUFFIX_SIZE = 8;

MultiDvdArchive::MultiDvdArchive(u16 archiveCount) : m_archiveCount(archiveCount) {
    m_archives = new DvdArchive[archiveCount];
    m_fileStarts = new void *[archiveCount];
    m_fileSizes = new size_t[archiveCount];
    m_suffixes = new char *[archiveCount];
    m_formats = new Format[archiveCount];

    for (u16 i = 0; i < m_archiveCount; i++) {
        m_fileStarts[i] = nullptr;
        m_fileSizes[i] = 0;
        m_suffixes[i] = new char[SUFFIX_SIZE];
        strncpy(m_suffixes[i], SZS_EXTENSION, SUFFIX_SIZE);
        m_formats[i] = Format::Double;
    }
}

MultiDvdArchive::~MultiDvdArchive() {
    delete[] m_archives;
    // WARN: Could lead to a memory leak if this is the only reference to the file!
    delete[] m_fileStarts;
    delete[] m_fileSizes;
    // WARN: Could lead to a memory leak if the pointer is not static!
    delete[] m_suffixes;
    delete[] m_formats;
}

void *MultiDvdArchive::getFile(const char *filename, size_t *size) const {
    void *file = nullptr;

    for (u16 i = m_archiveCount; i-- > 0; ) {
        const DvdArchive &archive = m_archives[i];

        if (!archive.isLoaded()) {
            continue;
        }

        file = archive.getFile(filename, size);
        if (file) {
            break;
        }
    }

    return file;
}

void MultiDvdArchive::load(const char *filename) {
    char buffer[256];

    for (u16 i = 0; i < m_archiveCount; i++) {
        switch (m_formats[i]) {
        case Format::Double:
            snprintf(buffer, sizeof(buffer), "%s%s", filename, m_suffixes[i]);
            break;
        case Format::Single:
            snprintf(buffer, sizeof(buffer), "%s", filename);
            break;
        case Format::None:
            break;
        default:
            continue;
        }

        if (m_formats[i] == Format::None) {
            m_archives[i].load(m_fileStarts[i], m_fileSizes[i], true);
        } else {
            m_archives[i].load(buffer, true);
        }
    }
}

void MultiDvdArchive::load(const MultiDvdArchive *other) {
    for (u16 i = 0; i < m_archiveCount; i++) {
        m_archives[i].load(&other->m_archives[i]);
    }
}

void MultiDvdArchive::rip(const char *filename) {
    char buffer[256];

    for (u16 i = 0; i < m_archiveCount; i++) {
        switch (m_formats[i]) {
        case Format::Double:
            snprintf(buffer, sizeof(buffer), "%s%s", filename, m_suffixes[i]);
            break;
        case Format::Single:
            snprintf(buffer, sizeof(buffer), "%s", filename);
            break;
        case Format::None:
            break;
        default:
            continue;
        }
        m_archives[i].rip(buffer);
    }
}

void MultiDvdArchive::clear() {
    for (u16 i = 0; i < m_archiveCount; i++) {
        m_archives[i].clear();
    }
}

void MultiDvdArchive::unmount() {
    for (u16 i = 0; i < m_archiveCount; i++) {
        m_archives[i].unmount();
    }
}

bool MultiDvdArchive::isLoaded() const {
    for (u16 i = 0; i < m_archiveCount; i++) {
        if (m_archives[i].isLoaded()) {
            return true;
        }
    }

    return false;
}

u16 MultiDvdArchive::rippedArchiveCount() const {
    u16 count = 0;
    for (u16 i = 0; i < m_archiveCount; i++) {
        if (m_archives[i].isRipped()) {
            count++;
        }
    }

    return count;
}

} // namespace System
