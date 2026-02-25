#include "KartCamera.hh"

#include "game/field/CollisionDirector.hh"

namespace Render {

/// @addr{0x805A1D10}
KartCamera::KartCamera() : m_hopPosY(0), m_forward(EGG::Vector3f::zero), m_camParams(nullptr) {}

KartCamera::~KartCamera() = default;

/// @addr{0x805A21D0}
void KartCamera::calc() {
    constexpr f32 HOP_POS_INTERP_RATE = 0.8f;
    constexpr f32 FORWARD_INTERP_RATE = 0.35f;
    constexpr f32 HORIZ_POS_INTERP_RATE = 0.05f;
    constexpr f32 VERT_POS_INTERP_RATE = 0.03f;
    constexpr f32 FAST_VERT_POS_INTERP_RATE = 0.1f;

    const auto *kartObj = Kart::KartObjectManager::Instance()->object(0);
    EGG::Vector3f targetPos = kartObj->pos();
    m_hopPosY += HOP_POS_INTERP_RATE * (kartObj->move()->hopPosY() - m_hopPosY);
    targetPos.y -= m_hopPosY;

    calcForward(FORWARD_INTERP_RATE, kartObj);
    calcDriftOffset(kartObj);

    calcCamera(HORIZ_POS_INTERP_RATE, VERT_POS_INTERP_RATE, FAST_VERT_POS_INTERP_RATE,
            m_forwardCamera, false, kartObj, targetPos);
    calcCamera(HORIZ_POS_INTERP_RATE, VERT_POS_INTERP_RATE, FAST_VERT_POS_INTERP_RATE,
            m_backwardCamera, true, kartObj, targetPos);

    calcCollision(m_forwardCamera, false);
    calcCollision(m_backwardCamera, true);
}

KartCamera *KartCamera::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new KartCamera;
    return s_instance;
}

void KartCamera::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

/// @addr{0x805A3070}
void KartCamera::calcDriftOffset(const Kart::KartObjectProxy *proxy) {
    constexpr f32 MAX_DRIFT_YAW_AUTOMATIC = 10.0f;
    constexpr f32 MAX_DRIFT_YAW_MANUAL = 15.0f;
    constexpr f32 YAW_STEP_AUTOMATIC = 0.5f;
    constexpr f32 YAW_STEP_MANUAL = 1.0f;

    const auto &status = proxy->status();
    if (status.onBit(Kart::eStatus::DriftManual, Kart::eStatus::Hop)) {
        f32 yaw = static_cast<f32>(proxy->hopStickX()) * -0.5f;
        if (proxy->vehicleType() == Kart::KartParam::Stats::DriftType::Inside_Drift_Bike) {
            yaw *= -1.0f;
        }
        m_driftYaw += yaw;

        f32 yawMax = status.onBit(Kart::eStatus::AutoDrift) ? MAX_DRIFT_YAW_AUTOMATIC :
                                                              MAX_DRIFT_YAW_MANUAL;
        f32 pitch = 90.0f - RAD2DEG * EGG::Mathf::acos(m_forward.dot(EGG::Vector3f::ey));
        if (pitch < 0.0f) {
            yawMax += 0.1f * EGG::Mathf::abs(pitch);
        }

        m_driftYaw = std::clamp(m_driftYaw, -yawMax, yawMax);
    } else if (m_driftYaw != 0.0f) {
        f32 yawStep = status.onBit(Kart::eStatus::AutoDrift) ? YAW_STEP_AUTOMATIC : YAW_STEP_MANUAL;
        if (m_driftYaw > 0.0f) {
            m_driftYaw = std::max(0.0f, m_driftYaw - yawStep);
        } else {
            m_driftYaw = std::min(0.0f, m_driftYaw + yawStep);
        }
    }
}

/// @addr{0x805A34B0}
void KartCamera::calcCamera(f32 horizInterpRate, f32 vertInterpRate, f32 fastVertInterpRate,
        KartCameraState &state, bool isBackwards, const Kart::KartObjectProxy *proxy,
        const EGG::Vector3f &targetPos) const {
    EGG::Vector3f forwardDir = isBackwards ? -m_forward : m_forward;
    EGG::Vector3f pitchAxis = m_right.cross(forwardDir);
    pitchAxis.normalise();

    f32 driftYaw = DEG2RAD * m_driftYaw;

    const auto &status = proxy->status();
    if (status.onBit(Kart::eStatus::AutoDrift)) {
        driftYaw *= -1.0f;
    }
    f32 bigAirPitch = std::min(1.553343f, state.m_bigAirFallPitch);

    EGG::Matrix34f bigAirPitchMat;
    EGG::Matrix34f yawRotMat;
    yawRotMat.setAxisRotation(driftYaw, m_right);
    bigAirPitchMat.setAxisRotation(bigAirPitch, pitchAxis);
    EGG::Vector3f orbitDir = yawRotMat.ps_multVector(bigAirPitchMat.ps_multVector(forwardDir));

    f32 pitch = 90.0f -
            RAD2DEG * EGG::Mathf::acos(std::clamp(orbitDir.dot(EGG::Vector3f::ey), -1.0f, 1.0f));

    // If camera is looking downwards
    if (pitch > 0.0f) {
        EGG::Matrix34f downPitchMat;
        f32 fVar16 = status.onBit(Kart::eStatus::TouchingGround) ? 0.1f : 0.6f;
        state.m_1c = state.m_1c + 0.2f * (fVar16 - state.m_1c);
        downPitchMat.setAxisRotation(DEG2RAD * pitch * state.m_1c, pitchAxis);
        orbitDir = downPitchMat.ps_multVector(orbitDir);
    }
    orbitDir *= -1.0f;
    calcAirtimeHeight(state, proxy);
    state.m_pos += targetPos - state.m_targetPos;
    EGG::Vector3f posOffset = state.m_dist * orbitDir;
    posOffset.y += m_camParams->posY - proxy->cameraDistY() + state.m_bigAirHeight;

    EGG::Vector3f horizDelta = targetPos + posOffset - state.m_pos;
    f32 vertDelta = horizDelta.y;
    horizDelta.y = 0.0f;
    state.m_pos += horizInterpRate * horizDelta;

    if (status.onBit(Kart::eStatus::TouchingGround) && proxy->speed() > 30.0f && pitch > 15.0f) {
        vertInterpRate = fastVertInterpRate;
    }
    state.m_pos.y += vertDelta * vertInterpRate;

    EGG::Vector3f targetDelta = state.m_pos - targetPos;
    if (targetDelta.ps_length() != 0.0f) {
        state.m_pos = targetPos + state.m_dist * targetDelta.ps_normalize();
    }

    state.m_targetPos = targetPos;
}

/// @addr{0x805A463C}
void KartCamera::calcAirtimeHeight(KartCameraState &state,
        const Kart::KartObjectProxy *proxy) const {
    f32 targetPitch = 0.0f;
    f32 interpRate = 0.2f;
    const auto &status = proxy->status();

    if (status.onBit(Kart::eStatus::AirtimeOver20)) {
        state.m_bigAirHeight = std::min(300.0f, state.m_bigAirHeight + 10.0f);

        const EGG::Vector3f &vel = proxy->velocity();
        if (vel.y < 0.0f) {
            targetPitch = std::min(0.3f, 0.005f * -vel.y);
        }
        interpRate = 0.03f;
    } else {
        state.m_bigAirHeight = 0.0f;
    }

    state.m_bigAirFallPitch += interpRate * (targetPitch - state.m_bigAirFallPitch);
}

/// @addr{0x805A49BC}
void KartCamera::initPos() {
    constexpr f32 FORWARD_INTERP_RATE = 1.0f;
    constexpr f32 HORIZ_POS_INTERP_RATE = 1.0f;
    constexpr f32 VERT_POS_INTERP_RATE = 1.0f;
    constexpr f32 FAST_VERT_POS_INTERP_RATE = 1.0f;

    m_driftYaw = 0.0f;
    m_forwardCamera.init();
    m_backwardCamera.init();
    m_backwardCamera.m_dist = m_camParams->dist;
    m_forwardCamera.m_dist = m_camParams->dist;

    const auto *kartObj = Kart::KartObjectManager::Instance()->object(0);
    calcForward(FORWARD_INTERP_RATE, kartObj);
    calcCamera(HORIZ_POS_INTERP_RATE, VERT_POS_INTERP_RATE, FAST_VERT_POS_INTERP_RATE,
            m_forwardCamera, false, kartObj, kartObj->pos());
}

/// @addr{0x805A5D70}
void KartCamera::calcCollision(KartCameraState &state, bool isBackwards) const {
    constexpr f32 FRONT_RADIUS = 80.0f;
    constexpr f32 BACK_RADIUS = 140.0f;

    f32 radius = isBackwards ? BACK_RADIUS : FRONT_RADIUS;
    Field::CollisionInfo info;
    Field::KCLTypeMask mask;

    if (!Field::CollisionDirector::Instance()->checkSphereFullPush(radius, state.m_pos,
                state.m_prevPos, KCL_TYPE_62F8BDFF, &info, &mask, 0)) {
        state.m_prevPos = state.m_pos;
    }
}

KartCamera *KartCamera::s_instance = nullptr;

} // namespace Render
