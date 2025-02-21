#include "DvdArchive.hh"

#include <abstract/File.hh>

#include <egg/core/Decomp.hh>

namespace System {

/// @addr{0x80518CC0}
DvdArchive::DvdArchive()
    : m_archive(nullptr), m_archiveStart(nullptr), m_archiveSize(0), m_fileStart(nullptr),
      m_fileSize(0), m_state(State::Cleared) {}

/// @addr{0x80518CF4}
DvdArchive::~DvdArchive() {
    unmount();
}

/// @addr{0x80519508}
void DvdArchive::decompress() {
    m_archiveSize = EGG::Decomp::GetExpandSize(reinterpret_cast<u8 *>(m_fileStart));
    m_archiveStart = new u8[m_archiveSize];
    EGG::Decomp::DecodeSZS(reinterpret_cast<u8 *>(m_fileStart),
            reinterpret_cast<u8 *>(m_archiveStart));
    m_state = State::Decompressed;
}

/// @addr{0x80519420}
void *DvdArchive::getFile(const char *filename, size_t *size) const {
    if (m_state != State::Mounted) {
        return nullptr;
    }

    char buffer[256];
    if (filename[0] == '/') {
        snprintf(buffer, sizeof(buffer), "%s", filename);
    } else {
        snprintf(buffer, sizeof(buffer), "/%s", filename);
    }
    buffer[sizeof(buffer) - 1] = '\0';

    Abstract::ArchiveHandle::FileInfo fileInfo{0, 0};
    s32 entryId = m_archive->convertPathToEntryId(buffer);
    if (entryId == -1) {
        return nullptr;
    }

    void *file = m_archive->getFileFast(entryId, fileInfo);
    if (file && size) {
        *size = fileInfo.length;
    }

    return file;
}

/// @addr{0x80518E10}
void DvdArchive::load(const char *path, bool decompress_) {
    if (m_state == State::Cleared) {
        rip(path);
    }

    if (m_state == State::Ripped) {
        if (decompress_) {
            decompress();
            clearFile();
        } else {
            move();
        }
        mount();
    }
}

/// @addr{0x805195D8}
void DvdArchive::load(const DvdArchive *other) {
    if (other->m_state == State::Ripped) {
        m_fileStart = other->m_fileStart;
        m_state = State::Ripped;
        decompress();
        mount();
        m_fileStart = nullptr;
        m_fileSize = 0;
    } else {
        m_state = State::Cleared;
    }
}

/// @addr{0x80518FBC}
void DvdArchive::load(void *fileStart, size_t fileSize, bool decompress_) {
    m_fileStart = fileStart;
    m_fileSize = fileSize;
    if (decompress_) {
        decompress();
        m_fileStart = nullptr;
        m_fileSize = 0;
    }
    mount();
}

/// @addr{0x80518DCC}
void DvdArchive::mount() {
    m_archive = EGG::Archive::Mount(m_archiveStart);
    m_state = State::Mounted;
}

/// @addr{0x805195A4}
void DvdArchive::move() {
    m_archiveStart = m_fileStart;
    m_archiveSize = m_fileSize;
    m_fileStart = nullptr;
    m_fileSize = 0;
    m_state = State::Decompressed;
}

/// @addr{0x805190F0}
void DvdArchive::rip(const char *path) {
    m_fileStart = Abstract::File::Load(path, m_fileSize);
    if (m_fileSize != 0 && m_fileStart) {
        m_state = State::Ripped;
    }
}

/// @addr{0x80519240}
void DvdArchive::clear() {
    clearArchive();
    clearFile();
}

/// @addr{0x80519370}
void DvdArchive::clearArchive() {
    if (!m_archiveStart) {
        return;
    }

    delete[] static_cast<u8 *>(m_archiveStart);
    m_archiveStart = nullptr;
    m_archiveSize = 0;
}

/// @addr{0x805193C8}
void DvdArchive::clearFile() {
    if (!m_fileStart) {
        return;
    }

    delete[] static_cast<u8 *>(m_fileStart);
    m_fileStart = nullptr;
    m_fileSize = 0;
}

/// @addr{0x805192CC}
void DvdArchive::unmount() {
    if (m_state == State::Mounted) {
        m_archive->unmount();
    }
    clear();
    m_state = State::Cleared;
}

} // namespace System
