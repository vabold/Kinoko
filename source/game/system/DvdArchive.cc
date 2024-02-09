#include "DvdArchive.hh"

#include <abstract/File.hh>

#include <egg/core/Decomp.hh>

namespace System {

DvdArchive::DvdArchive()
    : m_archive(nullptr), m_archiveStart(nullptr), m_archiveSize(0), m_fileStart(nullptr),
      m_fileSize(0), m_state(State::Cleared) {}

DvdArchive::~DvdArchive() {
    unmount();
}

void DvdArchive::decompress() {
    s32 expandSize = EGG::Decomp::GetExpandSize(reinterpret_cast<u8 *>(m_fileStart));
    void *archive = new u8[expandSize];
    EGG::Decomp::DecodeSZS(reinterpret_cast<u8 *>(m_fileStart), reinterpret_cast<u8 *>(archive));
    m_archiveSize = expandSize;
    m_archiveStart = archive;
    m_state = State::Decompressed;
}

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

    void *file = m_archive->getFile(entryId, fileInfo);
    if (file && size) {
        *size = fileInfo.length;
    }

    return file;
}

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

void DvdArchive::mount() {
    m_archive = EGG::Archive::Mount(m_archiveStart);
    m_state = State::Mounted;
}

void DvdArchive::move() {
    m_archiveStart = m_fileStart;
    m_archiveSize = m_fileSize;
    m_fileStart = nullptr;
    m_fileSize = 0;
    m_state = State::Decompressed;
}

void DvdArchive::rip(const char *path) {
    m_fileStart = Abstract::File::Load(path, m_fileSize);
    if (m_fileSize != 0 && m_fileStart) {
        m_state = State::Ripped;
    }
}

void DvdArchive::clear() {
    clearArchive();
    clearFile();
}

void DvdArchive::clearArchive() {
    if (!m_archiveStart) {
        return;
    }

    delete[] static_cast<u8 *>(m_archiveStart);
    m_archiveStart = nullptr;
    m_archiveSize = 0;
}

void DvdArchive::clearFile() {
    if (m_fileStart) {
        return;
    }

    delete[] static_cast<u8 *>(m_fileStart);
    m_fileStart = nullptr;
    m_fileSize = 0;
}

void DvdArchive::unmount() {
    if (m_state == State::Mounted) {
        m_archive->unmount();
    }
    clear();
    m_state = State::Cleared;
}

bool DvdArchive::isLoaded() const {
    return m_state == State::Mounted;
}

bool DvdArchive::isRipped() const {
    return m_state == State::Ripped;
}

} // namespace System
