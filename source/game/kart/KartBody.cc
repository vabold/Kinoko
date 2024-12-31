#include "KartBody.hh"

#include <egg/math/Math.hh>

namespace Kart {

/// @addr{0x8056C394}
KartBody::KartBody(KartPhysics *physics) : m_physics(physics) {
    m_anAngle = 0.0f;
    m_sinkDepth = 0.0f;
    m_targetSinkDepth = 0.0f;
}

/// @addr{0x8056C604}
/// @brief Computes a matrix to represent wheel rotation. For Karts, this is wheel-agnostic.
EGG::Matrix34f KartBody::wheelMatrix(u16) {
    EGG::Matrix34f mat;
    mat.makeQT(fullRot(), pos());
    return mat;
}

/// @addr{0x8056C4B4}
void KartBody::reset() {
    m_physics->reset();
    m_anAngle = 0.0f;
    m_sinkDepth = 0.0f;
    m_targetSinkDepth = 0.0f;
}

/// @addr{0x8056C9C4}
void KartBody::calcSinkDepth() {
    m_sinkDepth += (m_targetSinkDepth - m_sinkDepth) * 0.1f;
}

/// @addr{0x8056C950}
void KartBody::trySetTargetSinkDepth(f32 val) {
    m_targetSinkDepth = std::max(val, m_targetSinkDepth);
}

/// @addr{0x8056C964}
void KartBody::calcTargetSinkDepth() {
    m_targetSinkDepth = 3.0f * static_cast<f32>(collisionData().intensity);
}

/// @addr{0x8056E424}
void KartBody::setAngle(f32 val) {
    m_anAngle = val;
}

KartPhysics *KartBody::physics() const {
    return m_physics;
}

f32 KartBody::sinkDepth() const {
    return m_sinkDepth;
}

/// @addr{0x8056CCC0}
KartBodyKart::KartBodyKart(KartPhysics *physics) : KartBody(physics) {}

/// @addr{0x8056E494}
KartBodyKart::~KartBodyKart() {
    delete m_physics;
}

/// @addr{0x8056D858}
KartBodyBike::KartBodyBike(KartPhysics *physics) : KartBody(physics) {}

/// @addr{0x8056E2BC}
KartBodyBike::~KartBodyBike() {
    delete m_physics;
}

/// @addr{0x8056DD54}
/// @brief Computes a matrix to represent the rotation of a wheel.
/// @details For the front wheel, we factor in the handlebar rotation.
/// For the rear wheel, we only factor in the kart's rotation.
/// @param wheelIdx 0 for front wheel, 1 for rear wheel
EGG::Matrix34f KartBodyBike::wheelMatrix(u16 wheelIdx) {
    EGG::Matrix34f mat;

    mat.makeQT(fullRot(), pos());
    if (wheelIdx != 0) {
        return mat;
    }

    EGG::Vector3f position = param()->bikeDisp().m_handlePos * scale();
    EGG::Vector3f rotation = DEG2RAD * param()->bikeDisp().m_handleRot;

    EGG::Matrix34f handleMatrix;
    handleMatrix.makeRT(rotation, position);
    EGG::Matrix34f tmp = mat.multiplyTo(handleMatrix);

    EGG::Vector3f yRotation = EGG::Vector3f(0.0f, DEG2RAD * m_anAngle, 0.0f);
    EGG::Matrix34f yRotMatrix;
    yRotMatrix.makeR(yRotation);
    mat = tmp.multiplyTo(yRotMatrix);

    return mat;
}

KartBodyQuacker::KartBodyQuacker(KartPhysics *physics) : KartBodyBike(physics) {}

KartBodyQuacker::~KartBodyQuacker() = default;

EGG::Matrix34f KartBodyQuacker::wheelMatrix(u16 /* wheelIdx */) {
    EGG::Matrix34f mat;
    mat.makeQT(fullRot(), pos());
    return mat;
}

} // namespace Kart
