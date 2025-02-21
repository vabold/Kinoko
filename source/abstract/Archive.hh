#pragma once

#include <Common.hh>

// TODO: for the love of god please find a better name this is not called a "U8" archive
#define U8_SIGNATURE 0x55AA382D

/// @brief An abstraction of components from the nw4r and RVL libraries.
namespace Abstract {

class ArchiveHandle {
public:
    struct RawArchive {
        [[nodiscard]] bool isValidSignature() const {
            return parse<u32>(signature) == U8_SIGNATURE;
        }

        u32 signature;
        u32 nodesOffset;
        u32 nodesSize;
        u32 filesOffset;
    };

    // TODO: union
    struct Node {
        [[nodiscard]] bool isDirectory() const {
            return !!(str[0]);
        }

        [[nodiscard]] u32 stringOffset() const {
            return parse<u32>(val) & 0xFFFFFF;
        }

        union {
            u32 val;
            u8 str[4];
        };
        union {
            struct {
                u32 parent;
                u32 next;
            } m_directory;
            struct {
                u32 startAddress;
                u32 length;
            } file;
        };
    };

    struct FileInfo {
        u32 startOffset;
        u32 length;
    };

    ArchiveHandle(void *archiveStart);

    [[nodiscard]] s32 convertPathToEntryId(const char *path) const;
    bool open(s32 entryId, FileInfo &info) const;

    /// @addr{0x80124CC0}
    [[nodiscard]] void *getFileAddress(const FileInfo &info) const {
        return static_cast<u8 *>(m_startAddress) + info.startOffset;
    }

    [[nodiscard]] Node *node(s32 entryId) const {
        auto *nodeAddress = static_cast<u8 *>(m_nodesAddress) + sizeof(Node) * entryId;
        return reinterpret_cast<Node *>(nodeAddress);
    }

    [[nodiscard]] void *startAddress() const {
        return m_startAddress;
    }

private:
    void *m_startAddress;
    void *m_nodesAddress;
    void *m_filesAddress;
    u32 m_count;
    u32 m_currentNode;
    const char *m_strings;
};

} // namespace Abstract
