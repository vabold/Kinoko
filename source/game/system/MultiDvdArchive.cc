#include "MultiDvdArchive.hh"

#include <cstring>

namespace Kinoko::System {

static size_t SUFFIX_SIZE = 8;

/// @addr{0x8052A538}
MultiDvdArchive::MultiDvdArchive(u16 archiveCount) : m_archiveCount(archiveCount) {
    m_archives = EGG::egg_new_array<DvdArchive>(archiveCount);
    m_fileStarts = static_cast<void **>(EGG::egg_alloc(archiveCount * sizeof(void *)));
    m_fileSizes = static_cast<size_t *>(EGG::egg_alloc(archiveCount * sizeof(size_t)));
    m_suffixes = static_cast<char **>(EGG::egg_alloc(archiveCount * sizeof(char *)));
    m_formats = EGG::egg_new_array<Format>(archiveCount);

    for (u16 i = 0; i < m_archiveCount; i++) {
        m_fileStarts[i] = nullptr;
        m_fileSizes[i] = 0;
        m_suffixes[i] = static_cast<char *>(EGG::egg_alloc(SUFFIX_SIZE));
        strncpy(m_suffixes[i], SZS_EXTENSION, SUFFIX_SIZE);
        m_formats[i] = Format::Double;
    }
}

/// @addr{0x8052A6DC}
MultiDvdArchive::~MultiDvdArchive() {
    EGG::egg_delete_array(m_archives, m_archiveCount);
    // WARN: Could lead to a memory leak if this is the only reference to the file!
    EGG::egg_free(m_fileStarts);
    EGG::egg_free(m_fileSizes);
    // WARN: Could lead to a memory leak if the pointer is not static!
    EGG::egg_free(m_suffixes);
    EGG::egg_delete_array(m_formats, m_archiveCount);
}

/// @addr{0x8052A760}
void *MultiDvdArchive::getFile(const char *filename, size_t *size) const {
    void *file = nullptr;

    for (u16 i = m_archiveCount; i-- > 0;) {
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

/// @addr{0x8052A954}
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

/// @addr{0x8052AAE8}
void MultiDvdArchive::load(const MultiDvdArchive *other) {
    for (u16 i = 0; i < m_archiveCount; i++) {
        m_archives[i].load(&other->m_archives[i]);
    }
}

/// @addr{0x8052AB6C}
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

/// @addr{0x8052AC40}
void MultiDvdArchive::clear() {
    for (u16 i = 0; i < m_archiveCount; i++) {
        m_archives[i].clear();
    }
}

/// @addr{0x8052AA88}
void MultiDvdArchive::unmount() {
    for (u16 i = 0; i < m_archiveCount; i++) {
        m_archives[i].unmount();
    }
}

/// @addr{0x8052A800}
bool MultiDvdArchive::isLoaded() const {
    for (u16 i = 0; i < m_archiveCount; i++) {
        if (m_archives[i].isLoaded()) {
            return true;
        }
    }

    return false;
}

/// @addr{0x8052AE08}
u16 MultiDvdArchive::rippedArchiveCount() const {
    u16 count = 0;
    for (u16 i = 0; i < m_archiveCount; i++) {
        if (m_archives[i].isRipped()) {
            count++;
        }
    }

    return count;
}

} // namespace Kinoko::System
