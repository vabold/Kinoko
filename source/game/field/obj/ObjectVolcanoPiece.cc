#include "game/field/obj/ObjectVolcanoPiece.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x80817DE8}
ObjectVolcanoPiece::ObjectVolcanoPiece(const System::MapdataGeoObj &params)
    : ObjectKCL(params), m_initialPos(m_pos), m_initialRot(m_rot),
      m_restDuration(static_cast<s32>(params.setting(1)) * 60),
      m_shakeDuration(m_restDuration + static_cast<s32>(params.setting(2)) * 60),
      m_quakeDuration(m_shakeDuration + static_cast<s32>(params.setting(7)) + 1),
      m_colMgrB(nullptr), m_colMgrC(nullptr) {
    snprintf(m_modelName, sizeof(m_modelName), "VolcanoPiece%hd", params.setting(0));
}

/// @addr{0x80803DA8}
ObjectVolcanoPiece::~ObjectVolcanoPiece() {
    delete m_colMgrB;
    delete m_colMgrC;
}

/// @addr{0x80819400}
void ObjectVolcanoPiece::calc() {
    s32 timer = static_cast<s32>(static_cast<f32>(System::RaceManager::Instance()->timer()));
    if (calcState(timer) == State::Fall && FALL_DURATION - 1 == calcT(timer)) {
        update(0);
    }

    m_flags.setBit(eFlags::Matrix);
    EGG::Vector3f movingObjVel;
    m_transform = calcShakeAndFall(&movingObjVel, 0);
    m_pos = m_transform.base(3);
    setMovingObjVel(movingObjVel);
}

/// @addr{0x80817F6C}
void ObjectVolcanoPiece::createCollision() {
    ObjectKCL::createCollision();

    char filepath[128];
    snprintf(filepath, sizeof(filepath), "%sb.kcl", getKclName());
    auto *file = System::ResourceManager::Instance()->getFile(filepath, nullptr,
            System::ArchiveId::Course);

    if (file) {
        m_colMgrB = new ObjColMgr(file);
    }

    snprintf(filepath, sizeof(filepath), "%sc.kcl", getKclName());
    file = System::ResourceManager::Instance()->getFile(filepath, nullptr,
            System::ArchiveId::Course);

    if (file) {
        m_colMgrC = new ObjColMgr(file);
    }

    EGG::Matrix34f rtMat;
    EGG::Matrix34f invMat;
    rtMat.makeRT(m_initialRot, m_initialPos);
    rtMat.ps_inverse(invMat);

    m_objColMgr->setMtx(rtMat);
    m_objColMgr->setInvMtx(invMat);
    m_objColMgr->setScale(m_scale.y);

    if (m_colMgrB) {
        m_colMgrB->setMtx(rtMat);
        m_colMgrB->setInvMtx(invMat);
        m_colMgrB->setScale(m_scale.y);
    }

    if (m_colMgrC) {
        m_colMgrC->setMtx(rtMat);
        m_colMgrC->setInvMtx(invMat);
        m_colMgrC->setScale(m_scale.y);
    }
}

template <typename T, typename U>
    requires(std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>) &&
        (std::is_same_v<U, ObjectVolcanoPiece::CheckPointPartialFunc> ||
                std::is_same_v<U, ObjectVolcanoPiece::CheckPointFullFunc>)
bool ObjectVolcanoPiece::checkPointImpl(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, T *pInfo, KCLTypeMask *pFlagsOut, U checkFunc) {
    State state = calcState(System::RaceManager::Instance()->timer());
    bool hasCol = (m_objColMgr->*checkFunc)(v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || hasCol) {
        return hasCol;
    }

    hasCol = m_colMgrB && (m_colMgrB->*checkFunc)(v0, v1, flags, pInfo, pFlagsOut);
    hasCol = hasCol || (m_colMgrC && (m_colMgrC->*checkFunc)(v0, v1, flags, pInfo, pFlagsOut));

    return hasCol;
}

template <typename T, typename U>
    requires(std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>) &&
        (std::is_same_v<U, ObjectVolcanoPiece::CheckSpherePartialFunc> ||
                std::is_same_v<U, ObjectVolcanoPiece::CheckSphereFullFunc>)
bool ObjectVolcanoPiece::checkSphereImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, T *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset, U checkFunc) {
    update(timeOffset);
    State state = calcState(System::RaceManager::Instance()->timer());
    bool hasCol = (m_objColMgr->*checkFunc)(radius, v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || hasCol) {
        return hasCol;
    }

    hasCol = m_colMgrB && (m_colMgrB->*checkFunc)(radius, v0, v1, flags, pInfo, pFlagsOut);
    hasCol = hasCol ||
            (m_colMgrC && (m_colMgrC->*checkFunc)(radius, v0, v1, flags, pInfo, pFlagsOut));

    return hasCol;
}

bool ObjectVolcanoPiece::checkCollisionImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset, CheckSphereFullFunc checkFunc) {
    u32 t = System::RaceManager::Instance()->timer() - timeOffset;
    State state = calcState(t);
    update(timeOffset);

    if (state == State::Rest) {
        return (m_objColMgr->*checkFunc)(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    if (state == State::Gone) {
        return m_colMgrC && (m_colMgrC->*checkFunc)(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    bool hasCol = (m_objColMgr->*checkFunc)(radius, v0, v1, flags, pInfo, pFlagsOut);
    hasCol = hasCol ||
            (m_colMgrB && (m_colMgrB->*checkFunc)(radius, v0, v1, flags, pInfo, pFlagsOut));
    hasCol = hasCol ||
            (m_colMgrC && (m_colMgrC->*checkFunc)(radius, v0, v1, flags, pInfo, pFlagsOut));

    return hasCol;
}

/// @addr{0x808044C0}
void ObjectVolcanoPiece::narrScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
        u32 /*timeOffset*/) {
    State state = calcState(System::RaceManager::Instance()->timer());
    m_objColMgr->narrScLocal(radius, pos, mask);

    if (state == State::Rest) {
        return;
    }

    if (m_colMgrB) {
        m_colMgrB->narrScLocal(radius, pos, mask);
    }

    if (m_colMgrC) {
        m_colMgrC->narrScLocal(radius, pos, mask);
    }
}

/// @addr{0x80818334}
void ObjectVolcanoPiece::update(u32 timeOffset) {
    State state = calcState(System::RaceManager::Instance()->timer() - timeOffset);
    if (state == State::Rest || state == State::Gone) {
        return;
    }

    EGG::Vector3f movingObjVel;
    const EGG::Matrix34f &rtMat = calcShakeAndFall(&movingObjVel, timeOffset);
    EGG::Matrix34f invMat;
    rtMat.ps_inverse(invMat);

    m_objColMgr->setMtx(rtMat);
    m_objColMgr->setInvMtx(invMat);

    if (m_colMgrB) {
        m_colMgrB->setMtx(rtMat);
        m_colMgrB->setInvMtx(invMat);
    }

    setMovingObjVel(movingObjVel);
}

/// @addr{0x80818674}
void ObjectVolcanoPiece::calcScale(u32 timeOffset) {
    State state = calcState(static_cast<s32>(
            static_cast<f32>(System::RaceManager::Instance()->timer() - timeOffset)));

    if (state == State::Rest || state == State::Gone) {
        return;
    }

    f32 scale = getScaleY(timeOffset);
    m_objColMgr->setScale(scale);

    if (m_colMgrB) {
        m_colMgrB->setScale(scale);
    }
}

/// @addr{0x80805924}
void ObjectVolcanoPiece::setMovingObjVel(const EGG::Vector3f &v) {
    m_objColMgr->setMovingObjVel(v);

    if (m_colMgrB) {
        m_colMgrB->setMovingObjVel(v);
    }
}

/// @addr{0x808187B4}
const EGG::Matrix34f &ObjectVolcanoPiece::calcShakeAndFall(EGG::Vector3f *vel, u32 timeOffset) {
    constexpr f32 FALL_SPEED = 10.0f;
    constexpr s32 SHAKE_STEPS = 8;
    constexpr s32 SHAKE_STEPS_HALF = SHAKE_STEPS / 2;
    constexpr f32 SHAKE_STEP_AMPLITUDE = 5.0f;

    u32 t = System::RaceManager::Instance()->timer() - timeOffset;
    State state = calcState(t);

    EGG::Vector3f pos = m_initialPos;

    switch (state) {
    case State::Shake: {
        s32 step = static_cast<s32>(calcT(t)) % SHAKE_STEPS + 1;

        if (step > SHAKE_STEPS_HALF) {
            step = SHAKE_STEPS_HALF - step;
        }
        pos.y += SHAKE_STEP_AMPLITUDE * static_cast<f32>(step);
    } break;
    case State::Fall:
        pos.y -= FALL_SPEED * static_cast<f32>(calcT(t));
        break;
    case State::Gone:
        pos.y -= FALL_SPEED * static_cast<f32>(FALL_DURATION);
        break;
    default:
        break;
    }

    if (vel) {
        EGG::Vector3f prevPos = m_initialPos;
        State prevState = calcState(t - 1);

        switch (prevState) {
        case State::Shake: {
            s32 step = static_cast<s32>(calcT(t - 1)) % SHAKE_STEPS + 1;

            if (step > SHAKE_STEPS_HALF) {
                step = SHAKE_STEPS_HALF - step;
            }
            prevPos.y += SHAKE_STEP_AMPLITUDE * static_cast<f32>(step);
        } break;
        case State::Fall:
            prevPos.y -= FALL_SPEED * static_cast<f32>(calcT(t - 1));
            break;
        case State::Gone:
            prevPos.y -= FALL_SPEED * static_cast<f32>(FALL_DURATION);
            break;
        default:
            break;
        }

        *vel = EGG::Vector3f(0.0f, pos.y - prevPos.y, 0.0f);
    }

    m_rtMat.makeRT(m_rot, pos);
    return m_rtMat;
}

/// @addr{0x80818FCC}
ObjectVolcanoPiece::State ObjectVolcanoPiece::calcState(s32 frame) const {
    if (frame < m_restDuration) {
        return State::Rest;
    }

    if (frame < m_shakeDuration) {
        return State::Shake;
    }

    if (frame < m_quakeDuration) {
        return State::Quake;
    }

    if (frame < m_quakeDuration + FALL_DURATION) {
        return State::Fall;
    }

    return State::Gone;
}

/// @addr{0x80819028}
f32 ObjectVolcanoPiece::calcT(s32 frame) const {
    if (frame < m_restDuration) {
        return static_cast<f32>(frame);
    }

    if (frame < m_shakeDuration) {
        return static_cast<f32>(frame - m_restDuration);
    }

    if (frame < m_quakeDuration) {
        return static_cast<f32>(frame - m_shakeDuration);
    }

    if (frame < m_quakeDuration + FALL_DURATION) {
        return static_cast<f32>(frame - m_quakeDuration);
    }

    return static_cast<f32>(frame - m_quakeDuration - FALL_DURATION);
}

} // namespace Field
