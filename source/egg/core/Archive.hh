#pragma once

#include <Common.hh>

#include <abstract/Archive.hh>

/// @brief EGG core library
namespace EGG {

class Archive : Disposer {
public:
    ~Archive();

    void unmount();
    [[nodiscard]] s32 convertPathToEntryId(const char *path) const;
    void *getFileFast(s32 entryId, Abstract::ArchiveHandle::FileInfo &info) const;

    [[nodiscard]] static Archive *FindArchive(void *archiveStart);
    [[nodiscard]] static Archive *Mount(void *archiveStart);

private:
    Archive(void *archiveStart);

    Abstract::ArchiveHandle m_handle;
    s32 m_refCount = 1;

    static std::list<Archive *> s_archiveList; ///< The linked list of all mounted archives.
};

} // namespace EGG
