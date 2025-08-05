#pragma once

#include <Common.hh>

#include <abstract/Archive.hh>
#include <abstract/memory/List.hh>

class SavestateManager;

namespace Abstract::Memory {

struct MEMList;

} // namespace Abstract::Memory

/// @brief EGG core library
namespace EGG {

class Archive : Disposer {
    friend class ::SavestateManager;

public:
    ~Archive();

    void unmount();
    [[nodiscard]] s32 convertPathToEntryId(const char *path) const;
    void *getFileFast(s32 entryId, Abstract::ArchiveHandle::FileInfo &info) const;

    [[nodiscard]] static Archive *FindArchive(void *archiveStart);
    [[nodiscard]] static Archive *Mount(void *archiveStart);

private:
    Archive(void *archiveStart);

    [[nodiscard]] static constexpr uintptr_t GetLinkOffset() {
        // offsetof doesn't work, so instead of hardcoding an offset, we derive it ourselves
        return reinterpret_cast<uintptr_t>(&reinterpret_cast<Archive *>(NULL)->m_link);
    }

    Abstract::ArchiveHandle m_handle;
    s32 m_refCount = 1;
    Abstract::Memory::MEMLink m_link;

    static Abstract::Memory::MEMList s_archiveList; ///< The linked list of all mounted archives.
};

} // namespace EGG
