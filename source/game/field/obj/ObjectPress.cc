#include "ObjectPress.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/ObjectDirector.hh"

#include "game/kart/KartObject.hh"

namespace Field {

/// @addr{0x80777564}
ObjectPress::ObjectPress(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_loweringVelocity(0.0f),
      m_raisedDuration(static_cast<s32>(params.setting(2))) {}

/// @addr{0x807775FC}
ObjectPress::~ObjectPress() = default;

/// @addr{0x8077763C}
void ObjectPress::init() {
    constexpr f32 FLOOR_CHECK_SPEED = 100.0f;
    constexpr f32 HEIGHT = 20.0f;
    constexpr EGG::Vector3f HITBOX_OFFSET = EGG::Vector3f(0.0f, HEIGHT, 0.0f);

    m_state = State::Raised;
    m_startingRise = false;
    m_anmDuration = 0;
    ASSERT(m_mapObj);
    m_raisedTimer = static_cast<s32>(m_mapObj->setting(1));
    m_windUpTimer = 0;
    m_raisedHeight = m_pos.y;

    bool hasCol = false;
    CollisionInfo info;
    auto *colDir = CollisionDirector::Instance();

    // The game performs floor collision checks here to make sure that if a stomper is positioned
    // above a sloped floor, it does not partially clip through the floor when it stomps down.
    // If there is no floor below the stomper, then we get stuck in an infinite loop here.
    while (!hasCol) {
        m_flags.setBit(eFlags::Position);
        m_pos.y -= FLOOR_CHECK_SPEED;

        hasCol = colDir->checkSphereFull(HEIGHT, m_pos + HITBOX_OFFSET, EGG::Vector3f::inf,
                KCL_TYPE_FLOOR, &info, nullptr, 0);
    }

    EGG::Vector3f loweredPos = m_pos + info.tangentOff;
    m_pos.x = loweredPos.x;
    m_pos.z = loweredPos.z;
    m_loweredHeight = loweredPos.y;
    m_flags.setBit(eFlags::Position);
    m_pos.y = m_raisedHeight;
    m_startedLowered = false;
}

/// @addr{0x8077788C}
void ObjectPress::calc() {
    m_startedLowered = false;

    switch (m_state) {
    case State::Raised:
        calcRaised();
        break;
    case State::WindUp:
        calcWindUp();
        break;
    case State::Lowering:
        calcLowering();
        break;
    case State::Lowered:
        calcLowered();
        break;
    case State::Raising:
        calcRaising();
        break;
    }
}

/// @addr{0x807786E4}
void ObjectPress::loadAnims() {
    std::array<const char *, 1> names = {{
            "Press",
    }};

    std::array<Render::AnmType, 1> types = {{
            Render::AnmType::Chr,
    }};

    linkAnims(names, types);
}

/// @addr{0x8077840C}
void ObjectPress::createCollision() {
    constexpr f32 POINT_SCALE = 10.0f;
    constexpr std::array<EGG::Vector3f, 16> POINTS = {{
            EGG::Vector3f(37.5f, 0.0f, 43.5f) * POINT_SCALE,
            EGG::Vector3f(43.0f, 0.0f, 38.0f) * POINT_SCALE,
            EGG::Vector3f(37.5f, 0.0f, -43.5f) * POINT_SCALE,
            EGG::Vector3f(43.0f, 0.0f, 38.0f) * POINT_SCALE,
            EGG::Vector3f(-37.5f, 0.0f, -43.5f) * POINT_SCALE,
            EGG::Vector3f(-43.0f, 0.0f, -38.0f) * POINT_SCALE,
            EGG::Vector3f(-37.5f, 0.0f, 43.5f) * POINT_SCALE,
            EGG::Vector3f(-43.0f, 0.0f, 38.0f) * POINT_SCALE,
            EGG::Vector3f(40.0f, 143.8f, 46.0f) * POINT_SCALE,
            EGG::Vector3f(45.5f, 143.8f, 40.5f) * POINT_SCALE,
            EGG::Vector3f(40.0f, 143.8f, -46.0f) * POINT_SCALE,
            EGG::Vector3f(45.5f, 143.8f, -40.5f) * POINT_SCALE,
            EGG::Vector3f(-40.0f, 143.8f, -46.0f) * POINT_SCALE,
            EGG::Vector3f(-45.5f, 143.8f, -40.5f) * POINT_SCALE,
            EGG::Vector3f(-40.0f, 143.8f, 46.0f) * POINT_SCALE,
            EGG::Vector3f(-45.5f, 143.8f, 40.5f) * POINT_SCALE,
    }};

    m_collision = new ObjectCollisionConvexHull(POINTS);
}

/// @addr{0x807782D4}
Kart::Reaction ObjectPress::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    constexpr f32 CRUSH_THRESHOLD = 430.0f;

    EGG::Vector3f diff = kartObj->pos() - m_pos;
    bool close =
            EGG::Mathf::abs(diff.x) < CRUSH_THRESHOLD && EGG::Mathf::abs(diff.z) < CRUSH_THRESHOLD;

    // ObjectId::Press corresponds to a WallAllSpeed reaction, so when the stomper is coming down,
    // the base game instead uses ObjectId::PressSoko to induce LongCrushLoseItem.
    if (close && (m_state == State::Lowering || m_startedLowered)) {
        const auto &hitTable = ObjectDirector::Instance()->hitTableKart();
        return hitTable.reaction(hitTable.slot(ObjectId::PressSoko));
    }

    return reactionOnKart;
}

/// @addr{0x80777A90}
void ObjectPress::calcRaised() {
    constexpr u32 WINDUP_FRAMES = 10;

    if (--m_raisedTimer == 0) {
        m_state = State::WindUp;
        m_windUpTimer = WINDUP_FRAMES;
    }
}

void ObjectPress::calcWindUp() {
    constexpr f32 SPEED = 10.0f;

    m_flags.setBit(eFlags::Position);
    m_pos.y += SPEED;

    if (--m_windUpTimer == 0) {
        m_state = State::Lowering;
    }
}

/// @addr{0x80777B90}
void ObjectPress::calcLowering() {
    constexpr f32 ACCEL = 3.0f;

    m_loweringVelocity -= ACCEL;
    m_flags.setBit(eFlags::Position);
    m_pos.y += m_loweringVelocity;

    checkCollisionLowering();
}

void ObjectPress::calcLowered() {
    if (--m_anmDuration > 0) {
        return;
    }

    if (m_startingRise) {
        m_state = State::Raising;
    } else {
        auto *anmMgr = m_drawMdl->anmMgr();
        f32 frameCount = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount();
        anmMgr->playAnim(frameCount, -ANM_RATE, 0);

        m_startingRise = true;
        m_anmDuration = frameCount / ANM_RATE;
    }
}

void ObjectPress::calcRaising() {
    constexpr f32 SPEED = 10.0f;

    f32 height = m_pos.y + SPEED;

    if (height < m_raisedHeight) {
        m_pos.y = height;
    } else {
        m_pos.y = m_raisedHeight;
        m_state = State::Raised;
        m_raisedTimer = m_raisedDuration;
    }

    m_flags.setBit(eFlags::Position);
}

/// @addr{0x80777D10}
void ObjectPress::checkCollisionLowering() {
    constexpr f32 RADIUS = 10.0f;
    constexpr EGG::Vector3f HITBOX_OFFSET = EGG::Vector3f(0.0f, RADIUS, 0.0f);

    CollisionInfo info;

    if (!CollisionDirector::Instance()->checkSphereFull(RADIUS, m_pos + HITBOX_OFFSET,
                EGG::Vector3f::inf, KCL_TYPE_FLOOR, &info, nullptr, 0)) {
        return;
    }

    m_loweringVelocity = 0.0f;
    m_pos = m_pos + info.tangentOff;
    m_flags.setBit(eFlags::Position);

    auto *anmMgr = m_drawMdl->anmMgr();
    anmMgr->playAnim(0.0f, ANM_RATE, 0);

    m_state = State::Lowered;
    m_startingRise = false;
    m_anmDuration = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount() / ANM_RATE;
    m_startedLowered = true;
}

/// @addr{0x8076E7AC}
ObjectPressSenko::ObjectPressSenko(const System::MapdataGeoObj &params)
    : ObjectPress(params), m_startingWindup(false) {}

/// @addr{0x8076E818}
ObjectPressSenko::~ObjectPressSenko() = default;

/// @addr{0x8076E870}
void ObjectPressSenko::calcRaised() {
    if (m_startingWindup) {
        startWindup();
        m_startingWindup = false;
    }
}

/// @addr{0x8077808C}
void ObjectPressSenko::startWindup() {
    constexpr u32 WINDUP_DURATION = 10;

    m_state = State::WindUp;
    m_windUpTimer = WINDUP_DURATION;
}

} // namespace Field
