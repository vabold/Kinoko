#include "ObjectKuribo.hh"

#include "game/field/CollisionDirector.hh"

#include <cmath>

namespace Field {

/// @addr{0x806DB184}
ObjectKuribo::ObjectKuribo(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), StateManager(this) {
    m_animStep = static_cast<f32>(m_mapObj->setting(2)) / 100.0f;
    m_speedStep = static_cast<f32>(m_mapObj->setting(1)) / 100.0f;
}

/// @addr{0x806DB3A0}
ObjectKuribo::~ObjectKuribo() = default;

/// @addr{0x806DB40C}
void ObjectKuribo::init() {
    calcTransform();
    m_origin = m_transform.base(2);

    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setCurrVel(0.0f);

    m_currSpeed = 0.0f;
    m_animTimer = 0.0f;
    m_frameCount = 0;

    auto *anmMgr = m_drawMdl->anmMgr();
    anmMgr->playAnim(0.0f, m_animStep, 0);
    m_maxAnimTimer = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount();
    m_nextStateId = 1;
}

/// @addr{0x806DB5B0}
void ObjectKuribo::calc() {
    m_animTimer = ::fmodf(static_cast<f32>(m_frameCount) * m_animStep, m_maxAnimTimer);

    if (m_nextStateId >= 0) {
        m_currentStateId = m_nextStateId;
        m_nextStateId = -1;
        m_currentFrame = 0;

        auto enterFunc = m_entries[m_entryIds[m_currentStateId]].onEnter;
        (this->*enterFunc)();
    } else {
        ++m_currentFrame;
    }

    auto calcFunc = m_entries[m_entryIds[m_currentStateId]].onCalc;
    (this->*calcFunc)();

    ++m_frameCount;
}

/// @addr{0x806dd278}
void ObjectKuribo::loadAnims() {
    std::array<const char *, 2> names = {{
            "walk_l",
            "walk_r",
    }};

    std::array<Render::AnmType, 2> types = {{
            Render::AnmType::Chr,
            Render::AnmType::Chr,
    }};

    linkAnims(names, types);
}

void ObjectKuribo::enterStateStub() {}

void ObjectKuribo::calcStateStub() {}

/// @addr{0x806DC220}
/// @brief Called when the Goomba is changing direction.
void ObjectKuribo::calcStateReroute() {
    if (m_railInterpolator->curPoint().setting[0] < m_currentFrame) {
        m_nextStateId = 1;
    }

    checkSphereFull();
    calcRot();
    setMatrixTangentTo(m_rot, m_origin);
}

/// @addr{0x806DC3F8}
/// @brief Called when Goomba is walking along the rail.
void ObjectKuribo::calcStateWalk() {
    calcAnim();
}

/// @addr{0x806DCDDC}
void ObjectKuribo::calcAnim() {
    bool shouldMove;

    if (m_railInterpolator->isMovementDirectionForward()) {
        shouldMove = m_animTimer > 15.0f && m_animTimer < 25.0f;
    } else {
        shouldMove = m_animTimer > 45.0f && m_animTimer < 55.0f;
    }

    if (shouldMove) {
        m_currSpeed = std::min(10.0f, m_currSpeed + m_speedStep);
    } else {
        m_currSpeed = std::max(0.0f, m_currSpeed - m_speedStep);
    }

    m_railInterpolator->setCurrVel(m_currSpeed);
    m_flags |= 1;
    const auto &curPos = m_railInterpolator->curPos();
    m_pos.x = curPos.x;
    m_pos.z = curPos.z;

    if (m_railInterpolator->calc() == RailInterpolator::Status::ChangingDirection) {
        m_nextStateId = 0;
        m_currSpeed = 0.0f;
    }

    checkSphereFull();
    calcRot();
    setMatrixTangentTo(m_rot, m_origin);
}

/// @addr{0x806DCC9C}
void ObjectKuribo::calcRot() {
    m_rot = interpolate(0.1f, m_rot, m_floorNrm);

    if (m_rot.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_rot.normalise2();
    } else {
        m_rot = EGG::Vector3f::ey;
    }
}

/// @addr{0x806DCB58}
void ObjectKuribo::checkSphereFull() {
    constexpr f32 RADIUS = 50.0f;

    // Apply gravity if we're not changing direction
    if (m_currentStateId != 0) {
        m_flags |= 1;
        m_pos.y -= 2.0f;
    }

    CollisionInfo colInfo;
    EGG::Vector3f pos = m_pos;
    pos.y += RADIUS;

    bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, pos, EGG::Vector3f::inf,
            KCL_TYPE_FLOOR, &colInfo, nullptr, 0);

    if (hasCol) {
        m_pos += colInfo.tangentOff;
        m_flags |= 1;

        if (colInfo.floorDist > -std::numeric_limits<f32>::min()) {
            m_floorNrm = colInfo.floorNrm;
        }
    }
}

/// @addr{0x806DCD48}
EGG::Vector3f ObjectKuribo::interpolate(f32 scale, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1) const {
    return v0 + (v1 - v0) * scale;
}

const std::array<StateManagerEntry<ObjectKuribo>, 4> StateManager<ObjectKuribo>::STATE_ENTRIES = {{
        {0, &ObjectKuribo::enterStateStub, &ObjectKuribo::calcStateReroute},
        {1, &ObjectKuribo::enterStateStub, &ObjectKuribo::calcStateWalk},
        {2, &ObjectKuribo::enterStateStub, &ObjectKuribo::calcStateStub},
        {3, &ObjectKuribo::enterStateStub, &ObjectKuribo::calcStateStub},
}};

StateManager<ObjectKuribo>::StateManager(ObjectKuribo *obj) {
    constexpr size_t ENTRY_COUNT = 4;

    m_obj = obj;
    m_entries = std::span{STATE_ENTRIES};
    m_entryIds = std::span(new u16[ENTRY_COUNT], ENTRY_COUNT);

    // The base game initializes all entries to 0xffff, possibly to avoid an uninitialized value
    for (auto &id : m_entryIds) {
        id = 0xffff;
    }

    for (size_t i = 0; i < m_entryIds.size(); ++i) {
        m_entryIds[STATE_ENTRIES[i].id] = i;
    }
}

StateManager<ObjectKuribo>::~StateManager() {
    delete[] m_entryIds.data();
}

} // namespace Field
