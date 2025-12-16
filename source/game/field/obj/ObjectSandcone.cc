#include "ObjectSandcone.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x80686F84}
ObjectSandcone::ObjectSandcone(const System::MapdataGeoObj &params)
    : ObjectKCL(params), m_flowRate(static_cast<f32>(params.setting(0)) / 100.0f),
      m_finalHeightDelta(static_cast<f32>(params.setting(1))), m_startFrame(params.setting(2)) {
    m_baseMtx.makeRT(m_rot, m_pos);
}

/// @addr{0x806871E0}
ObjectSandcone::~ObjectSandcone() = default;

/// @addr{0x806872A0}
void ObjectSandcone::init() {
    m_duration = static_cast<u16>(m_finalHeightDelta / m_flowRate);
    m_currentMtx = m_baseMtx;

    // Moved from getUpdatedMatrix to init b/c this only needs to be computed once per object.
    m_finalPos = m_pos + EGG::Vector3f::ey * (static_cast<f32>(m_duration) * m_flowRate);
}

/// @addr{0x806873Bc}
void ObjectSandcone::calc() {
    m_flags.setBit(eFlags::Matrix);
    m_transform = getUpdatedMatrix(0);
    m_pos = m_transform.base(3);
}

/// @addr{0x80687800}
const EGG::Matrix34f &ObjectSandcone::getUpdatedMatrix(u32 timeOffset) {
    m_currentMtx = m_baseMtx;

    s32 t = static_cast<s32>(System::RaceManager::Instance()->timer() - timeOffset);
    s32 startFrame = static_cast<s32>(m_startFrame);

    if (t > startFrame + static_cast<s32>(m_duration)) {
        // The sandcone has finished "flowing", so just return the final position.
        // For Kinoko, we introduce a slight performance improvement by caching the m_finalPos.
        m_currentMtx.setBase(3, m_finalPos);
    } else if (t > startFrame) {
        EGG::Vector3f deltaPos = EGG::Vector3f::ey * ((t - startFrame) * m_flowRate);
        m_currentMtx.setBase(3, m_baseMtx.base(3) + deltaPos);
    }

    return m_currentMtx;
}

/// @addr{0x80687A2C}
bool ObjectSandcone::checkCollision(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);

    return m_objColMgr->checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80687CC0}
bool ObjectSandcone::checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);

    return m_objColMgr->checkSphereCachedFullPush(radius, pos, prevPos, mask, info, maskOut);
}

} // namespace Field
