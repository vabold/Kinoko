#pragma once

#include "game/system/map/MapdataAccessorBase.hh"

#include <egg/util/Stream.hh>

namespace System {

class MapdataStageInfo {
public:
    struct SData {
        u8 _0[0x1 - 0x0];
        u8 polePosition;
        u8 translationMode;
        u8 _3[0xc - 0x3];
    };
    STATIC_ASSERT(sizeof(SData) == 0xc);

    MapdataStageInfo(const SData *data);

    void read(EGG::Stream &stream);

    [[nodiscard]] u8 polePosition() const {
        return m_rawData->polePosition;
    }

    [[nodiscard]] u8 translationMode() const {
        return m_rawData->translationMode;
    }

private:
    const SData *m_rawData;
};

class MapdataStageInfoAccessor
    : public MapdataAccessorBase<MapdataStageInfo, MapdataStageInfo::SData> {
public:
    MapdataStageInfoAccessor(const MapSectionHeader *header);
    ~MapdataStageInfoAccessor() override;
};

} // namespace System
