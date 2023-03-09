#include "Archive.hh"

#include <cstring>

namespace Abstract {

ArchiveHandle::ArchiveHandle(void *archiveStart) : m_startAddress(archiveStart) {
    RawArchive *rawArchive = reinterpret_cast<RawArchive *>(archiveStart);
    assert(rawArchive->isValidSignature());

    m_nodesAddress = static_cast<u8 *>(archiveStart) +
            parse<u32>(rawArchive->m_nodesOffset, std::endian::big);
    m_filesAddress = static_cast<u8 *>(archiveStart) +
            parse<u32>(rawArchive->m_filesOffset, std::endian::big);

    // "The right bound of the root node is the number of nodes"
    m_count = parse<u32>(node(0)->m_directory.m_next, std::endian::big);
    // Strings exist directly after the last node
    m_strings = reinterpret_cast<const char *>(reinterpret_cast<Node *>(m_nodesAddress) + m_count);
    m_currentNode = 0;
}

// NOTE (vabold): If anyone wants to de-spaghetti this, please feel free
//                My hope is that I can leave this function alone forever
s32 ArchiveHandle::convertPathToEntryId(const char *path) {
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
                    entryId = node(entryId)->m_directory.m_parent;
                    path += 3;
                    continue;
                } else if (path[2] == '\0') {
                    return node(entryId)->m_directory.m_parent;
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
        const u32 anchor = entryId;
        entryId++;
        while (entryId < node(anchor)->m_directory.m_next) {
            while (true) {
                if (node(anchor)->isDirectory() || !endOfPath) {
                    const char *entryName = m_strings + node(entryId)->stringOffset();
                    K_LOG("%s", entryName);

                    if (entryName[0] == '.' && entryName[1] == '\0') {
                        entryId++;
                        continue;
                    }

                    if (strcmp(path, entryName) == 0) {
                        found = true;
                        break;
                    }
                }

                if (node(entryId)->isDirectory()) {
                    entryId = node(entryId)->m_directory.m_next;
                    break;
                }

                entryId++;
            }

            if (!found) {
                return -1;
            }

            break;
        }

        if (endOfPath) {
            return entryId;
        }

        path += nameLength + 1;
    }
}

bool ArchiveHandle::open(s32 entryId, FileInfo &info) const {
    if (entryId < 0 || static_cast<u32>(entryId) >= m_count) {
        return false;
    }

    auto *node_ = node(entryId);
    if (node_->isDirectory()) {
        return false;
    }

    info.m_startOffset = node_->m_file.m_startAddress;
    info.m_length = node_->m_file.m_length;
    return true;
}

void *ArchiveHandle::getFileAddress(const FileInfo &info) const {
    return static_cast<u8 *>(m_startAddress) + info.m_startOffset;
}

ArchiveHandle::Node *ArchiveHandle::node(s32 entryId) const {
    auto *nodeAddress = static_cast<u8 *>(m_nodesAddress) + sizeof(Node) * entryId;
    return reinterpret_cast<Node *>(nodeAddress);
}

void *ArchiveHandle::startAddress() const {
    return m_startAddress;
}

bool ArchiveHandle::RawArchive::isValidSignature() const {
    auto signature = parse<u32>(m_signature, std::endian::big);
    return signature == U8_SIGNATURE;
}

bool ArchiveHandle::Node::isDirectory() const {
    return !!(m_str[0]);
}

u32 ArchiveHandle::Node::stringOffset() const {
    return parse<u32>(m_val, std::endian::big) & 0xFFFFFF;
}

} // namespace Abstract
