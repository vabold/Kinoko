#include "ObjectTownBridge.hh"

#include "game/system/RaceManager.hh"

#include <algorithm>

namespace Field {

/// @addr{0x80809448}
ObjectTownBridge::ObjectTownBridge(const System::MapdataGeoObj &params)
    : ObjectKCL(params), m_rotateUpwards(m_rot.y < 0.0f),
      m_angVel(static_cast<f32>(params.setting(0))),
      m_pivotFrames(static_cast<s32>(params.setting(1))),
      m_raisedFrames(static_cast<s32>(params.setting(2))),
      m_loweredFrames(static_cast<s32>(params.setting(3))),
      m_fullAnimFrames(m_pivotFrames * 2 + (m_loweredFrames + m_raisedFrames)) {
    m_state = State::Raising;
}

/// @addr{0x8080ACE0}
ObjectTownBridge::~ObjectTownBridge() {
    // Whichever ObjColMgr is active will be destroyed naturally as part of ObjectKCL's destructor.
    // We need to destroy the other ones to avoid leaking. The base game does not bother doing this.
    if (m_flatColMgr != m_objColMgr) {
        delete m_flatColMgr;
    }

    if (m_midColMgr != m_objColMgr) {
        delete m_midColMgr;
    }

    if (m_raisedColMgr != m_objColMgr) {
        delete m_raisedColMgr;
    }
}

/// @addr{0x80809774}
void ObjectTownBridge::calc() {
    s32 t = System::RaceManager::Instance()->timer();
    f32 angle = calcBridgeAngle(t);

    // Set the object collision based off the angle of the bridge.
    // The thresholds are >30 degrees for "raised", >10 degrees for "middle", otherwise "flat".
    angle = std::clamp(angle, -45.0f, 45.0f);
    f32 absAng = EGG::Mathf::abs(angle);

    if (absAng <= 10.0f) {
        m_objColMgr = m_flatColMgr;
    } else if (absAng <= 30.0f) {
        m_objColMgr = m_midColMgr;
    } else {
        m_objColMgr = m_raisedColMgr;
    }

    m_flags.setBit(eFlags::Rotation);
    m_rot.z = F_PI * angle / 180.0f;
    m_state = calcState(t);
}

/// @addr{0x808095B8}
void ObjectTownBridge::createCollision() {
    ObjectKCL::createCollision();

    const char *name = getKclName();
    auto *resMgr = System::ResourceManager::Instance();
    char filepath[128];

    snprintf(filepath, sizeof(filepath), "%s2.kcl", name);
    m_midColMgr = new ObjColMgr(resMgr->getFile(filepath, nullptr, System::ArchiveId::Course));

    snprintf(filepath, sizeof(filepath), "%s3.kcl", name);
    m_flatColMgr = new ObjColMgr(resMgr->getFile(filepath, nullptr, System::ArchiveId::Course));

    m_raisedColMgr = m_objColMgr;
}

/// @addr{0x80809CDC}
f32 ObjectTownBridge::calcBridgeAngle(s32 t) const {
    u32 animFrame = t % m_fullAnimFrames;
    State state = calcState(animFrame);

    switch (state) {
    case State::Raised: {
        s32 sign = m_rotateUpwards ? -1 : 1;
        return m_angVel * static_cast<f32>(sign);
    } break;
    case State::Lowered: {
        return 0.0f;
    } break;
    case State::Raising: {
        s32 sign = m_rotateUpwards ? -1 : 1;
        return m_angVel * static_cast<f32>((static_cast<s32>(animFrame) * sign)) /
                static_cast<f32>(m_pivotFrames);
    } break;
    case State::Lowering: {
        s32 sign = m_rotateUpwards ? -1 : 1;
        u32 rot = m_pivotFrames - (animFrame - (m_pivotFrames + m_raisedFrames));
        return m_angVel * static_cast<f32>(sign * static_cast<s32>(rot)) /
                static_cast<f32>(m_pivotFrames);
    } break;
    default:
        return 0.0f;
    }
}

/// @brief Helper function which determines the current state of the bridge based on t.
ObjectTownBridge::State ObjectTownBridge::calcState(s32 t) const {
    if (t < m_pivotFrames) {
        return State::Raising;
    }

    if (t < m_pivotFrames + m_raisedFrames) {
        return State::Raised;
    }

    if (t < m_pivotFrames * 2 + m_raisedFrames) {
        return State::Lowering;
    }

    return State::Lowered;
}

} // namespace Field
