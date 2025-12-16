#include "ObjectHeyhoShip.hh"

namespace Field {

/// @addr{0x806D18FC}
ObjectHeyhoShip::ObjectHeyhoShip(const System::MapdataGeoObj &params)
    : ObjectProjectileLauncher(params), m_yAmplitude(static_cast<f32>(params.setting(1))),
      m_frame(0) {
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

    if (tangent.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        tangent.normalise2();
    } else {
        tangent = EGG::Vector3f::ey;
    }

    m_framesSinceLastLaunch = 1000;
    setMatrixTangentTo(EGG::Vector3f::ey, tangent);
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
    constexpr f32 PERIOD = 100.0f;

    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);

    f32 fidx = DEG2FIDX * (360.0f * static_cast<f32>(++m_frame) / PERIOD);
    m_pos.y = m_yAmplitude * EGG::Mathf::SinFIdx(fidx) + m_pos.y;
}

} // namespace Field
