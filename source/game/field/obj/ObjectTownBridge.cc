#include "ObjectTownBridge.hh"

#include "game/field/ObjColMgr.hh"

#include "game/system/RaceManager.hh"
#include "game/system/ResourceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

#include <algorithm>

namespace Kinoko::Field {

/// @addr{0x80809448}
/// @copybrief ObjectKCL::ObjectKCL(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Sets @ref m_rotateUpwards based on whether the drawbridge is oriented upright. Sets
/// @ref m_raisedAngle based on param setting 1, @ref m_pivotFrames based on param setting 2,
/// @ref m_raisedFrames based on param setting 3, and @ref m_loweredFrames based on param
/// setting 4. Computes @ref m_fullAnimFrames by doubling @ref m_pivotFrames and summing with
/// @ref m_loweredFrames and @ref m_raisedFrames.
ObjectTownBridge::ObjectTownBridge(const System::MapdataGeoObj &params)
    : ObjectKCL(params),
      m_rotateUpwards(rot().y < 0.0f),
      m_raisedAngle(static_cast<float>(params.setting(0))),
      m_pivotFrames(static_cast<u32>(params.setting(1))),
      m_raisedFrames(static_cast<u32>(params.setting(2))),
      m_loweredFrames(static_cast<u32>(params.setting(3))),
      m_fullAnimFrames(m_pivotFrames * 2 + (m_loweredFrames + m_raisedFrames)) {}

/// @addr{0x8080ACE0}
/// @brief Virtual destructor that deletes all inactive object collision managers
ObjectTownBridge::~ObjectTownBridge() {
    // Whichever ObjColMgr is active will be destroyed naturally as part of ObjectKCL's
    // destructor. We need to destroy the other ones to avoid leaking. The base game does not
    // bother doing this.
    if (m_flatColMgr != m_objColMgr) {
        EGG::egg_delete(m_flatColMgr);
    }

    if (m_midColMgr != m_objColMgr) {
        EGG::egg_delete(m_midColMgr);
    }

    if (m_raisedColMgr != m_objColMgr) {
        EGG::egg_delete(m_raisedColMgr);
    }
}

/// @addr{0x80809774}
/// @copybrief ObjectBase::calc()
/// @details Calculates the drawbridge's current angle based on the number of frames elapsed in the
/// race. If the drawbridge's angle above the floor is less than or equal to `10.0f` degrees then it
/// uses @ref m_flatColMgr, if less than or equal to `30.0f` it uses @ref m_midColMgr, otherwise it
/// uses @ref m_raisedColMgr. Updates the drawbridge's rotation based on the computed angle.
void ObjectTownBridge::calc() {
    u32 t = System::RaceManager::Instance()->timer();
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

    setRot(EGG::Vector3f(rot().x, rot().y, F_PI * angle / 180.0f));
}

/// @addr{0x808095B8}
/// @copybrief ObjectBase::createCollision()
/// @details Creates the collision managers for the bridge's different states. `TownBridgeDS.kcl`
/// represents the drawbridge's collision when it is near/fully upright, `TownBridgeDS2.kcl`
/// represents when it's in the middle of raising or lowering, and `TownBridgeDS3.kcl` represents
/// when it is near/fully lowered.
void ObjectTownBridge::createCollision() {
    ObjectKCL::createCollision();

    const char *name = getKclName();
    auto *resMgr = System::ResourceManager::Instance();
    char filepath[128];

    snprintf(filepath, sizeof(filepath), "%s2.kcl", name);
    m_midColMgr =
            EGG::egg_new<ObjColMgr>(resMgr->getFile(filepath, nullptr, System::ArchiveId::Course));

    snprintf(filepath, sizeof(filepath), "%s3.kcl", name);
    m_flatColMgr =
            EGG::egg_new<ObjColMgr>(resMgr->getFile(filepath, nullptr, System::ArchiveId::Course));

    m_raisedColMgr = m_objColMgr;
}

/// @addr{0x8080ACD8}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectTownBridge::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x8080A8D0}
/// @copybrief ObjectKCL::colRadiusAdditionalLength()
/// @return The additional length to be added to the drawbridge's collision radius, `500.0f`.
f32 ObjectTownBridge::colRadiusAdditionalLength() const {
    return 500.0f;
}

/// @addr{0x80809CDC}
/// @brief Calculates the current angle of the bridge based on the current frame and state
/// @param t The number of frames elapsed in the race
/// @return The current angle of the bridge (in degrees)
/// @details Mods `t` to lie in the range [`0`, @ref m_fullAnimFrames) and passes this
/// into @ref calcState() to get the current state of the drawbridge. If the drawbridge is raised,
/// then simply returns @ref m_raisedAngle with the sign specified by @ref m_rotateUpwards, or if
/// the drawbridge is fully lowered, then returns `0.0f`. If the drawbridge is raising upwards, then
/// the angle is computed as a fraction of @ref m_raisedAngle defined by the wrapped frame divided
/// by @ref m_pivotFrames. If the drawbridge is lowering, then the angle is computed as a fraction
/// of @ref m_raisedAngle based on how long the drawbridge has been lowering for compared to the
/// total @ref m_pivotFrames lowering duration.
f32 ObjectTownBridge::calcBridgeAngle(u32 t) const {
    u32 animFrame = t % m_fullAnimFrames;
    State state = calcState(animFrame);

    switch (state) {
    case State::Raised: {
        s32 sign = m_rotateUpwards ? -1 : 1;
        return m_raisedAngle * static_cast<f32>(sign);
    } break;
    case State::Lowered: {
        return 0.0f;
    } break;
    case State::Raising: {
        s32 sign = m_rotateUpwards ? -1 : 1;
        return m_raisedAngle * static_cast<f32>((static_cast<s32>(animFrame) * sign)) /
                static_cast<f32>(m_pivotFrames);
    } break;
    case State::Lowering: {
        s32 sign = m_rotateUpwards ? -1 : 1;
        u32 rot = m_pivotFrames - (animFrame - (m_pivotFrames + m_raisedFrames));
        return m_raisedAngle * static_cast<f32>(sign * static_cast<s32>(rot)) /
                static_cast<f32>(m_pivotFrames);
    } break;
    default:
        return 0.0f;
    }
}

/// @addr{0x80809FC4}
/// @brief Helper function which determines the current state of the bridge based on `t`
/// @param t The current frame within the drawbridge's animation cycle, in the range [`0`, @ref
/// m_fullAnimFrames)
/// @return The current state of the drawbridge
/// @details If `t` is less than @ref m_pivotFrames, then returns @ref State::Raising. If `t` is
/// less than the sum of @ref m_pivotFrames and @ref m_raisedFrames, then returns @ref
/// State::Raised. If `t` is less than the sum of @ref m_raisedFrames and two times @ref
/// m_pivotFrames, then returns @ref State::Lowering. Otherwise, returns @ref State::Lowered.
ObjectTownBridge::State ObjectTownBridge::calcState(u32 t) const {
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

} // namespace Kinoko::Field
