#include "ObjectBeltCrossing.hh"

namespace Field {

/// @addr{0x807FC764}
ObjectBeltCrossing::ObjectBeltCrossing(const System::MapdataGeoObj &params) : ObjectBelt(params) {
    m_roadVel = 28.0f;
}

/// @addr{0x807FD8A8}
ObjectBeltCrossing::~ObjectBeltCrossing() = default;

/// @addr{0x807FC7D8}
EGG::Vector3f ObjectBeltCrossing::calcRoadVelocity(u32 variant, const EGG::Vector3f & /*pos*/,
        s32 /*timeOffset*/) const {
    switch (variant) {
    case 0:
        return -EGG::Vector3f::ex * m_roadVel;
    case 1:
        return EGG::Vector3f::ex * m_roadVel;
    default:
        return EGG::Vector3f::zero;
    }
}

/// @addr{0x807FC874}
bool ObjectBeltCrossing::isMoving(u32 variant, const EGG::Vector3f & /*pos*/) const {
    return variant == 0 || variant == 1;
}

} // namespace Field
