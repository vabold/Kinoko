#pragma once

#include <Common.hh>

// TODO: for the love of god please find a better name this is not called a "U8" archive
#define U8_SIGNATURE 0x55AA382D

namespace Abstract {

struct RawArchive {
    bool isValidSignature() const;

    u32 m_signature;
    u32 m_nodesOffset;
    u32 m_nodesSize;
    u32 m_filesOffset;
};

class ArchiveHandle {
public:
    struct RawArchive {
        bool isValidSignature() const;

        u32 m_signature;
        u32 m_nodesOffset;
        u32 m_nodesSize;
        u32 m_filesOffset;
    };

    // TODO: union
    struct Node {
        bool isDirectory() const;
        const char *getName() const;
        u32 stringOffset() const;

        union {
            u32 m_val;
            u8 m_str[4];
        };
        union {
            struct {
                u32 m_parent;
                u32 m_next;
            } m_directory;
            struct {
                u32 m_startAddress;
                u32 m_length;
            } m_file;
        };
    };

    struct FileInfo {
        u32 m_startOffset;
        u32 m_length;
    };

    ArchiveHandle(void *archiveStart);

    s32 convertPathToEntryId(const char *path);
    bool open(s32 entryId, FileInfo &info) const;

    void *getFileAddress(const FileInfo &info) const;
    Node *node(s32 entryId) const;
    void *startAddress() const;

private:
    void *m_startAddress;
    void *m_nodesAddress;
    void *m_filesAddress;
    u32 m_count;
    u32 m_currentNode;
    const char *m_strings;
};

} // namespace Abstract
