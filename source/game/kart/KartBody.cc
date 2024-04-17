#include "KartBody.hh"

#include <egg/math/Math.hh>

namespace Kart {

KartBody::KartBody(KartPhysics *physics) : m_physics(physics) {
    m_anAngle = 0.0f;
}

// TODO LATER
EGG::Matrix34f KartBody::wheelMatrix(u16) {
    return EGG::Matrix34f::ident;
}

void KartBody::reset() {
    m_physics->reset();
    m_anAngle = 0.0f;
}

KartPhysics *KartBody::physics() const {
    return m_physics;
}

void KartBody::setAngle(f32 val) {
    m_anAngle = val;
}

KartBodyKart::KartBodyKart(KartPhysics *physics) : KartBody(physics) {}

KartBodyKart::~KartBodyKart() {
    delete m_physics;
}

KartBodyBike::KartBodyBike(KartPhysics *physics) : KartBody(physics) {}

KartBodyBike::~KartBodyBike() {
    delete m_physics;
}

EGG::Matrix34f KartBodyBike::wheelMatrix(u16 wheelIdx) {
    EGG::Matrix34f mat;

    mat.makeQT(fullRot(), pos());
    if (wheelIdx != 0) {
        return mat;
    }

    EGG::Vector3f position = param()->bikeDisp().m_handlePos * scale();
    EGG::Vector3f rotation = param()->bikeDisp().m_handleRot * DEG2RAD;

    EGG::Matrix34f handleMatrix;
    handleMatrix.makeRT(rotation, position);
    EGG::Matrix34f tmp = mat.multiplyTo(handleMatrix);

    EGG::Vector3f yRotation = EGG::Vector3f(0.0f, m_anAngle * DEG2RAD, 0.0f);
    EGG::Matrix34f yRotMatrix;
    yRotMatrix.makeR(yRotation);
    mat = tmp.multiplyTo(yRotMatrix);

    return mat;
}

} // namespace Kart
