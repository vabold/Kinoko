#include "ObjectPakkunF.hh"

namespace Field {

/// @addr{0x807743A4}
ObjectPakkunF::ObjectPakkunF(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_attackFrames(0), m_currAttackFrame(0),
      m_waitDuration(static_cast<s32>(params.setting(0))) {}

/// @addr{0x807754FC}
ObjectPakkunF::~ObjectPakkunF() = default;

/// @addr{0x807743E4}
void ObjectPakkunF::init() {
    m_state = State::Wait;
    m_waitFrames = m_waitDuration;

    calcTransform();
}

/// @addr{0x80774754}
void ObjectPakkunF::calc() {
    switch (m_state) {
    case State::Wait:
        calcWait();
        break;
    case State::Attack:
        calcAttack();
        break;
    default:
        break;
    }

    calcCollisionTransform();
}

/// @addr{0x80775464}
void ObjectPakkunF::loadAnims() {
    std::array<const char *, 3> names = {{
            "attack",
            "damage",
            "wait",
    }};

    std::array<Render::AnmType, 3> types = {{
            Render::AnmType::Chr,
            Render::AnmType::Chr,
            Render::AnmType::Chr,
    }};

    linkAnims(names, types);
}

/// @addr{0x80775108}
void ObjectPakkunF::calcCollisionTransform() {
    constexpr EGG::Vector3f INIT_POS = EGG::Vector3f(0.0f, 620.0f, 70.0f);
    constexpr EGG::Vector3f FINAL_POS = EGG::Vector3f(0.0f, 160.0f, 550.0f);

    if (!m_collision) {
        return;
    }

    EGG::Vector3f posOffset;

    if (m_state == State::Attack) {
        if (m_currAttackFrame <= 10) {
            posOffset = INIT_POS * m_scale.x;
        } else if (m_currAttackFrame <= 20) {
            f32 fVar1 = 0.1f * static_cast<f32>(m_currAttackFrame - 10);
            posOffset = FINAL_POS * m_scale.x * fVar1 + INIT_POS * m_scale.x * (1.0f - fVar1);
        } else if (m_currAttackFrame <= 30) {
            f32 fVar1 = 0.1f * static_cast<f32>(m_currAttackFrame - 20);
            posOffset = INIT_POS * m_scale.x * fVar1 + FINAL_POS * m_scale.x * (1.0f - fVar1);
        } else {
            posOffset = INIT_POS * m_scale.x;
        }
    } else {
        posOffset = INIT_POS * m_scale.x;
    }

    EGG::Matrix34f rotMat;
    rotMat.makeR(m_rot);

    // Probably always evaluates to the identity matrix in time trials
    EGG::Matrix34f damageRotMat = EGG::Matrix34f::ident;
    damageRotMat.setAxisRotation(0.0f, EGG::Vector3f::ey);

    EGG::Matrix34f transformMat;
    transformMat.makeT(m_pos + rotMat.multiplyTo(damageRotMat).ps_multVector(posOffset));

    m_collision->transform(transformMat, m_scale);
}

/// @addr{0x80774A00}
void ObjectPakkunF::calcWait() {
    if (--m_waitFrames == 0) {
        enterAttack();
    }
}

/// @addr{0x80774A84}
void ObjectPakkunF::calcAttack() {
    ++m_currAttackFrame;

    if (--m_attackFrames == 0) {
        m_state = State::Wait;
        m_waitFrames = m_waitDuration;
    }
}

/// @addr{0x80774CB0}
void ObjectPakkunF::enterAttack() {
    constexpr s32 LINGERING_FRAMES = 60; ///< Additional frames before switching back to idle state

    m_state = State::Attack;
    auto *anmMgr = m_drawMdl->anmMgr();
    anmMgr->playAnim(0.0f, 1.0f, 0);
    m_currAttackFrame = 0;
    auto *attackAnm = anmMgr->activeAnim(Render::AnmType::Chr);
    m_attackFrames = static_cast<s32>(attackAnm->frameCount()) + LINGERING_FRAMES;
}

} // namespace Field
