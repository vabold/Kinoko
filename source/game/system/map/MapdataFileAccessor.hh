#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

namespace System {

class MapdataFileAccessor {
public:
    struct SData {
        u32 magic;
        u32 fileSize;
        u16 sectionCount;
        u16 headerSize;
        u32 revision;
        s32 offsets[];
    };

    MapdataFileAccessor(const SData *data);

    const MapSectionHeader *findSection(u32 sectionName) const;

    u32 version() const {
        return m_version;
    }

private:
    const SData *m_rawData;
    const u32 *m_sectionDef;
    u32 m_version;
    u32 m_sectionDefOffset;
};

} // namespace System
