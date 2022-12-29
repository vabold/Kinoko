#include "Archive.hh"

namespace EGG {

Archive::~Archive() = default;

void Archive::unmount() {
    if (m_refCount-- == 0) {
        delete this;
    }
}

s32 Archive::convertPathToEntryId(const char *path) {
    return m_handle.convertPathToEntryId(path);
}

void *Archive::getFile(s32 entryId, Abstract::ArchiveHandle::FileInfo &info) {
    m_handle.open(entryId, info);
    return m_handle.getFileAddress(info);
}

Archive *Archive::Mount(void *archiveStart) {
    assert(archiveStart);
    Archive *archive = nullptr;

    // Check if archive already exists
    if (!s_archiveList.empty()) {
        for (size_t i = 0; i < s_archiveList.size(); i++) {
            if (!s_archiveList[i]) {
                continue;
            }

            if (s_archiveList[i]->m_handle.startAddress() == archiveStart) {
                archive = s_archiveList[i];
                break;
            }
        }
    }

    if (archive) {
        // It already exists, increase the reference count
        archive->m_refCount++;
    } else {
        // Create a new archive and add it to the vector
        archive = new Archive(archiveStart);
        s_archiveList.push_back(archive);
    }

    return archive;
}

Archive::Archive(void *archiveStart) : m_handle(archiveStart) {}

} // namespace EGG
