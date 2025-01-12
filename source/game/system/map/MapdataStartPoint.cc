#include "MapdataStartPoint.hh"

#include "game/system/CourseMap.hh"
#include "game/system/map/MapdataStageInfo.hh"

#include <egg/math/Math.hh>
#include <egg/math/Quat.hh>

namespace System {

// We have to define these early so they're available for findKartStartPoint
static constexpr s8 X_TRANSLATION_TABLE[12][12] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {-5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {-10, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {-10, 5, -5, 10, 0, 0, 0, 0, 0, 0, 0, 0},
        {-10, 0, 10, -5, 5, 0, 0, 0, 0, 0, 0, 0},
        {-10, -2, 6, -6, 2, 10, 0, 0, 0, 0, 0, 0},
        {-5, 5, -10, 0, 10, -5, 5, 0, 0, 0, 0, 0},
        {-10, 0, 10, -5, 5, -10, 0, 10, 0, 0, 0, 0},
        {-10, -2, 6, -6, 2, 10, -10, -2, 6, 0, 0, 0},
        {-10, 0, 10, -5, 5, -10, 0, 10, -5, 5, 0, 0},
        {-10, -2, 6, -6, 2, 10, -10, -2, 6, -6, 2, 0},
        {-10, -2, 6, -6, 2, 10, -10, -2, 6, -6, 2, 10},
};

static constexpr s8 Z_TRANSLATION_TABLE[12][12] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 2, 2, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 2, 2, 2, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 2, 2, 2, 0, 0, 0},
        {0, 0, 0, 1, 1, 2, 2, 2, 3, 3, 0, 0},
        {0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 0},
        {0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3},
};

MapdataStartPoint::MapdataStartPoint(const SData *data) : m_rawData(data) {
    EGG::RamStream stream = EGG::RamStream(data, sizeof(SData));
    read(stream);
}

void MapdataStartPoint::read(EGG::Stream &stream) {
    m_position.read(stream);
    m_rotation.read(stream);
    if (CourseMap::Instance()->version() > 1830) {
        m_playerIndex = stream.read_s16();
    } else {
        m_playerIndex = 0;
    }
}

/// @addr{0x80514368}
/// @note vabold: Chances are these variables are never getting good names. Good luck!
void MapdataStartPoint::findKartStartPoint(EGG::Vector3f &pos, EGG::Vector3f &angles, u8 placement,
        u8 playerCount) {
    EGG::Quatf rotation;
    rotation.setRPY(m_rotation * DEG2RAD);

    EGG::Vector3f zAxis = rotation.rotateVector(-EGG::Vector3f::ez);
    EGG::Vector3f xAxis = rotation.rotateVector(-EGG::Vector3f::ex);

    CourseMap *courseMap = CourseMap::Instance();
    const MapdataStageInfo *stageInfo = courseMap->getStageInfo();
    ASSERT(stageInfo);
    int translationDirection = stageInfo->polePosition() == 1 ? -1 : 1;

    f32 cos = EGG::Mathf::CosFIdx(courseMap->startTmpAngle() * DEG2FIDX);
    f32 sin = EGG::Mathf::SinFIdx(courseMap->startTmpAngle() * DEG2FIDX) * translationDirection;

    int xTranslation = translationDirection * X_TRANSLATION_TABLE[playerCount - 1][0];
    f32 xScalar =
            sin * (courseMap->startTmp0() * (static_cast<f32>(xTranslation) + 10.0f) / 10.0f) / cos;
    EGG::Vector3f xTmp = -zAxis * xScalar;

    int zTranslation = Z_TRANSLATION_TABLE[playerCount - 1][placement];
    f32 zScalar = courseMap->startTmp2() * static_cast<f32>(zTranslation / 2) +
            courseMap->startTmp1() * static_cast<f32>(zTranslation) +
            courseMap->startTmp3() * static_cast<f32>((zTranslation + 1) / 2);
    EGG::Vector3f zTmp = zAxis * zScalar;

    EGG::Vector3f tmp0 = xTmp + zTmp;
    EGG::Vector3f tmp1 = xAxis * courseMap->startTmp0();
    EGG::Vector3f tmp2 = tmp0 - tmp1;
    EGG::Vector3f tmpPos = tmp2 + m_position;

    EGG::Vector3f vCos = xAxis * cos;
    EGG::Vector3f vSin = zAxis * sin;
    EGG::Vector3f vRes = vCos + vSin;

    int tmpTranslation = translationDirection * X_TRANSLATION_TABLE[playerCount - 1][placement];
    f32 tmpScalar =
            courseMap->startTmp0() * (static_cast<f32>(tmpTranslation) + 10.0f) / (cos * 10.0f);
    EGG::Vector3f tmpRes = vRes * tmpScalar;

    pos = tmpPos + tmpRes;
    angles = m_rotation;
}

/// @addr{0x80514258}
MapdataStartPointAccessor::MapdataStartPointAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataStartPoint, MapdataStartPoint::SData>(header) {
    if (CourseMap::Instance()->version() > 1830) {
        init(reinterpret_cast<const MapdataStartPoint::SData *>(m_sectionHeader + 1),
                parse<u16>(m_sectionHeader->count));
    } else {
        init(reinterpret_cast<const MapdataStartPoint::SData *>(
                     reinterpret_cast<const u8 *>(m_sectionHeader + 4)),
                1);
    }
}

MapdataStartPointAccessor::~MapdataStartPointAccessor() = default;

} // namespace System
