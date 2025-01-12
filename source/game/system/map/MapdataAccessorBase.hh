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
    MapdataAccessorBase(const MapdataAccessorBase &) = delete;
    MapdataAccessorBase(MapdataAccessorBase &&) = delete;

    virtual ~MapdataAccessorBase() {
        if (m_entries) {
            for (size_t i = 0; i < m_entryCount; ++i) {
                delete m_entries[i];
            }
        }

        delete[] m_entries;
    }

    [[nodiscard]] T *get(u16 i) const {
        return i < m_entryCount ? m_entries[i] : nullptr;
    }

    [[nodiscard]] TData *getData(u16 i) const {
        return i < m_entryCount ? m_entries[i]->data() : nullptr;
    }

    [[nodiscard]] u16 size() const {
        return m_entryCount;
    }

    virtual void init(const TData *start, u16 count) {
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
