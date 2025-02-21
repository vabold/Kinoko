#include "game/system/map/MapdataCheckPath.hh"

namespace System {

/// @addr{0x80515098}
MapdataCheckPath::MapdataCheckPath(const SData *data) : m_rawData(data), m_depth(-1) {
    EGG::RamStream stream = EGG::RamStream(data, sizeof(SData));
    read(stream);
    m_oneOverCount = 1.0f / m_size;
}

void MapdataCheckPath::read(EGG::Stream &stream) {
    m_start = stream.read_u8();
    m_size = stream.read_u8();
    for (auto &prev : m_prev) {
        prev = stream.read_u8();
    }

    for (auto &next : m_next) {
        next = stream.read_u8();
    }
}

/// @brief Performs DFS to calculate @ref m_depth for all subsequent checkpaths.
/// @param depth Number of checkpaths from first checkpath.
/// @addr{0x805150E0}
void MapdataCheckPath::findDepth(s8 depth, const MapdataCheckPathAccessor &accessor) {
    if (m_depth != -1) {
        return;
    }

    m_depth = depth;

    for (auto &nextID : m_next) {
        if (nextID == 0xFF) {
            continue;
        }

        accessor.get(nextID)->findDepth(depth + 1, accessor);
    }
}

/// @addr{Inlined in 0x8051377C}
MapdataCheckPathAccessor::MapdataCheckPathAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataCheckPath, MapdataCheckPath::SData>(header) {
    init(reinterpret_cast<const MapdataCheckPath::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));

    if (m_entryCount == 0) {
        return;
    }

    // Maximum number of paths one could traverse through in a lap
    s8 maxDepth = -1;
    get(0)->findDepth(0, *this);

    for (size_t i = 0; i < size(); ++i) {
        maxDepth = std::max(maxDepth, get(i)->depth());
    }

    m_lapProportion = 1.0f / (maxDepth + 1.0f);
}

MapdataCheckPathAccessor::~MapdataCheckPathAccessor() = default;

/// @addr{0x80515014}
MapdataCheckPath *MapdataCheckPathAccessor::findCheckpathForCheckpoint(u16 checkpointId) const {
    for (size_t i = 0; i < size(); ++i) {
        MapdataCheckPath *checkpath = get(i);
        if (checkpath->isPointInPath(checkpointId)) {
            return checkpath;
        }
    }

    return nullptr;
}

f32 MapdataCheckPathAccessor::lapProportion() const {
    return m_lapProportion;
}

} // namespace System
