#include "KartJump.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartMove.hh"
#include "game/kart/KartParam.hh"
#include "game/kart/KartPhysics.hh"
#include "game/kart/KartState.hh"

#include "game/system/KPadController.hh"

#include <egg/math/Math.hh>

namespace Kart {

/// @addr{0x80575A44}
KartJump::KartJump(KartMove *move) : m_move(move) {
    m_cooldown = 0;

    // The base game doesn't initialize this explicitly, since EGG::Heaps are memset to 0.
    m_nextAllowTimer = 0;
}

/// @addr{0x80575AA8}
KartJump::~KartJump() = default;

/// @addr{0x805764FC}
void KartJump::calcRot() {
    m_angleDelta *= m_angleDeltaFactor;
    m_angleDelta = std::max(m_angleDelta, m_properties.angleDeltaMin);
    m_angleDeltaFactor -= m_angleDeltaFactorDec;
    m_angleDeltaFactor = std::max(m_angleDeltaFactor, m_properties.angleDeltaFactorMin);
    m_angle += m_angleDelta;
    m_angle = std::min(m_angle, m_finalAngle);

    switch (m_type) {
    case TrickType::KartFlipTrickZ:
        m_rot.setRPY(0.0f, 0.0f, -(m_angle * DEG2RAD) * m_rotSign);
        break;
    case TrickType::FlipTrickYLeft:
    case TrickType::FlipTrickYRight:
        m_rot.setRPY(0.0f, m_angle * DEG2RAD * m_rotSign, 0.0f);
        break;
    default:
        break;
    }

    physics()->composeStuntRot(m_rot);
}

/// @addr{0x80576460}
void KartJump::setupProperties() {
    static constexpr std::array<TrickProperties, 3> TRICK_PROPERTIES = {{
            {11.0f, 1.5f, 0.9f, 0.0018f},
            {14.0f, 1.5f, 0.9f, 0.0006f},
            {7.5f, 2.5f, 0.93f, 0.05f},
    }};

    static constexpr std::array<f32, 3> FINAL_ANGLES = {{
            360.0f,
            720.0f,
            180.0f,
    }};

    if (m_variant == SurfaceVariant::SingleFlipTrick) {
        m_properties = TRICK_PROPERTIES[0];
        m_finalAngle = FINAL_ANGLES[0];
    } else if (m_variant == SurfaceVariant::StuntTrick) {
        m_properties = TRICK_PROPERTIES[1];
        m_finalAngle = FINAL_ANGLES[1];
    } else if (m_type == TrickType::BikeSideStuntTrick) {
        m_properties = TRICK_PROPERTIES[2];
        m_finalAngle = FINAL_ANGLES[2];
    }

    m_angleDelta = m_properties.initialAngleDiff;
    m_angleDeltaFactorDec = m_properties.angleDiffMulDec;
    m_angle = 0.0f;
    m_angleDeltaFactor = 1.0f;
    m_rot = EGG::Quatf::ident;
}

/// @addr{0x80575AE8}
void KartJump::reset() {
    m_cooldown = 0;
}

/// @addr{0x80575D7C}
void KartJump::tryStart(const EGG::Vector3f &left) {
    auto &status = KartObjectProxy::status();

    if (status.offBit(eStatus::TrickStart)) {
        return;
    }

    if (m_move->speedRatioCapped() > 0.5f) {
        s32 boostRampType = state()->boostRampType();

        if (boostRampType == 0) {
            m_variant = SurfaceVariant::StuntTrick;
        } else if (boostRampType == 1) {
            m_variant = SurfaceVariant::SingleFlipTrick;
        } else {
            m_variant = SurfaceVariant::DoubleFlipTrick;
        }

        start(left);
    }

    status.resetBit(eStatus::TrickStart);
}

/// @addr{0x805763E4}
void KartJump::calc() {
    m_cooldown = std::max(0, m_cooldown - 1);

    if (status().onBit(eStatus::TrickRot)) {
        calcRot();
    }

    calcInput();
}

bool KartJump::someFlagCheck() {
    return status().onBit(eStatus::InAction, eStatus::TrickStart, eStatus::InATrick,
            eStatus::OverZipper);
}

/// @addr{0x80575B38}
void KartJump::calcInput() {
    constexpr s16 TRICK_ALLOW_TIMER = 14;

    System::Trick trick = inputs()->currentState().trick;

    if (!someFlagCheck() && trick != System::Trick::None) {
        m_nextTrick = trick;
        m_nextAllowTimer = TRICK_ALLOW_TIMER;
    }

    auto &status = KartObjectProxy::status();

    u32 airtime = state()->airtime();
    if (airtime == 0 || m_nextAllowTimer < 1 || airtime > 10 ||
            (status.offBit(eStatus::Trickable) && state()->boostRampType() < 0) ||
            someFlagCheck()) {
        m_nextAllowTimer = std::max(0, m_nextAllowTimer - 1);
    } else {
        if (airtime > 2) {
            status.setBit(eStatus::TrickStart);
        }
        if (status.onBit(eStatus::RampBoost)) {
            m_boostRampEnabled = true;
        }
    }
    if (status.onBit(eStatus::TouchingGround) &&
            collide()->surfaceFlags().offBit(KartCollide::eSurfaceFlags::BoostRamp)) {
        m_boostRampEnabled = false;
    }
}

/// @addr{0x805766B8}
void KartJump::end() {
    auto &status = KartObjectProxy::status();

    if (status.onBit(eStatus::TrickRot)) {
        physics()->composeDecayingStuntRot(m_rot);
    }

    status.resetBit(eStatus::InATrick, eStatus::TrickRot);
    m_boostRampEnabled = false;
}

/// @addr{0x80576230}
void KartJump::setAngle(const EGG::Vector3f &left) {
    static constexpr std::array<std::array<AngleProperties, 3>, 3> ANGLE_PROPERTIES = {{
            {{
                    {40.0f, 15.0f},
                    {45.0f, 20.0f},
                    {45.0f, 20.0f},
            }},
            {{
                    {36.0f, 13.0f},
                    {42.0f, 18.0f},
                    {42.0f, 18.0f},
            }},
            {{
                    {32.0f, 11.0f},
                    {39.0f, 16.0f},
                    {39.0f, 16.0f},
            }},
    }};

    f32 vel1YDot = m_move->vel1Dir().dot(EGG::Vector3f::ey);
    EGG::Vector3f vel1YCross = m_move->vel1Dir().cross(EGG::Vector3f::ey);
    f32 vel1YCrossMag = vel1YCross.length();
    f32 pitch = EGG::Mathf::abs(EGG::Mathf::atan2(vel1YCrossMag, vel1YDot));
    f32 angle = 90.0f - (pitch * RAD2DEG);
    u32 weightClass = static_cast<u32>(param()->stats().weightClass);
    f32 targetAngle = ANGLE_PROPERTIES[weightClass][static_cast<u32>(m_variant)].targetAngle;

    if (status().onBit(eStatus::JumpPad) || angle > targetAngle) {
        return;
    }

    f32 rotAngle = ANGLE_PROPERTIES[weightClass][static_cast<u32>(m_variant)].rotAngle;

    if (angle + rotAngle > targetAngle) {
        rotAngle = targetAngle - angle;
    }

    EGG::Matrix34f nextDir;
    nextDir.setAxisRotation(-rotAngle * DEG2RAD, left);
    m_move->setDir(nextDir.ps_multVector(m_move->dir()));
    m_move->setVel1Dir(m_move->dir());
}

/// @addr{0x80575EE8}
void KartJump::start(const EGG::Vector3f &left) {
    init();
    setAngle(left);
    status().setBit(eStatus::InATrick);
    m_cooldown = 5;
}

/// @addr{0x8057616C}
void KartJump::init() {
    if (m_variant == SurfaceVariant::DoubleFlipTrick) {
        m_type = TrickType::StuntTrickBasic;
        return;
    }

    if (m_nextTrick < System::Trick::Left) {
        m_type = TrickType::KartFlipTrickZ;
        m_rotSign = (m_nextTrick == System::Trick::Up) ? -1.0f : 1.0f;
    } else {
        m_type = static_cast<TrickType>(m_nextTrick);
        m_rotSign = (m_type == TrickType::FlipTrickYRight) ? -1.0f : 1.0f;
    }

    setupProperties();
    status().setBit(eStatus::TrickRot);
}

KartJumpBike::KartJumpBike(KartMove *move) : KartJump(move) {}

/// @addr{0x80576AFC}
KartJumpBike::~KartJumpBike() = default;

/// @addr{0x80576994}
void KartJumpBike::calcRot() {
    /// @brief Computed using double precision, so we hard-code it.
    constexpr f32 PI_OVER_9 = 0.34906584f;
    /// @brief Computed using double precision, so we hard-code it.
    constexpr f32 PI_OVER_3 = 1.0471976f;

    m_angleDelta *= m_angleDeltaFactor;
    m_angleDelta = std::max(m_angleDelta, m_properties.angleDeltaMin);
    m_angleDeltaFactor -= m_angleDeltaFactorDec;
    m_angleDeltaFactor = std::max(m_angleDeltaFactor, m_properties.angleDeltaFactorMin);
    m_angle += m_angleDelta;
    m_angle = std::min(m_angle, m_finalAngle);

    switch (m_type) {
    case TrickType::BikeFlipTrickNose:
    case TrickType::BikeFlipTrickTail:
        m_rot.setRPY(-(m_angle * DEG2RAD) * m_rotSign, 0.0f, 0.0f);
        break;
    case TrickType::FlipTrickYLeft:
    case TrickType::FlipTrickYRight:
        m_rot.setRPY(0.0f, m_angle * DEG2RAD * m_rotSign, 0.0f);
        break;
    case TrickType::BikeSideStuntTrick: {
        f32 sin = EGG::Mathf::SinFIdx(m_angle * DEG2FIDX);
        m_rot.setRPY(sin * -PI_OVER_9, (sin * m_rotSign) * -PI_OVER_3,
                (sin * m_rotSign) * PI_OVER_9);
    } break;
    default:
        break;
    }

    physics()->composeStuntRot(m_rot);
}

/// @addr{0x80576758}
void KartJumpBike::start(const EGG::Vector3f &left) {
    KartJump::start(left);

    KartMoveBike *moveBike = static_cast<KartMoveBike *>(m_move);
    moveBike->cancelWheelie();
}

/// @addr{0x8057689C}
void KartJumpBike::init() {
    constexpr f32 DOUBLE_FLIP_TRICK_FINAL_ANGLE = 180.0f;

    if (m_variant == SurfaceVariant::DoubleFlipTrick) {
        if (m_nextTrick < System::Trick::Left) {
            return;
        }

        m_type = TrickType::BikeSideStuntTrick;
        m_rotSign = (m_nextTrick == System::Trick::Right) ? -1.0f : 1.0f;
        setupProperties();
        m_finalAngle = DOUBLE_FLIP_TRICK_FINAL_ANGLE;
    } else {
        m_type = static_cast<TrickType>(m_nextTrick);
        m_rotSign =
                (m_type == TrickType::FlipTrickYRight || m_type == TrickType::BikeFlipTrickTail) ?
                -1.0f :
                1.0f;
        setupProperties();
    }

    status().setBit(eStatus::TrickRot);
}

} // namespace Kart
