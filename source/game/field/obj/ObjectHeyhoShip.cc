#include "ObjectHeyhoShip.hh"

namespace Field {

/// @addr{0x806D18FC}
ObjectHeyhoShip::ObjectHeyhoShip(const System::MapdataGeoObj &params)
    : ObjectProjectileLauncher(params),
      m_yAmplitude(static_cast<f32>(static_cast<s16>(params.setting(1)))), m_frameCount(0) {
    registerManagedObject();
}

/// @addr{0x806D2320}
ObjectHeyhoShip::~ObjectHeyhoShip() = default;

/// @addr{0x806D19D8}
void ObjectHeyhoShip::init() {
    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setPerPointVelocities(true);
    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);

    const EGG::Vector3f &railTan = m_railInterpolator->curTangentDir();
    EGG::Vector3f tangent = EGG::Vector3f(railTan.x, 0.0f, railTan.z);
    tangent.normalise2();
    tangent = RotateXZByYaw(F_PI / 2.0f, tangent);

    if (EGG::Mathf::abs(tangent.y) > 0.1f) {
        tangent.y = 0.01f;
    }

    m_tangent = tangent;

    if (m_tangent.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_tangent.normalise2();
    } else {
        m_tangent = EGG::Vector3f::ey;
    }

    m_up = EGG::Vector3f::ey;
    m_framesSinceLastLaunch = 1000;
    m_roll = 0.0f;
    setMatrixTangentTo(m_up, m_tangent);

    m_period = 100.0f;
}

/// @addr{0x806D1B9C}
void ObjectHeyhoShip::calc() {
    if (m_railInterpolator->calc() == RailInterpolator::Status::SegmentEnd) {
        m_framesSinceLastLaunch = 0;
    } else {
        ++m_framesSinceLastLaunch;
    }

    calcPos();
}

/// @addr{0x806D1D10}
void ObjectHeyhoShip::calcPos() {
    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);

    ++m_frameCount;
    f32 fidx = DEG2FIDX * (360.0f * static_cast<f32>(m_frameCount) / m_period);
    m_pos.y = m_yAmplitude * EGG::Mathf::SinFIdx(fidx) + m_pos.y;
}

} // namespace Field
