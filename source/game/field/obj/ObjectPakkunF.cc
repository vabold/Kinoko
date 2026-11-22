#include "ObjectPakkunF.hh"

#include "game/field/ObjectCollisionBase.hh"

#include "game/render/AnmMgr.hh"
#include "game/render/DrawMdl.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x807743A4}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes @ref m_attackFrames and @ref m_currAttackFrame to zero. Sets @ref
/// m_waitDuration based on param setting 1.
/// @note @ref m_waitDuration is normally set in @ref init(), but since it is conditionally set
/// based on whether we are in time trial mode, we can move it to the constructor for Kinoko.
/// This also allows us to mark the member as `const`. As for @ref m_attackFrames and @ref
/// m_currAttackFrame, these are not initialized in the constructor in the base game; rather,
/// they are zeroed out as a result of the fact that heap memory is zeroed on console.
ObjectPakkunF::ObjectPakkunF(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      m_attackFrames(0),
      m_currAttackFrame(0),
      m_waitDuration(static_cast<s32>(params.setting(0))) {}

/// @addr{0x807754FC}
/// @brief Default virtual destructor
ObjectPakkunF::~ObjectPakkunF() = default;

/// @addr{0x807743E4}
/// @copybrief ObjectBase::init()
/// @details Initializes @ref m_state to @ref State::Wait and sets @ref m_waitFrames to @ref
/// m_waitDuration. Finally, updates the piranha's transformation matrix.
/// @note @ref m_waitDuration is normally initialized in this function, but since it is
/// conditionally set based on whether we are in time trial mode, we can move it to the
/// constructor for Kinoko. This also allows us to mark the member as `const`.
void ObjectPakkunF::init() {
    m_state = State::Wait;
    m_waitFrames = m_waitDuration;

    calcTransform();
}

/// @addr{0x80774754}
/// @copybrief ObjectBase::calc()
/// @details If the piranha is idle, calls @ref calcWait(). If the piranha is attacking, calls
/// @ref calcAttack().
/// @note The base game also calls @ref calcCollisionTransform() every frame. Kinoko skips this,
/// since @ref ObjectDirector::checkKartObjectCollision() always calls it before checking
/// collision, and nothing else reads the collision transform.
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
}

/// @addr{0x80775464}
/// @copybrief ObjectBase::loadAnims()
/// @details Loads the `attack`, `damage`, and `wait` animations for the piranha.
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
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details The piranha's collision is offset from its position, up by `620.0f` units and forward
/// by `70.0f` units, multiplied by the X-component of the piranha's @ref scale(). When attacking,
/// for the first 10 frames, it remains at that initial offset. For the next 10 frames of an attack,
/// it linearly interpolates from the initial position to a "final" position. For the next 10
/// frames, it linearly interpolates back to the initial position where it remains until the cycle
/// is reset. Updates the collision manager's transformation matrix accordingly.
void ObjectPakkunF::calcCollisionTransform() {
    constexpr EGG::Vector3f INIT_POS = EGG::Vector3f(0.0f, 620.0f, 70.0f);
    constexpr EGG::Vector3f FINAL_POS = EGG::Vector3f(0.0f, 160.0f, 550.0f);

    if (!m_collision) {
        return;
    }

    EGG::Vector3f posOffset;

    if (m_state == State::Attack) {
        if (m_currAttackFrame <= 10) {
            posOffset = INIT_POS * scale().x;
        } else if (m_currAttackFrame <= 20) {
            f32 fVar1 = 0.1f * static_cast<f32>(m_currAttackFrame - 10);
            posOffset = FINAL_POS * scale().x * fVar1 + INIT_POS * scale().x * (1.0f - fVar1);
        } else if (m_currAttackFrame <= 30) {
            f32 fVar1 = 0.1f * static_cast<f32>(m_currAttackFrame - 20);
            posOffset = INIT_POS * scale().x * fVar1 + FINAL_POS * scale().x * (1.0f - fVar1);
        } else {
            posOffset = INIT_POS * scale().x;
        }
    } else {
        posOffset = INIT_POS * scale().x;
    }

    EGG::Matrix34f rotMat;
    rotMat.makeR(rot());

    // Probably always evaluates to the identity matrix in time trials
    EGG::Matrix34f damageRotMat = EGG::Matrix34f::ident;
    damageRotMat.setAxisRotation(0.0f, EGG::Vector3f::ey);

    EGG::Matrix34f transformMat;
    transformMat.makeT(pos() + rotMat.multiplyTo(damageRotMat).ps_multVector(posOffset));

    m_collision->transform(transformMat, scale());
}

/// @addr{0x807754F4}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectPakkunF::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x80775458}
/// @copybrief ObjectBase::getCollisionRadius()
/// @return The collision radius of the piranha, `1000.0f`.
/// @details Extends the collision radius to ensure GJK collision checks occur from far enough
/// away when the piranha chomps forward.
f32 ObjectPakkunF::getCollisionRadius() const {
    return 1000.0f;
}

/// @addr{0x80774C80}
/// @brief Transitions the piranha from the attack state back to the wait state
/// @details Sets the piranha's state to @ref State::Wait and resets @ref m_waitFrames to
/// @ref m_waitDuration.
void ObjectPakkunF::enterWait() {
    m_state = State::Wait;
    m_waitFrames = m_waitDuration;
}

/// @addr{0x80774CB0}
/// @brief Runs once when the piranha is about to attack
/// @details Sets the piranha's state to @ref State::Attack. Resets @ref m_currAttackFrame to
/// zero, and sets @ref m_attackFrames to the animation duration plus a `60` frame lingering
/// period.
void ObjectPakkunF::enterAttack() {
    constexpr s32 LINGERING_FRAMES = 60; // Additional frames before switching back to idle

    m_state = State::Attack;
    auto *anmMgr = m_drawMdl->anmMgr();
    anmMgr->playAnim(0.0f, 1.0f, 0);
    m_currAttackFrame = 0;
    auto *attackAnm = anmMgr->activeAnim(Render::AnmType::Chr);
    m_attackFrames = static_cast<s32>(attackAnm->frameCount()) + LINGERING_FRAMES;
}

/// @addr{0x80774A00}
/// @brief Runs every frame when the piranha is idle
/// @details When the wait timer reaches 0, the piranha will enter the attack state.
void ObjectPakkunF::calcWait() {
    if (--m_waitFrames == 0) {
        enterAttack();
    }
}

/// @addr{0x80774A84}
/// @brief Runs every frame while the piranha is attacking
/// @details Increments @ref m_currAttackFrame and calls @ref calcAttackTimer() to see if the
/// piranha should stop attacking.
void ObjectPakkunF::calcAttack() {
    ++m_currAttackFrame;
    calcAttackTimer();
}

/// @addr{0x80774C1C}
/// @brief Checks if the piranha should stop attacking and transition back to the wait state
/// @details Decrements @ref m_attackFrames. If it reaches zero, calls @ref enterWait() to
/// transition the piranha back to the wait state.
void ObjectPakkunF::calcAttackTimer() {
    if (--m_attackFrames == 0) {
        enterWait();
    }
}

} // namespace Kinoko::Field
