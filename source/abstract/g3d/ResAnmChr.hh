#pragma once

#include "abstract/g3d/AnmObj.hh"
#include "abstract/g3d/ResCommon.hh"

#include <egg/util/Stream.hh>

// Credit: kiwi515/ogws

namespace Abstract {
namespace g3d {

/// @brief Represents the CHR0 file format, which pertains to model movement animations.
class ResAnmChr {
public:
    struct InfoData {
        void read(EGG::Stream &stream) {
            numFrame = stream.read_u16();
            numNode = stream.read_u16();
            policy = static_cast<AnmPolicy>(stream.read_u32());
            scalingRule = stream.read_u32();
        }

        u16 numFrame;
        u16 numNode;
        AnmPolicy policy;
        u32 scalingRule;
    };
    STATIC_ASSERT(sizeof(InfoData) == 0xC);

    struct Data {
        ResBlockHeaderData header;
        u32 revision;
        s32 toResFileData;
        s32 toChrDataDic;
        s32 toResUserData;
        s32 name;
        s32 originalPath;
        InfoData info;
    };
    STATIC_ASSERT(sizeof(Data) == 0x2C);

    ResAnmChr(const void *data) : m_rawData(reinterpret_cast<const Data *>(data)) {
        EGG::RamStream stream = EGG::RamStream(data, sizeof(Data));
        read(stream);
    }

    void read(EGG::Stream &stream) {
        stream.jump(offsetof(Data, info));
        m_infoData.read(stream);
    }

    [[nodiscard]] u16 frameCount() const {
        return m_infoData.numFrame;
    }

    [[nodiscard]] AnmPolicy policy() const {
        return m_infoData.policy;
    }

private:
    const Data *m_rawData;
    InfoData m_infoData;
};

class AnmObjChrRes : public FrameCtrl {
public:
    AnmObjChrRes(const ResAnmChr &chr)
        : FrameCtrl(0.0f, chr.frameCount(), GetAnmPlayPolicy(chr.policy())), m_resAnmChr(chr) {}

    [[nodiscard]] u16 frameCount() const {
        return m_resAnmChr.frameCount();
    }

private:
    ResAnmChr m_resAnmChr;
};

} // namespace g3d
} // namespace Abstract
