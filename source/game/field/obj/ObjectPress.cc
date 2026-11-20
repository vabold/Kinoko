#include "ObjectPress.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"
#include "game/field/ObjectCollisionConvexHull.hh"
#include "game/field/ObjectDirector.hh"

#include "game/kart/KartObject.hh"

#include "game/render/AnmMgr.hh"
#include "game/render/DrawMdl.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x80777564}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes @ref m_loweringSpeed to zero. Sets @ref m_initDelay based on param
/// setting 2 and @ref m_raisedDuration based on param setting 3.
ObjectPress::ObjectPress(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      m_loweringSpeed(0.0f),
      m_initDelay(static_cast<u32>(params.setting(1))),
      m_raisedDuration(static_cast<u32>(params.setting(2))) {}

/// @addr{0x807775FC}
/// @brief Default virtual destructor
ObjectPress::~ObjectPress() = default;

/// @addr{0x8077763C}
/// @copybrief ObjectBase::init()
/// @details Repeatedly performs floor collision checks to find the lowered position and to
/// laterally adjust the stomper's position to avoid the stomper partially clipping into the floor.
/// Each collision check lowers the stomper's position by `100.0f` units until a collision occurs.
/// @note If the press is positioned such that there is no floor beneath it, then this function will
/// be stuck in an infinite loop, causing the race to never start.
void ObjectPress::init() {
    constexpr f32 FLOOR_CHECK_SPEED = 100.0f;
    constexpr f32 HEIGHT = 20.0f;
    constexpr EGG::Vector3f HITBOX_OFFSET = EGG::Vector3f(0.0f, HEIGHT, 0.0f);

    m_state = State::Raised;
    m_startingRise = false;
    initTimers();
    m_raisedHeight = pos().y;

    bool hasCol = false;
    CollisionInfo info;
    auto *colDir = CollisionDirector::Instance();

    while (!hasCol) {
        subPos(EGG::Vector3f(0.0f, FLOOR_CHECK_SPEED, 0.0f));

        hasCol = colDir->checkSphereFull(HEIGHT, pos() + HITBOX_OFFSET, EGG::Vector3f::inf,
                KCL_TYPE_FLOOR, &info, nullptr, 0);
    }

    EGG::Vector3f loweredPos = pos() + info.tangentOff;
    setPos(EGG::Vector3f(loweredPos.x, m_raisedHeight, loweredPos.z));
    m_startedLowered = false;
}

/// @addr{0x8077788C}
/// @copybrief ObjectBase::calc()
/// @details Calls the appropriate calculation function based on the press's current state.
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
/// @copybrief ObjectBase::loadAnims()
/// @details Loads the `Press` animation.
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
/// @copybrief ObjectBase::createCollision()
/// @details Defines the convex hull geometry that represents the press's collision shape.
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

    m_collision = EGG::egg_new<ObjectCollisionConvexHull>(POINTS);
}

/// @addr{0x807787E8}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectPress::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @copybrief ObjectBase::getCollisionRadius()
/// @return The collision radius of the press, `700.0f`.
f32 ObjectPress::getCollisionRadius() const {
    return 700.0f;
}

/// @addr{0x807782D4}
/// @copybrief ObjectCollidable::onCollision()
/// @param kartObj The kart object involved in the collision
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @return @ref Kart::Reaction::LongCrushLoseItem if the kart should be crushed, otherwise @ref
/// Kart::Reaction::WallAllSpeed.
/// @details Checks if the kart is within `430.0f` units of the press's position. If so, and the
/// press is lowering, then the kart will be squished and items will be lost.
Kart::Reaction ObjectPress::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    constexpr f32 CRUSH_THRESHOLD = 430.0f;

    EGG::Vector3f diff = kartObj->pos() - pos();
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
/// @brief Runs every frame that the press is in the raised state
/// @details When the raised timer expires, the press will transition to the windup state where
/// it winds up for 10 frames before stomping down.
void ObjectPress::calcRaised() {
    constexpr u32 WINDUP_FRAMES = 10;

    if (--m_raisedTimer == 0) {
        m_state = State::WindUp;
        m_windUpTimer = WINDUP_FRAMES;
    }
}

/// @addr{0x8077786C}
/// @brief Initializes timers for the press's lifecycle
/// @details Sets @ref m_anmTimer to 0, @ref m_raisedTimer to @ref m_initDelay, and @ref
/// m_windUpTimer to 0.
void ObjectPress::initTimers() {
    m_anmTimer = 0;
    m_raisedTimer = m_initDelay;
    m_windUpTimer = 0;
}

/// @addr{0x80777B54}
/// @brief Runs every frame that the press is winding up before stomping down
/// @details Raises the stomper's position by `10.0f` every frame it is winding up. When the
/// windup timer expires, the press will transition to the lowering state.
void ObjectPress::calcWindUp() {
    constexpr f32 SPEED = 10.0f;

    addPos(EGG::Vector3f(0.0f, SPEED, 0.0f));

    if (--m_windUpTimer == 0) {
        m_state = State::Lowering;
    }
}

/// @addr{0x80777B90}
/// @brief Runs every frame that the press is stomping down
/// @details The press accelerates downwards with constant acceleration of `3.0f` and calls @ref
/// checkCollisionLowering() to check if it has hit the floor.
void ObjectPress::calcLowering() {
    constexpr f32 ACCEL = 3.0f;

    m_loweringSpeed -= ACCEL;
    addPos(EGG::Vector3f(0.0f, m_loweringSpeed, 0.0f));
    checkCollisionLowering();
}

/// @addr{0x80777D10}
/// @brief Runs while the press is lowering to check if it has hit the floor
/// @details Performs a spherical collision check with a radius of `10.0f` units and a vertical
/// offset of `10.0f` units. If no collision occurs, this function returns early. Otherwise, resets
/// @ref m_loweringSpeed to zero and offsets the press's position upwards to prevent clipping into
/// the floor. The press animation is played and the animation duration is cached to @ref
/// m_anmTimer. Finally, @ref m_state is set to @ref State::Lowered, @ref m_startingRise is set to
/// `false`, and @ref m_startedLowered is set to `true`.
void ObjectPress::checkCollisionLowering() {
    constexpr f32 RADIUS = 10.0f;
    constexpr EGG::Vector3f HITBOX_OFFSET = EGG::Vector3f(0.0f, RADIUS, 0.0f);

    CollisionInfo info;

    if (!CollisionDirector::Instance()->checkSphereFull(RADIUS, pos() + HITBOX_OFFSET,
                EGG::Vector3f::inf, KCL_TYPE_FLOOR, &info, nullptr, 0)) {
        return;
    }

    m_loweringSpeed = 0.0f;
    addPos(info.tangentOff);

    auto *anmMgr = m_drawMdl->anmMgr();
    anmMgr->playAnim(0.0f, ANM_RATE, 0);

    m_state = State::Lowered;
    m_startingRise = false;
    m_anmTimer = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount() / ANM_RATE;
    m_startedLowered = true;
}

/// @addr{0x80777BC0}
/// @brief Runs every frame that the press is in contact with the floor
/// @details Decrements @ref m_anmTimer. When it reaches 0, it calls @ref enterRaising() to play the
/// animation in reverse. When the reverse animation's timer expires, sets @ref m_state to @ref
/// State::Raising.
void ObjectPress::calcLowered() {
    if (--m_anmTimer > 0) {
        return;
    }

    if (m_startingRise) {
        m_state = State::Raising;
    } else {
        enterRaising();
    }
}

/// @addr{0x80778218}
/// @brief Runs when the stomp animation starts playing in reverse
/// @details Plays the stomping animation in reverse, caching the animation duration to @ref
/// m_anmTimer. Also sets the @ref m_startingRise flag.
void ObjectPress::enterRaising() {
    auto *anmMgr = m_drawMdl->anmMgr();
    f32 frameCount = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount();
    anmMgr->playAnim(frameCount, -ANM_RATE, 0);
    m_startingRise = true;
    m_anmTimer = frameCount / ANM_RATE;
}

/// @addr{0x80777CB0}
/// @brief Runs every frame that the press is rising back up to the raised position
/// @details The press raises up with a speed of `10.0f` units per frame. When the press reaches the
/// raised position, it will transition to the raised state by setting @ref m_state to @ref
/// State::Raised and resetting @ref m_raisedTimer to @ref m_raisedDuration.
void ObjectPress::calcRaising() {
    constexpr f32 SPEED = 10.0f;

    f32 height = pos().y + SPEED;

    if (height < m_raisedHeight) {
        setPos(EGG::Vector3f(pos().x, height, pos().z));
    } else {
        setPos(EGG::Vector3f(pos().x, m_raisedHeight, pos().z));
        m_state = State::Raised;
        m_raisedTimer = m_raisedDuration;
    }
}

/// @addr{0x8076E7AC}
/// @copydoc ObjectPress::ObjectPress(const System::MapdataGeoObj &)
/// @details Initializes the @ref m_shouldStartWindup flag to `false`.
ObjectPressSenko::ObjectPressSenko(const System::MapdataGeoObj &params)
    : ObjectPress(params), m_shouldStartWindup(false) {}

/// @addr{0x8076E818}
/// @brief Default virtual destructor
ObjectPressSenko::~ObjectPressSenko() = default;

/// @addr{0x8076EA28}
/// @copybrief ObjectBase::id()
/// @return The ID of the press object, `ObjectId::Press`.
ObjectId ObjectPressSenko::id() const {
    return ObjectId::Press;
}

/// @addr{0x8076EA20}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectPressSenko::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x8076EA30}
/// @copybrief ObjectBase::getResources()
/// @return The resource name for the press, `Press`.
const char *ObjectPressSenko::getResources() const {
    return "Press";
}

/// @addr{0x8076EA3C}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the press, `Press`.
const char *ObjectPressSenko::getKclName() const {
    return "Press";
}

/// @addr{0x8076E870}
/// @copybrief ObjectPress::calcRaised()
/// @details If the press has been told to begin stomping, it will start winding up for the
/// stomp by calling @ref startWindup() and resetting @ref m_shouldStartWindup to false.
void ObjectPressSenko::calcRaised() {
    if (m_shouldStartWindup) {
        startWindup();
        m_shouldStartWindup = false;
    }
}

/// @brief Interface for @ref ObjectItemboxBlock to tell the stomper to begin stomping
/// @details Sets @ref m_shouldStartWindup to true.
void ObjectPressSenko::beginStomp() {
    m_shouldStartWindup = true;
}

/// @addr{0x8077808C}
/// @brief Runs once when the press is in the raised state and has been told to begin stomping
/// down
/// @details Sets @ref m_state to @ref State::WindUp and initializes @ref m_windUpTimer to 10 so
/// the press will wind up for 10 frames before stomping down.
void ObjectPressSenko::startWindup() {
    constexpr u32 WINDUP_DURATION = 10;

    m_state = State::WindUp;
    m_windUpTimer = WINDUP_DURATION;
}

} // namespace Kinoko::Field
