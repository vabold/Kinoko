#pragma once

#include "game/kart/KartMove.hh"
#include "game/kart/KartObjectManager.hh"

namespace Render {

/// @brief Tracks the current state of the front and backwards player cameras.
class KartCameraState {
    friend class KartCamera;

public:
    KartCameraState() {
        m_pos.setZero();
        m_bigAirHeight = 0.0f;
        m_1c = 0.0f;
        m_prevPos.setZero();
        m_dist = 0.0f;
        m_bigAirFallPitch = 0.0f;
        m_targetPos.setZero();
    }

    ~KartCameraState() = default;

    /// @addr{0x805A1C3C}
    void init() {
        m_dist = 0.0f;
        m_bigAirFallPitch = 0.0f;
    }

private:
    EGG::Vector3f m_pos;
    f32 m_bigAirHeight; ///< Additional camera height applied after 20 frames of airtime
    f32 m_1c;           ///< TODO: Seems to be related to boost-induced pitch
    EGG::Vector3f m_prevPos;
    f32 m_dist;                ///< Distance away from TODO
    f32 m_bigAirFallPitch;     ///< Height applied once you start falling after 20 frames of airtime
    EGG::Vector3f m_targetPos; ///< The position the camera looks towards
};

/// @brief Manager class for the forward and backwards cameras.
/// @details Responsible for setting the camera state and performing camera collision checks.
class KartCamera {
public:
    KartCamera();
    ~KartCamera();

    /// @addr{0x805A2034}
    void init() {
        auto *param = Kart::KartObjectManager::Instance()->object(0)->param();
        m_camParams = &param->camera().m_cameraEntries[1];

        initPos();
    }

    void calc();

    static KartCamera *CreateInstance();
    static void DestroyInstance();

    [[nodiscard]] static KartCamera *Instance() {
        return s_instance;
    }

private:
    /// @addr{0x805A2B84}
    void calcForward(f32 t, const Kart::KartObjectProxy *proxy) {
        m_forward = Interpolate(t, m_forward, proxy->move()->smoothedForward());
        m_right = EGG::Vector3f::ey.perpInPlane(m_forward, true);
    }

    void calcDriftOffset(const Kart::KartObjectProxy *proxy);
    void calcCamera(f32 param1, f32 param2, f32 param3, KartCameraState &state, bool isBackwards,
            const Kart::KartObjectProxy *proxy, const EGG::Vector3f &targetPos) const;
    void calcAirtimeHeight(KartCameraState &state, const Kart::KartObjectProxy *proxy) const;
    void initPos();

    void calcCollision(KartCameraState &state, bool isRear) const;

    /// @addr{0x805A2C34}
    static EGG::Vector3f Interpolate(f32 t, const EGG::Vector3f &v0, const EGG::Vector3f &v1) {
        return v0 + (v1 - v0) * t;
    }

    f32 m_driftYaw; ///< Rotation induced when drifting
    f32 m_hopPosY;  ///< Induces a downwards camera position offset
    EGG::Vector3f m_forward;
    EGG::Vector3f m_right;

    /// @warning Kinoko assumes the use of the 16:9 camera. It is possible for a time trial
    /// desync to occur due to the difference in camera distance between 4:3 and 16:9. This
    /// varying distance can cause an ObjectKCL transformation matrix to be updated for one
    /// camera's collision check but not the other. As far as we know, this can only occur on
    /// the DS Delfino Square @ref Field::ObjectTownBridge, and there are no naturally occurring
    /// desyncs of this type.
    const Kart::KartParam::KartCameraParam::KartCameraData *m_camParams;

    KartCameraState m_forwardCamera;  ///< Forward camera state
    KartCameraState m_backwardCamera; ///< Rear camera state

    static KartCamera *s_instance;
};

} // namespace Render
