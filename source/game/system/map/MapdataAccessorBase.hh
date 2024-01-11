#pragma once

#include <Common.hh>

namespace System {

struct MapSectionHeader {
    s32 magic;
    u16 count;
};

template <typename T, typename TData>
class MapdataAccessorBase {
public:
    MapdataAccessorBase(const MapSectionHeader *header)
        : m_entries(nullptr), m_entryCount(0), m_sectionHeader(header) {}

    T *get(u16 i) const {
        return i < m_entryCount ? m_entries[i] : nullptr;
    }

    TData *getData(u16 i) const {
        return i < m_entryCount ? m_entries[i].data() : nullptr;
    }

    u16 size() const {
        return m_entryCount;
    }

    void init(const TData *start, u16 count) {
        if (count != 0) {
            m_entryCount = count;
            m_entries = new T *[count];
        }

        for (u16 i = 0; i < count; ++i) {
            m_entries[i] = new T(&start[i]);
        }
    }

protected:
    T **m_entries;
    u16 m_entryCount;
    const MapSectionHeader *m_sectionHeader;
};

} // namespace System
