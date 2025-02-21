#include "Archive.hh"

#include <cstring>

namespace Abstract {

/// @addr{0x80124500}
ArchiveHandle::ArchiveHandle(void *archiveStart) : m_startAddress(archiveStart) {
    RawArchive *rawArchive = reinterpret_cast<RawArchive *>(archiveStart);
    ASSERT(rawArchive->isValidSignature());

    m_nodesAddress = static_cast<u8 *>(archiveStart) + parse<u32>(rawArchive->nodesOffset);
    m_filesAddress = static_cast<u8 *>(archiveStart) + parse<u32>(rawArchive->filesOffset);

    // "The right bound of the root node is the number of nodes"
    m_count = parse<u32>(node(0)->m_directory.next);
    // Strings exist directly after the last node
    m_strings = reinterpret_cast<const char *>(reinterpret_cast<Node *>(m_nodesAddress) + m_count);
    m_currentNode = 0;
}

/// @addr{0x80124894}
s32 ArchiveHandle::convertPathToEntryId(const char *path) const {
    u32 entryId = m_currentNode;

    while (true) {
        // End search
        if (path[0] == '\0') {
            return entryId;
        }

        // Send initial slash to root directory
        if (path[0] == '/') {
            entryId = 0;
            path++;
            continue;
        }

        // Handle special cases
        if (path[0] == '.') {
            if (path[1] == '.') {
                if (path[2] == '/') {
                    entryId = node(entryId)->m_directory.parent;
                    path += 3;
                    continue;
                } else if (path[2] == '\0') {
                    return node(entryId)->m_directory.parent;
                } else {
                    // Malformed "..*" case
                    return -1;
                }
            } else if (path[1] == '/') {
                path += 2;
                continue;
            } else if (path[1] == '\0') {
                return entryId;
            }
        }

        // Main search
        const char *nameEnd = path;
        for (; nameEnd[0] != '\0' && nameEnd[0] != '/'; nameEnd++) {}

        bool endOfPath = nameEnd[0] == '\0';
        s32 nameLength = nameEnd - path;

        bool found = false;
        const u32 anchor = entryId++;
        while (entryId < parse<u32>(node(anchor)->m_directory.next)) {
            if (!node(anchor)->isDirectory() && endOfPath) {
                entryId++;
                continue;
            }

            const char *entryName = m_strings + node(entryId)->stringOffset();

            if (entryName[0] == '.' && entryName[1] == '\0') {
                entryId++;
                continue;
            }

            if (strncmp(path, entryName, nameLength) == 0) {
                found = true;
                break;
            }

            entryId++;
        }

        if (!found) {
            return -1;
        }

        if (endOfPath) {
            return entryId;
        }

        path += nameLength + 1;
    }
}

/// @addr{0x80124844}
bool ArchiveHandle::open(s32 entryId, FileInfo &info) const {
    if (entryId < 0 || static_cast<u32>(entryId) >= m_count) {
        return false;
    }

    auto *node_ = node(entryId);
    if (node_->isDirectory()) {
        return false;
    }

    info.startOffset = parse<u32>(node_->file.startAddress);
    info.length = parse<u32>(node_->file.length);
    return true;
}

} // namespace Abstract
