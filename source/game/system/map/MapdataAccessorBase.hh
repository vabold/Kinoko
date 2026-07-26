#pragma once

#include <Common.hh>

#include <egg/core/Heap.hh>

namespace Kinoko::System {

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
                EGG::egg_delete(m_entries[i]);
            }
            EGG::egg_free(m_entries);
        }
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

    void init(const TData *start, u16 count) {
        if (count != 0) {
            m_entryCount = count;
            m_entries = static_cast<T **>(EGG::egg_alloc(count * sizeof(T *)));
        }

        for (u16 i = 0; i < count; ++i) {
            m_entries[i] = EGG::egg_new<T>(&start[i]);
        }
    }

protected:
    T **m_entries;
    u16 m_entryCount;
    const MapSectionHeader *m_sectionHeader;
};

} // namespace Kinoko::System
