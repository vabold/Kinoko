#include "game/field/obj/ObjectVolcanoPiece.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x80817DE8}
ObjectVolcanoPiece::ObjectVolcanoPiece(const System::MapdataGeoObj &params)
    : ObjectKCL(params), m_initialPos(m_pos), m_initialRot(m_rot),
      m_restDuration(params.setting(1) * 60),
      m_shakeDuration(m_restDuration + params.setting(2) * 60),
      m_quakeDuration(m_shakeDuration + params.setting(7) + 1), m_colMgrB(nullptr),
      m_colMgrC(nullptr) {
    snprintf(m_modelName, sizeof(m_modelName), "VolcanoPiece%hd",
            static_cast<s16>(params.setting(0)));
}

/// @addr{0x80803DA8}
ObjectVolcanoPiece::~ObjectVolcanoPiece() {
    delete m_colMgrB;
    delete m_colMgrC;
}

/// @addr{0x80819400}
void ObjectVolcanoPiece::calc() {
    u32 timer = System::RaceManager::Instance()->timer();
    if (calcState(timer) == State::Fall) {
        f32 t = calcT(timer);

        if (FALL_DURATION - 1 == t) {
            update(0);
        }
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

/// @addr{0x80805404}
bool ObjectVolcanoPiece::checkPointPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding = m_objColMgr->checkPointPartial(v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding = m_colMgrB->checkPointPartial(v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding = m_colMgrC->checkPointPartial(v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x8080554C}
bool ObjectVolcanoPiece::checkPointPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding = m_objColMgr->checkPointPartialPush(v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding = m_colMgrB->checkPointPartialPush(v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding = m_colMgrC->checkPointPartialPush(v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x80805694}
bool ObjectVolcanoPiece::checkPointFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding = m_objColMgr->checkPointFull(v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding = m_colMgrB->checkPointFull(v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding = m_colMgrC->checkPointFull(v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x808057DC}
bool ObjectVolcanoPiece::checkPointFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding = m_objColMgr->checkPointFullPush(v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding = m_colMgrB->checkPointFullPush(v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding = m_colMgrC->checkPointFullPush(v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x80804F68}
bool ObjectVolcanoPiece::checkSpherePartial(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    update(timeOffset);
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding = m_objColMgr->checkSpherePartial(radius, v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding = m_colMgrB->checkSpherePartial(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding = m_colMgrC->checkSpherePartial(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x808050EC}
bool ObjectVolcanoPiece::checkSpherePartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    update(timeOffset);
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding = m_objColMgr->checkSpherePartialPush(radius, v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding = m_colMgrB->checkSpherePartialPush(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding = m_colMgrC->checkSpherePartialPush(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x80805270}
bool ObjectVolcanoPiece::checkSphereFull(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    update(timeOffset);
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding = m_objColMgr->checkSphereFull(radius, v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding = m_colMgrB->checkSphereFull(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding = m_colMgrC->checkSphereFull(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x808053F4}
bool ObjectVolcanoPiece::checkSphereFullPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkCollision(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
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

/// @addr{0x80804A48}
bool ObjectVolcanoPiece::checkPointCachedPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding = m_objColMgr->checkPointCachedPartial(v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding = m_colMgrB->checkPointCachedPartial(v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding = m_colMgrC->checkPointCachedPartial(v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x80804B90}
bool ObjectVolcanoPiece::checkPointCachedPartialPush(const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut) {
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding = m_objColMgr->checkPointCachedPartialPush(v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding = m_colMgrB->checkPointCachedPartialPush(v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding = m_colMgrC->checkPointCachedPartialPush(v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x80804CD8}
bool ObjectVolcanoPiece::checkPointCachedFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding = m_objColMgr->checkPointCachedFull(v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding = m_colMgrB->checkPointCachedFull(v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding = m_colMgrC->checkPointCachedFull(v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x80804E20}
bool ObjectVolcanoPiece::checkPointCachedFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding = m_objColMgr->checkPointCachedFullPush(v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding = m_colMgrB->checkPointCachedFullPush(v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding = m_colMgrC->checkPointCachedFullPush(v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x808045AC}
bool ObjectVolcanoPiece::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    update(timeOffset);
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding = m_objColMgr->checkSphereCachedPartial(radius, v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding = m_colMgrB->checkSphereCachedPartial(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding = m_colMgrC->checkSphereCachedPartial(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x80804730}
bool ObjectVolcanoPiece::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    update(timeOffset);
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding =
            m_objColMgr->checkSphereCachedPartialPush(radius, v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding =
                m_colMgrB->checkSphereCachedPartialPush(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding =
                m_colMgrC->checkSphereCachedPartialPush(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x808048B4}
bool ObjectVolcanoPiece::checkSphereCachedFull(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    update(timeOffset);
    State state = calcState(System::RaceManager::Instance()->timer());
    bool colliding = m_objColMgr->checkSphereCachedFull(radius, v0, v1, flags, pInfo, pFlagsOut);

    if (state == State::Rest || colliding) {
        return colliding;
    }

    if (m_colMgrB) {
        colliding = m_colMgrB->checkSphereCachedFull(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    if (!colliding && m_colMgrC) {
        colliding = m_colMgrC->checkSphereCachedFull(radius, v0, v1, flags, pInfo, pFlagsOut);
    }

    return colliding;
}

/// @addr{0x80804A38}
bool ObjectVolcanoPiece::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkCollisionCached(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
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
    State state = calcState(System::RaceManager::Instance()->timer() - timeOffset);

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

/// @addr{0x8080595C}
const EGG::Matrix34f &ObjectVolcanoPiece::getUpdatedMatrix(u32 timeOffset) {
    return calcShakeAndFall(nullptr, timeOffset);
}

/// @addr{0x808199A8}
bool ObjectVolcanoPiece::checkCollision(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    u32 t = System::RaceManager::Instance()->timer() - timeOffset;
    State state = calcState(t);
    update(timeOffset);

    if (state == State::Rest) {
        return m_objColMgr->checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut);
    }

    if (state == State::Gone) {
        return m_colMgrC &&
                m_colMgrC->checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut);
    }

    bool collided = m_objColMgr->checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut);

    if (!collided && m_colMgrB) {
        collided = m_colMgrB->checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut);
    }

    if (!collided && m_colMgrC) {
        collided = m_colMgrC->checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut);
    }

    return collided;
}

/// @addr{0x80819DA0}
bool ObjectVolcanoPiece::checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    u32 t = System::RaceManager::Instance()->timer() - timeOffset;
    State state = calcState(t);
    update(timeOffset);

    if (state == State::Rest) {
        return m_objColMgr->checkSphereCachedFullPush(radius, pos, prevPos, mask, info, maskOut);
    }

    if (state == State::Gone) {
        return m_colMgrC &&
                m_colMgrC->checkSphereCachedFullPush(radius, pos, prevPos, mask, info, maskOut);
    }

    bool collided =
            m_objColMgr->checkSphereCachedFullPush(radius, pos, prevPos, mask, info, maskOut);

    if (!collided && m_colMgrB) {
        collided = m_colMgrB->checkSphereCachedFullPush(radius, pos, prevPos, mask, info, maskOut);
    }

    if (!collided && m_colMgrC) {
        collided = m_colMgrC->checkSphereCachedFullPush(radius, pos, prevPos, mask, info, maskOut);
    }

    return collided;
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
            prevPos.y += SHAKE_STEP_AMPLITUDE - static_cast<f32>(step);
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
ObjectVolcanoPiece::State ObjectVolcanoPiece::calcState(u32 frame) const {
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
f32 ObjectVolcanoPiece::calcT(u32 frame) const {
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
