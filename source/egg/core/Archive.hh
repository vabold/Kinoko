#pragma once

#include <Common.hh>

#include <abstract/Archive.hh>

#include <list>

namespace EGG {

class Archive {
public:
    ~Archive();

    void unmount();
    s32 convertPathToEntryId(const char *path) const;
    void *getFileFast(s32 entryId, Abstract::ArchiveHandle::FileInfo &info) const;

    static Archive *FindArchive(void *archiveStart);
    static Archive *Mount(void *archiveStart);

private:
    Archive(void *archiveStart);

    Abstract::ArchiveHandle m_handle;
    s32 m_refCount = 1;

    static std::list<Archive *> s_archiveList;
};

} // namespace EGG
