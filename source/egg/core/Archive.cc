#include "Archive.hh"

#include <algorithm>
#include <cstddef>

namespace EGG {

Archive::~Archive() {
    auto iter = std::find(s_archiveList.begin(), s_archiveList.end(), this);
    if (iter != s_archiveList.end()) {
        s_archiveList.erase(iter);
    }
}

void Archive::unmount() {
    if (--m_refCount <= 0) {
        delete this;
    }
}

s32 Archive::convertPathToEntryId(const char *path) const {
    return m_handle.convertPathToEntryId(path);
}

void *Archive::getFileFast(s32 entryId, Abstract::ArchiveHandle::FileInfo &info) const {
    m_handle.open(entryId, info);
    return m_handle.getFileAddress(info);
}

Archive *Archive::FindArchive(void *archiveStart) {
    assert(archiveStart);

    for (auto iter = s_archiveList.begin(); iter != s_archiveList.end(); ++iter) {
        if ((*iter)->m_handle.startAddress() == archiveStart) {
            return *iter;
        }
    }

    return nullptr;
}

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

Archive::Archive(void *archiveStart) : m_handle(archiveStart) {}

std::list<Archive *> Archive::s_archiveList;

} // namespace EGG
