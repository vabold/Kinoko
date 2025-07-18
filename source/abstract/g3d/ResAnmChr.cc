#include "ResAnmChr.hh"

// Credit: kiwi515/ogws

namespace Abstract {
namespace g3d {

/// @addr{0x80055540}
ChrAnmResult ResAnmChr::getAnmResult(f32 frame, size_t idx) const {
    s32 offset = parse<s32>(m_rawData->toChrDataDic);
    ResDic dic = ResDic(reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(m_rawData) + offset));
    const NodeData *data = reinterpret_cast<const NodeData *>(dic[idx]);

    u32 flags = parse<u32>(data->flags);

    ChrAnmResult result;
    result.flags = flags &
            (ChrAnmResult::Flag::FLAG_ANM_EXISTS | ChrAnmResult::Flag::FLAG_MTX_IDENT |
                    ChrAnmResult::Flag::FLAG_ROT_TRANS_ZERO | ChrAnmResult::Flag::FLAG_SCALE_ONE |
                    ChrAnmResult::Flag::FLAG_SCALE_UNIFORM | ChrAnmResult::Flag::FLAG_ROT_ZERO |
                    ChrAnmResult::Flag::FLAG_TRANS_ZERO | ChrAnmResult::Flag::FLAG_PATCH_SCALE |
                    ChrAnmResult::Flag::FLAG_PATCH_ROT | ChrAnmResult::Flag::FLAG_PATCH_TRANS |
                    ChrAnmResult::Flag::FLAG_SSC_APPLY | ChrAnmResult::Flag::FLAG_SSC_PARENT |
                    ChrAnmResult::Flag::FLAG_XSI_SCALING);

    u32 index = (flags & NodeData::Flag::FLAG_HAS_SRT_MASK) >> 22;

    ASSERT(index < s_getAnmResultTable.size());

    (*s_getAnmResultTable[index])(frame, result, m_infoData, data);

    return result;
}

/// @brief Animation keyframe
template <typename TData, typename TDerived>
class CResAnmChrFrmBase {
public:
    CResAnmChrFrmBase(const TData *pPtr) : mPtr(pPtr) {}

    void operator++(int) {
        mPtr++;
    }
    void operator--(int) {
        mPtr--;
    }

    [[nodiscard]] TDerived operator+(int n) {
        return TDerived(mPtr + n);
    }

protected:
    const TData *mPtr;
};

template <typename T>
class CResAnmChrFrm : public CResAnmChrFrmBase<T, CResAnmChrFrm<T>> {};

template <>
class CResAnmChrFrm<ResAnmChr::Frm48Data>
    : public CResAnmChrFrmBase<ResAnmChr::Frm48Data, CResAnmChrFrm<ResAnmChr::Frm48Data>> {
public:
    CResAnmChrFrm(const ResAnmChr::Frm48Data *pPtr) : CResAnmChrFrmBase(pPtr) {}

    [[nodiscard]] s16 GetFrame() const {
        return parse<s16>(mPtr->frame);
    }

    [[nodiscard]] f32 GetFrameF32() const {
        return static_cast<f32>(parse<s16>(mPtr->frame)) * (1.0f / 32.0f);
    }

    [[nodiscard]] f32 GetValue(const ResAnmChr::FVSData *pFVSData) const {
        f32 x = parse<f32>(pFVSData->fvs48.scale);
        f32 y = static_cast<f32>(parse<u16>(mPtr->value));
        f32 z = parse<f32>(pFVSData->fvs48.offset);

        return EGG::Mathf::fma(x, y, z);
    }

    [[nodiscard]] f32 GetSlope() const {
        return static_cast<f32>(parse<s16>(mPtr->slope)) * (1.0f / 256.0f);
    }
};

/// @brief Animation traits
template <typename T>
class CAnmFmtTraits {};

template <>
class CAnmFmtTraits<ResAnmChr::FVS48Data> {
public:
    typedef ResAnmChr::Frm48Data TFrmData;
    typedef s16 TFrame;

    [[nodiscard]] static CResAnmChrFrm<TFrmData> GetKeyFrame(const ResAnmChr::FVSData *pFVSData,
            int index) {
        return CResAnmChrFrm<TFrmData>(&pFVSData->fvs48.frameValues[index]);
    }

    [[nodiscard]] static TFrame QuantizeFrame(f32 frame) {
        return static_cast<TFrame>(frame * 32.0f);
    }
};

/// @brief Calculate frame values (FVS) result
template <typename T>
[[nodiscard]] f32 CalcResultFVS(f32 frame, const ResAnmChr::NodeData *nodeData,
        const ResAnmChr::NodeData::AnmData *anmData, bool constant) {
    if (constant) {
        return parse<f32>(anmData->constValue);
    }

    const ResAnmChr::AnmData *pFVSAnmData = reinterpret_cast<const ResAnmChr::AnmData *>(
            reinterpret_cast<uintptr_t>(nodeData) + parse<s32>(anmData->toResAnmChrAnmData));

    return CalcAnimationFVS<CAnmFmtTraits<T>>(frame, &pFVSAnmData->fvs);
}

[[nodiscard]] inline f32 CalcResult48(f32 frame, const ResAnmChr::NodeData *nodeData,
        const ResAnmChr::NodeData::AnmData *anmData, bool constant) {
    return CalcResultFVS<ResAnmChr::FVS48Data>(frame, nodeData, anmData, constant);
}

/// @brief Frame values (FVS) implementation
template <typename TTraits>
[[nodiscard]] f32 CalcAnimationFVS(f32 frame, const ResAnmChr::FVSData *pFVSData) {
    auto first = TTraits::GetKeyFrame(pFVSData, 0);
    auto last = TTraits::GetKeyFrame(pFVSData, parse<u16>(pFVSData->numFrameValues) - 1);

    if (frame <= first.GetFrameF32()) {
        return first.GetValue(pFVSData);
    }

    if (last.GetFrameF32() <= frame) {
        return last.GetValue(pFVSData);
    }

    f32 frameOffset = frame - first.GetFrameF32();
    f32 numKeyFrame = static_cast<f32>(parse<u16>(pFVSData->numFrameValues));

    f32 f_estimatePos = parse<f32>(pFVSData->invKeyFrameRange) * (frameOffset * numKeyFrame);
    u16 i_estimatePos = static_cast<u16>(f_estimatePos);

    auto left = TTraits::GetKeyFrame(pFVSData, i_estimatePos);
    auto quantized = TTraits::QuantizeFrame(frame);

    if (quantized < left.GetFrame()) {
        do {
            left--;
        } while (quantized < left.GetFrame());
    } else {
        do {
            left++;
        } while (left.GetFrame() <= quantized);

        left--;
    }

    if (frame == left.GetFrameF32()) {
        return left.GetValue(pFVSData);
    }

    auto right = left + 1;

    f32 v0 = left.GetValue(pFVSData);
    f32 t0 = left.GetSlope();
    f32 v1 = right.GetValue(pFVSData);
    f32 t1 = right.GetSlope();

    f32 f0 = left.GetFrameF32();
    f32 f1 = right.GetFrameF32();

    f32 frameDelta = frame - f0;
    f32 keyFrameDelta = f1 - f0;
    f32 keyFrameDeltaInv = EGG::Mathf::finv(keyFrameDelta);
    f32 t = frameDelta * keyFrameDeltaInv;
    f32 tMinus1 = EGG::Mathf::fms(frameDelta, keyFrameDeltaInv, 1.0f);
    f32 tanInterp = EGG::Mathf::fma(tMinus1, t0, t * t1);
    f32 scaledCurve = t * (EGG::Mathf::fms(2.0f, t, 3.0f) * (v0 - v1));

    return EGG::Mathf::fma(frameDelta * tMinus1, tanInterp, EGG::Mathf::fma(t, scaledCurve, v0));
}

const ResAnmChr::NodeData::AnmData *GetAnmScale(f32 frame, EGG::Vector3f &result,
        const ResAnmChr::NodeData *nodeData, const ResAnmChr::NodeData::AnmData *anmData) {
    u32 flags = parse<u32>(nodeData->flags);

    switch (flags & ResAnmChr::NodeData::Flag::FLAG_SCALE_FMT_MASK) {
    case 0:
    case ResAnmChr::NodeData::Flag::FLAG_SCALE_FVS32_FMT:
    case ResAnmChr::NodeData::Flag::FLAG_SCALE_FVS96_FMT:
        PANIC("GetAnmScale flags not handled!");
        break;
    case ResAnmChr::NodeData::Flag::FLAG_SCALE_FVS48_FMT: {
        result.x = CalcResult48(frame, nodeData, anmData++,
                flags & ResAnmChr::NodeData::FLAG_SCALE_X_CONST);

        if (flags & ResAnmChr::NodeData::FLAG_SCALE_UNIFORM) {
            result.y = result.x;
            result.z = result.x;
        } else {
            result.y = CalcResult48(frame, nodeData, anmData++,
                    flags & ResAnmChr::NodeData::Flag::FLAG_SCALE_Y_CONST);
            result.z = CalcResult48(frame, nodeData, anmData++,
                    flags & ResAnmChr::NodeData::Flag::FLAG_SCALE_Z_CONST);
        }
    } break;
    default: {
        result.setZero();
    }
    }

    return anmData;
}

void GetAnmResult_Stub(f32 /*frame*/, ChrAnmResult & /*result*/,
        const ResAnmChr::InfoData & /*infoData*/, const ResAnmChr::NodeData * /*nodeData*/) {}

void GetAnmResult_S(f32 frame, ChrAnmResult &result, const ResAnmChr::InfoData & /*infoData*/,
        const ResAnmChr::NodeData *nodeData) {
    GetAnmScale(frame, result.s, nodeData, nodeData->anms);
    result.rt = EGG::Matrix34f::ident;
}

const std::array<ResAnmChr::GetAnmResultFunc, ResAnmChr::NUM_RESULT_FUNCS>
        ResAnmChr::s_getAnmResultTable = {{
                &GetAnmResult_Stub,
                &GetAnmResult_S,
                &GetAnmResult_Stub,
                &GetAnmResult_Stub,
                &GetAnmResult_Stub,
                &GetAnmResult_Stub,
                &GetAnmResult_Stub,
                &GetAnmResult_Stub,
        }};

} // namespace g3d
} // namespace Abstract
