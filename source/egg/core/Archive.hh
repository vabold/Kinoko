#pragma once

#include <Common.hh>

#include <vector>

#include <source/abstract/Archive.hh>

namespace EGG {

class Archive {
public:
    ~Archive();

    void unmount();
    s32 convertPathToEntryId(const char *path) const;
    void *getFile(s32 entryId, Abstract::ArchiveHandle::FileInfo &info) const;

    static Archive *Mount(void *archiveStart);

private:
    Archive(void *archiveStart);

    Abstract::ArchiveHandle m_handle;
    size_t m_vectorIdx = -1;
    s32 m_refCount = 1;
};

static std::vector<Archive *> s_archiveList;

} // namespace EGG
