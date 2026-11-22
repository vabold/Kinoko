#include "ObjectPenguin.hh"

#include "game/field/RailInterpolator.hh"

#include "game/render/AnmMgr.hh"
#include "game/render/DrawMdl.hh"

namespace Kinoko::Field {

/// @addr{0x80775624} @addr{0x8077708C}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
ObjectPenguin::ObjectPenguin(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

/// @addr{0x80775670} @addr{0x807774A4}
/// @brief Default virtual destructor
ObjectPenguin::~ObjectPenguin() = default;

/// @addr{0x807756B0}
/// @copybrief ObjectBase::init()
/// @details Initializes the rail interpolator to the start of the rail, sets the penguin's
/// state to @ref State::Walk, and sets the penguin's orientation basis to the default forward
/// direction.
void ObjectPenguin::init() {
    m_railInterpolator->init(0.0f, 0);
    m_state = State::Walk;
    m_basis = EGG::Vector3f::ez;
}

/// @addr{0x80775764}
/// @copybrief ObjectBase::calc()
/// @details If the penguin is in the walking state, it calls @ref calcWalk() to update its
/// position and orientation along the rail. Otherwise, does nothing.
void ObjectPenguin::calc() {
    if (m_state == State::Walk) {
        calcWalk();
    }
}

/// @addr{0x80777324}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectPenguin::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x8077588C}
/// @brief Runs when the penguin enters the walking state
/// @details Sets the penguin's state to @ref State::Walk, updates the transformation matrix,
/// and sets the rotation without triggering a transform update.
void ObjectPenguin::enterWalk() {
    m_state = State::Walk;
    calcTransform();
    setRotNoFlag(transform().base(2));
}

/// @addr{0x807757A0}
/// @brief Runs every frame that the penguin is walking
/// @details Updates the rail interpolator. Calls @ref calcPos() and @ref calcRot() to update
/// the penguin's position and orientation along the rail.
void ObjectPenguin::calcWalk() {
    m_railInterpolator->calc();
    calcPos();
    calcRot();
}

/// @addr{0x80775B1C}
/// @brief Updates the penguin's orientation based on the rail interpolator's tangent direction
/// @details Linearly interpolates the penguin's current orientation towards the tangent
/// direction of the rail and updates the transformation matrix accordingly.
void ObjectPenguin::calcRot() {
    constexpr f32 INTERP_RATE = 0.2f;

    m_basis = m_basis.interpolate(INTERP_RATE, m_railInterpolator->curTangentDir());
    m_basis.normalise();
    setMatrixFromOrthonormalBasisAndPos(m_basis);
}

/// @addr{0x80775C2C}
/// @brief Sets the penguin's position to the rail interpolator's position
void ObjectPenguin::calcPos() {
    setPos(m_railInterpolator->curPos());
}

/// @addr{0x80775E60}
/// @copydoc ObjectPenguin::ObjectPenguin(const System::MapdataGeoObj &)
ObjectPenguinS::ObjectPenguinS(const System::MapdataGeoObj &params) : ObjectPenguin(params) {}

/// @addr{0x80776070}
/// @brief Default virtual destructor
ObjectPenguinS::~ObjectPenguinS() = default;

/// @addr{0x807760B0}
/// @copybrief ObjectBase::init()
/// @details Initializes @ref m_anmTimer to zero, the rail interpolator to the beginning of the
/// rail, @ref m_basis to the rail's tangent direction, and the penguin's state to @ref
/// State::Walk.
void ObjectPenguinS::init() {
    initAnmTimer();
    m_railInterpolator->init(0.0f, 0);
    m_basis = m_railInterpolator->curTangentDir();
    m_state = State::Walk;
}

/// @addr{0x80776198}
/// @copybrief ObjectBase::calc()
/// @details Calls the associated calculation function based on the penguin's current state.
void ObjectPenguinS::calc() {
    switch (m_state) {
    case State::Walk:
        calcWalk();
        break;
    case State::Dive:
        calcDive();
        break;
    case State::Slide:
        calcSlide();
        break;
    case State::SlideSlow:
        calcWalk();
        calcTransform();
        break;
    case State::StandUp:
        calcStandUp();
        break;
    }
}

/// @addr{0x8077739C}
/// @copybrief ObjectBase::loadAnims()
/// @details Loads the `walk`, `dive`, `slider`, and `stand_up` animations for the penguin.
void ObjectPenguinS::loadAnims() {
    std::array<const char *, 4> names = {{
            "walk",
            "dive",
            "slider",
            "stand_up",
    }};

    std::array<Render::AnmType, 4> types = {{
            Render::AnmType::Chr,
            Render::AnmType::Chr,
            Render::AnmType::Chr,
            Render::AnmType::Chr,
    }};

    linkAnims(names, types);
}

/// @addr{0x80776AF0}
/// @brief Runs when the penguin enters the walking state
/// @details Sets the penguin's state to @ref State::Walk.
void ObjectPenguinS::enterWalk() {
    m_state = State::Walk;
}

/// @addr{0x8077637C}
/// @brief Runs every frame the penguin is in the walking state
/// @details Updates the penguin's rail, checking to see if it should change state. Updates the
/// penguin's position and orientation along the rail accordingly.
void ObjectPenguinS::calcWalk() {
    calcRail();
    calcPos();
    calcRot();
}

/// @addr{0x80776188}
/// @brief Initializes the animation timer to zero
void ObjectPenguinS::initAnmTimer() {
    m_anmTimer = 0;
}

/// @addr{0x807768A0}
/// @brief Every frame, updates the penguin's position along the rail, its state, and current
/// animation timer
/// @details Updates the rail interpolator. If the penguin is still in the middle of a rail
/// segment, early returns. Otherwise, fetches the first setting of the new rail node. If it's
/// set to 0 and the penguin is in the slow slide state, transitions to the stand up state. If
/// the setting is set to 1 and the penguin is in the walking state, transitions to the slide
/// state. If the setting is set to 2 and the penguin is in the walking state, transitions to
/// the dive state. Finally, updates the rail interpolator's speed based on the new rail node's
/// second setting.
void ObjectPenguinS::calcRail() {
    if (m_railInterpolator->calc() != RailInterpolator::Status::SegmentEnd) {
        return;
    }

    const auto &curPoint = m_railInterpolator->curPoint();
    u16 setting = curPoint.setting[0];

    if (setting == 0 && m_state == State::SlideSlow) {
        enterStandUp();
    } else if (setting == 1 && m_state == State::Walk) {
        enterSlide();
    } else if (setting == 2 && m_state == State::Walk) {
        enterDive();
    }

    m_railInterpolator->setCurrVel(static_cast<f32>(curPoint.setting[1]));
}

/// @addr{0x80776B7C}
/// @brief Transitions the penguin into the dive state
/// @details Sets @ref m_state to @ref State::Dive.
void ObjectPenguinS::enterDive() {
    m_state = State::Dive;
}

/// @addr{0x80776B94}
/// @brief Transitions the penguin into the slide state
/// @details Sets @ref m_state to @ref State::Slide and resets the animation timer based on the
/// `dive` animation's duration.
void ObjectPenguinS::enterSlide() {
    auto *anmMgr = m_drawMdl->anmMgr();
    anmMgr->playAnim(0.0f, 1.0f, 1);
    m_state = State::Slide;
    m_anmTimer = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount();
}

/// @addr{0x80776C58}
/// @brief Transitions the penguin into the slow slide state
/// @details Sets @ref m_state to @ref State::SlideSlow.
void ObjectPenguinS::enterSlideSlow() {
    m_state = State::SlideSlow;
}

/// @addr{0x80776CDC}
/// @brief Transitions the penguin into the stand up state
/// @details Sets @ref m_state to @ref State::StandUp and resets the animation timer based on
/// the `stand_up` animation's duration.
void ObjectPenguinS::enterStandUp() {
    auto *anmMgr = m_drawMdl->anmMgr();
    anmMgr->playAnim(0.0f, 1.0f, 3);
    m_state = State::StandUp;
    m_anmTimer = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount();
}

/// @addr{0x807763D0}
/// @brief Runs every frame the penguin is diving mid-air onto its belly
/// @details Simply calls @ref calcWalk() to update the rail and the penguin's position and
/// orientation along it.
void ObjectPenguinS::calcDive() {
    calcWalk();
}

/// @addr{0x80776498}
/// @brief Runs every frame the penguin is sliding on its belly full speed
/// @details Calls @ref calcWalk() to update the rail and the penguin's position and orientation
/// along it. Transitions to the slow slide state when @ref m_anmTimer reaches zero.
void ObjectPenguinS::calcSlide() {
    calcWalk();

    if (--m_anmTimer == 0) {
        enterSlideSlow();
    }
}

/// @addr{0x80776670}
/// @brief Runs every frame the penguin is standing up from a slide
/// @details Calls @ref calcWalk() to update the rail and the penguin's position and orientation
/// along it. Transitions to the walking state when @ref m_anmTimer reaches zero.
void ObjectPenguinS::calcStandUp() {
    calcWalk();

    if (--m_anmTimer == 0) {
        enterWalk();
    }
}

} // namespace Kinoko::Field
