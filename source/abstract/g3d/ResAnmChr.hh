#pragma once

#include "abstract/g3d/AnmObj.hh"
#include "abstract/g3d/ResCommon.hh"
#include "abstract/g3d/ResDic.hh"

#include <egg/math/Matrix.hh>
#include <egg/util/Stream.hh>

// Credit: kiwi515/ogws

namespace Abstract {
namespace g3d {

struct ChrAnmResult {
    enum Flag {
        FLAG_ANM_EXISTS = (1 << 0),
        FLAG_MTX_IDENT = (1 << 1),
        FLAG_ROT_TRANS_ZERO = (1 << 2),
        FLAG_SCALE_ONE = (1 << 3),
        FLAG_SCALE_UNIFORM = (1 << 4),
        FLAG_ROT_ZERO = (1 << 5),
        FLAG_TRANS_ZERO = (1 << 6),
        FLAG_PATCH_SCALE = (1 << 7),
        FLAG_PATCH_ROT = (1 << 8),
        FLAG_PATCH_TRANS = (1 << 9),

        // Maya Scale Compensation
        FLAG_SSC_APPLY = (1 << 10),
        FLAG_SSC_PARENT = (1 << 11),

        // Softimage Hierarchical Scaling
        FLAG_XSI_SCALING = (1 << 12),
    };

    /// @addr{0x800555C0}
    [[nodiscard]] EGG::Vector3f scale() const {
        return s;
    }

    u32 flags;
    EGG::Vector3f s;
    EGG::Vector3f rawR;
    EGG::Matrix34f rt;
};

/// @brief Represents the CHR0 file format, which pertains to model movement animations.
class ResAnmChr {
public:
    /// @brief Frame values (FVS) animation data
    struct Frm48Data {
        s16 frame; // at 0x0
        u16 value; // at 0x2
        s16 slope; // at 0x4
    };
    struct FVS48Data {
        f32 scale;                // at 0x0
        f32 offset;               // at 0x4
        Frm48Data frameValues[1]; // at 0x8
    };

    struct Frm96Data {
        f32 frame; // at 0x0
        f32 value; // at 0x4
        f32 slope; // at 0x8
    };
    struct FVS96Data {
        Frm96Data frameValues[1]; // at 0x0
    };

    struct FVSData {
        u16 numFrameValues;        // at 0x0
        u8 PADDING_0x2[0x4 - 0x2]; // at 0x2
        f32 invKeyFrameRange;      // at 0x4

        union {
            FVS48Data fvs48; // at 0x8
            FVS96Data fvs96; // at 0x8
        };
    };

    /// @brief Const value (CV) animation data
    struct CV32Data {
        f32 values[1]; // at 0x0
    };

    struct CVData {
        CV32Data cv32; // at 0x0
    };

    struct AnmData {
        union {
            FVSData fvs; // at 0x0
            CVData cv;   // at 0x0
        };
    };

    struct NodeData {
        enum Flag {
            FLAG_ANM_EXISTS = (1 << 0),
            FLAG_MTX_IDENT = (1 << 1),
            FLAG_ROT_TRANS_ZERO = (1 << 2),
            FLAG_SCALE_ONE = (1 << 3),
            FLAG_SCALE_UNIFORM = (1 << 4),
            FLAG_ROT_ZERO = (1 << 5),
            FLAG_TRANS_ZERO = (1 << 6),
            FLAG_PATCH_SCALE = (1 << 7),
            FLAG_PATCH_ROT = (1 << 8),
            FLAG_PATCH_TRANS = (1 << 9),

            // Maya Scale Compensation
            FLAG_SSC_APPLY = (1 << 10),
            FLAG_SSC_PARENT = (1 << 11),

            // Softimage Hierarchical Scaling
            FLAG_XSI_SCALING = (1 << 12),

            FLAG_SCALE_X_CONST = (1 << 13),
            FLAG_SCALE_Y_CONST = (1 << 14),
            FLAG_SCALE_Z_CONST = (1 << 15),

            FLAG_ROT_X_CONST = (1 << 16),
            FLAG_ROT_Y_CONST = (1 << 17),
            FLAG_ROT_Z_CONST = (1 << 18),

            FLAG_TRANS_X_CONST = (1 << 19),
            FLAG_TRANS_Y_CONST = (1 << 20),
            FLAG_TRANS_Z_CONST = (1 << 21),

            FLAG_HAS_SCALE = (1 << 22),
            FLAG_HAS_ROT = (1 << 23),
            FLAG_HAS_TRANS = (1 << 24),

            FLAG_SCALE_FVS32_FMT = (1 << 25),
            FLAG_SCALE_FVS48_FMT = (1 << 26),
            FLAG_SCALE_FVS96_FMT = FLAG_SCALE_FVS32_FMT | FLAG_SCALE_FVS48_FMT,

            FLAG_ROT_FVS32_FMT = (1 << 27),
            FLAG_ROT_FVS48_FMT = (1 << 28),
            FLAG_ROT_FVS96_FMT = FLAG_ROT_FVS32_FMT | FLAG_ROT_FVS48_FMT,

            FLAG_ROT_CV8_FMT = (1 << 29),
            FLAG_ROT_CV16_FMT = (1 << 27) | (1 << 29),
            FLAG_ROT_CV32_FMT = (1 << 28) | (1 << 29),

            FLAG_TRANS_FVS32_FMT = (1 << 30),
            FLAG_TRANS_FVS48_FMT = (1 << 31),
            FLAG_TRANS_FVS96_FMT = FLAG_TRANS_FVS32_FMT | FLAG_TRANS_FVS48_FMT,

            FLAG_HAS_SRT_MASK = FLAG_HAS_SCALE | FLAG_HAS_ROT | FLAG_HAS_TRANS,

            FLAG_SCALE_FMT_MASK =
                    FLAG_SCALE_FVS32_FMT | FLAG_SCALE_FVS48_FMT | FLAG_SCALE_FVS96_FMT,

            FLAG_ROT_FMT_MASK = FLAG_ROT_FVS32_FMT | FLAG_ROT_FVS48_FMT | FLAG_ROT_FVS96_FMT |
                    FLAG_ROT_CV8_FMT | FLAG_ROT_CV16_FMT | FLAG_ROT_CV32_FMT,

            FLAG_TRANS_FMT_MASK = FLAG_TRANS_FVS32_FMT | FLAG_TRANS_FVS48_FMT | FLAG_TRANS_FVS96_FMT
        };

        union AnmData {
            s32 toResAnmChrAnmData;
            f32 constValue;
        };

        s32 name;
        u32 flags;
        AnmData anms[1];
    };
    STATIC_ASSERT(sizeof(NodeData) == 0xC);

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

    [[nodiscard]] ChrAnmResult getAnmResult(f32 frame, size_t idx) const;

    [[nodiscard]] u16 frameCount() const {
        return m_infoData.numFrame;
    }

    [[nodiscard]] AnmPolicy policy() const {
        return m_infoData.policy;
    }

private:
    typedef void (*GetAnmResultFunc)(f32 frame, ChrAnmResult &result,
            const ResAnmChr::InfoData &infoData, const ResAnmChr::NodeData *nodeData);

    const Data *m_rawData;
    InfoData m_infoData;

    static constexpr size_t NUM_RESULT_FUNCS = 8;
    static const std::array<GetAnmResultFunc, NUM_RESULT_FUNCS> s_getAnmResultTable;
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
