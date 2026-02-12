#include "ObjectPenguin.hh"

namespace Field {

/// @addr{0x80775624} @addr{0x8077708C}
ObjectPenguin::ObjectPenguin(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

/// @addr{0x80775670} @addr{0x807774A4}
ObjectPenguin::~ObjectPenguin() = default;

/// @addr{0x807756B0}
void ObjectPenguin::init() {
    m_railInterpolator->init(0.0f, 0);
    m_state = State::Walk;
    m_basis = EGG::Vector3f::ez;
}

/// @addr{0x80775B1C}
void ObjectPenguin::calcRot() {
    constexpr f32 INTERP_RATE = 0.2f;

    m_basis = Interpolate(INTERP_RATE, m_basis, m_railInterpolator->curTangentDir());
    m_basis.normalise();
    setMatrixFromOrthonormalBasisAndPos(m_basis);
}

/// @addr{0x8077588C}
void ObjectPenguin::enterWalk() {
    m_state = State::Walk;
    calcTransform();
    setRotNoFlag(transform().base(2));
}

/// @addr{0x80775E60}
ObjectPenguinS::ObjectPenguinS(const System::MapdataGeoObj &params) : ObjectPenguin(params) {}

/// @addr{0x80776070}
ObjectPenguinS::~ObjectPenguinS() = default;

/// @addr{0x807760B0}
void ObjectPenguinS::init() {
    m_anmTimer = 0;
    m_railInterpolator->init(0.0f, 0);
    m_basis = m_railInterpolator->curTangentDir();
    m_state = State::Walk;
}

/// @addr{0x80776198}
void ObjectPenguinS::calc() {
    switch (m_state) {
    case State::Walk:
        calcWalk();
        break;
    case State::Dive:
        calcDive();
        break;
    case State::Slider:
        calcSlider();
        break;
    case State::SliderSlow:
        calcWalk();
        calcTransform();
        break;
    case State::StandUp:
        calcStandUp();
        break;
    }
}

/// TODO: Maybe not needed
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

/// @addr{0x80776498}
void ObjectPenguinS::calcSlider() {
    calcRail();
    calcPos();
    calcRot();

    if (--m_anmTimer == 0) {
        m_state = State::SliderSlow;
    }
}

/// @addr{0x80776670}
void ObjectPenguinS::calcStandUp() {
    calcRail();
    calcPos();
    calcRot();

    if (--m_anmTimer == 0) {
        enterWalk();
    }
}

/// @addr{0x807768A0}
void ObjectPenguinS::calcRail() {
    if (m_railInterpolator->calc() != RailInterpolator::Status::SegmentEnd) {
        return;
    }

    const auto &curPoint = m_railInterpolator->curPoint();
    u16 setting = curPoint.setting[0];

    if (setting == 0 && m_state == State::SliderSlow) {
        auto *anmMgr = m_drawMdl->anmMgr();
        anmMgr->playAnim(0.0f, 1.0f, 3);
        m_state = State::StandUp;
        m_anmTimer = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount();
    } else if (setting == 1 && m_state == State::Walk) {
        auto *anmMgr = m_drawMdl->anmMgr();
        anmMgr->playAnim(0.0f, 1.0f, 1);
        m_state = State::Slider;
        m_anmTimer = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount();
    } else if (setting == 2 && m_state == State::Walk) {
        m_state = State::Dive;
    }

    m_railInterpolator->setCurrVel(static_cast<f32>(curPoint.setting[1]));
}

} // namespace Field
