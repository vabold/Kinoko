#include "ObjectBeltEasy.hh"

namespace Field {

/// @addr{0x807FC578}
ObjectBeltEasy::ObjectBeltEasy(const System::MapdataGeoObj &params) : ObjectBelt(params) {
    m_roadVel = 20.0f;
}

/// @addr{0x807FD8F0}
ObjectBeltEasy::~ObjectBeltEasy() = default;

/// @addr{0x807FC62C}
EGG::Vector3f ObjectBeltEasy::calcRoadVelocity(u32 variant, const EGG::Vector3f & /*pos*/,
        s32 /*timeOffset*/) const {
    switch (variant) {
    case 2:
        return EGG::Vector3f::ex * m_roadVel;
    case 3:
        return -EGG::Vector3f::ex * m_roadVel;
    default:
        return EGG::Vector3f::zero;
    }
}

} // namespace Field
