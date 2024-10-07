#include "Archive.hh"

#include <algorithm>
#include <cstddef>

namespace EGG {

/// @brief Removes the archive from the static list.
/// @addr{0x8020f6ec}
/// @details Called when the archive's reference count becomes 0.
Archive::~Archive() {
    auto iter = std::find(s_archiveList.begin(), s_archiveList.end(), this);
    if (iter != s_archiveList.end()) {
        s_archiveList.erase(iter);
    }
}

/// @addr{0x8020fa38}
void Archive::unmount() {
    if (--m_refCount <= 0) {
        delete this;
    }
}

/// @addr{0x8020fa78}
s32 Archive::convertPathToEntryId(const char *path) const {
    return m_handle.convertPathToEntryId(path);
}

/// @addr{0x8020fa80}
void *Archive::getFileFast(s32 entryId, Abstract::ArchiveHandle::FileInfo &info) const {
    m_handle.open(entryId, info);
    return m_handle.getFileAddress(info);
}

/// @brief Checks to see if a given archive is already mounted.
/// @addr{Inlined in 0x8020F768}
/// @param archiveStart The address of the archive to search for.
/// @return The archive if it is already mounted, or nullptr if it is not.
Archive *Archive::FindArchive(void *archiveStart) {
    ASSERT(archiveStart);

    for (auto iter = s_archiveList.begin(); iter != s_archiveList.end(); ++iter) {
        if ((*iter)->m_handle.startAddress() == archiveStart) {
            return *iter;
        }
    }

    return nullptr;
}

/// @brief Creates a new Archive object or increments the ref count for an already existing Archive.
/// @addr{0x8020F768}
/// @param archiveStart The address of the archive to mount.
/// @return The Archive, regardless if it is new or already exists.
Archive *Archive::Mount(void *archiveStart) {
    Archive *archive = FindArchive(archiveStart);

    if (!archive) {
        // Create a new archive and add it to the list
        archive = new Archive(archiveStart);
        s_archiveList.push_back(archive);
    } else {
        // It already exists, increase the reference count
        archive->m_refCount++;
    }

    return archive;
}

/// @addr{Inlined in 0x8020F768}
Archive::Archive(void *archiveStart) : m_handle(archiveStart) {}

std::list<Archive *> Archive::s_archiveList;

} // namespace EGG
