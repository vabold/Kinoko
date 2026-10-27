#include "KartBody.hh"

#include <egg/math/Math.hh>

namespace Kinoko::Kart {

/// @addr{0x8056C394}
KartBody::KartBody(KartPhysics *physics) : m_physics(physics) {
    m_anAngle = 0.0f;
    m_sinkDepth = 0.0f;
    m_targetSinkDepth = 0.0f;
}

/// @addr{0x8056C604}
/// @brief Computes a matrix to represent wheel rotation. For Karts, this is wheel-agnostic.
/// @param bodyMat The kart's rotation and position matrix
/// @details For Kinoko, we introduce a performance optimization by having the caller compute
/// `bodyMat` once for all wheels, rather than recomputing on each `wheelMatrix` call.
EGG::Matrix34f KartBody::wheelMatrix(u16, const EGG::Matrix34f &bodyMat) {
    return bodyMat;
}

/// @addr{0x8056C458}
void KartBody::init(const KartParam::KartDisp &kartDisp, const KartParam::BikeDisp &bikeDisp) {
    setParams(kartDisp, bikeDisp);
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

/// @addr{0x8056CCC0}
KartBodyKart::KartBodyKart(KartPhysics *physics) : KartBody(physics) {}

/// @addr{0x8056E494}
KartBodyKart::~KartBodyKart() {
    EGG::egg_delete(m_physics);
}

/// @addr{0x8056D858}
KartBodyBike::KartBodyBike(KartPhysics *physics) : KartBody(physics) {}

/// @addr{0x8056E2BC}
KartBodyBike::~KartBodyBike() {
    EGG::egg_delete(m_physics);
}

/// @addr{0x8056DD54}
/// @brief Computes a matrix to represent the rotation of a wheel.
/// @details For the front wheel, we factor in the handlebar rotation.
/// For the rear wheel, we only factor in the kart's rotation.
/// For Kinoko, we introduce a performance optimization by having the caller compute `bodyMat`
/// once for all wheels, rather than recomputing on each `wheelMatrix` call.
/// @param wheelIdx 0 for front wheel, 1 for rear wheel
/// @param bodyMat The kart's rotation and position matrix
EGG::Matrix34f KartBodyBike::wheelMatrix(u16 wheelIdx, const EGG::Matrix34f &bodyMat) {
    if (wheelIdx != 0) {
        return bodyMat;
    }

    m_handleRotMat.setBase(3, m_handlePos * scale());
    EGG::Matrix34f tmp = bodyMat.multiplyTo(m_handleRotMat);

    EGG::Vector3f yRotation = EGG::Vector3f(0.0f, DEG2RAD * m_anAngle, 0.0f);
    EGG::Matrix34f yRotMatrix;
    yRotMatrix.makeR(yRotation);

    return tmp.multiplyTo(yRotMatrix);
}

/// @addr{0x8056D970}
void KartBodyBike::setParams(const KartParam::KartDisp & /* kartDisp */,
        const KartParam::BikeDisp &bikeDisp) {
    m_handlePos = bikeDisp.m_handlePos;
    m_handleRotMat.makeR(DEG2RAD * bikeDisp.m_handleRot);
}

KartBodyQuacker::KartBodyQuacker(KartPhysics *physics) : KartBodyBike(physics) {}

KartBodyQuacker::~KartBodyQuacker() = default;

/// @param bodyMat The kart's rotation and position matrix
/// @details For Kinoko, we introduce a performance optimization by having the caller compute
/// `bodyMat` once for all wheels, rather than recomputing on each `wheelMatrix` call.
EGG::Matrix34f KartBodyQuacker::wheelMatrix(u16 /* wheelIdx */, const EGG::Matrix34f &bodyMat) {
    return bodyMat;
}

} // namespace Kinoko::Kart
