#include "ObjectHeyho.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/ObjectCollisionBase.hh"
#include "game/field/Rail.hh"
#include "game/field/RailInterpolator.hh"
#include "game/field/RailManager.hh"

#include "game/render/DrawMdl.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x806CE828}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes the Shy Guy's color based on param setting 2. Initializes the Shy Guy's
/// maximum velocity based on param setting 1. Calculates the apex and midpoint of the Shy Guy's
/// route, and derives the necessary acceleration for the Shy Guy.
ObjectHeyho::ObjectHeyho(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      StateManager(this, STATE_ENTRIES),
      m_color(static_cast<Color>(params.setting(1))) {
    const auto *rail = RailManager::Instance()->rail(params.pathId());
    ASSERT(rail);
    const auto &railPts = rail->points();

    // The two endpoints are candidates for the highest Y coordinate in the route
    // The midpoint's Y coordinate should be the lowest in the route
    m_apex = std::max(railPts.front().pos.y, railPts.back().pos.y);
    m_midpoint = railPts[railPts.size() / 2].pos;

    // The object should speed up as we approach the low point and slow down as we leave it
    // We form an acceleration constant so multiplying with (pos - center) gives v^2
    // This way, we can inversely correlate height difference with speed
    ASSERT(m_apex - m_midpoint.y > std::numeric_limits<f32>::epsilon());
    f32 maxVel = static_cast<f32>(static_cast<s16>(params.setting(0)));
    m_maxVelSq = maxVel * maxVel;
    m_accel = m_maxVelSq / (m_apex - m_midpoint.y);
}

/// @addr{0x806CEB24}
/// @brief Default virtual destructor
ObjectHeyho::~ObjectHeyho() = default;

/// @addr{0x806CEB90}
/// @copybrief ObjectBase::init()
/// @details Initializes the Shy Guy's rail interpolator to the midpoint of its route and snaps the
/// Shy Guy's position to this midpoint. Derives the Shy Guy's current speed based on @ref
/// m_maxVelSq, @ref m_accel, and @ref m_midpoint, and sets the initial velocity vector accordingly.
/// Finally, initializes the Shy Guy's orientation vectors, other internal state, and activates the
/// @ref Animation::Move animation.
void ObjectHeyho::init() {
    ASSERT(m_railInterpolator);
    m_railInterpolator->init(0.0f, m_railInterpolator->pointCount() / 2);
    setPos(m_railInterpolator->curPos());
    m_currentSpeed = EGG::Mathf::sqrt(
            m_maxVelSq - m_accel * (m_railInterpolator->curPos().y - m_midpoint.y));

    m_initVel = m_railInterpolator->curTangentDir() * m_currentSpeed;
    m_floorCollision = false;
    m_up = EGG::Vector3f::ey;
    m_forward = EGG::Vector3f::ez;
    m_freeFall = false;
    m_spinFrame = 0;

    changeAnimation(Animation::Move);
}

/// @addr{0x806CEDF8}
/// @copybrief ObjectBase::calc()
/// @details Checks if the Shy Guy should transition to the next state. Updates the Shy Guy's
/// position along the rail. Evaluates the Shy Guy's state machine. Finally, interpolates the
/// Shy Guy's transform based off the current floor normal.
void ObjectHeyho::calc() {
    calcStateTransition();
    calcMotion();
    StateManager::calc();
    calcInterp();
}

/// @addr{0x806D013C}
/// @copybrief ObjectBase::loadAnims()
/// @details Loads the animations for the Shy Guy moving normally, jumping mid-air, and landing
/// after a jump.
/// @note These animations are implemented in Kinoko because the Shy Guy's collision check function
/// branches depending on which animation is currently active.
void ObjectHeyho::loadAnims() {
    std::array<const char *, 4> names = {{
            "body_color",
            "move",
            "jump",
            "jump_ed",
    }};

    std::array<Render::AnmType, 4> types = {{
            Render::AnmType::Pat,
            Render::AnmType::Chr,
            Render::AnmType::Chr,
            Render::AnmType::Chr,
    }};

    linkAnims(names, types);
}

/// @addr{0x806D01D4}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details Scales the collision transform based on the Shy Guy's current position, scale, and
/// initial velocity. It also applies a `100.0f` offset in the direction of @ref m_up to the
/// collision transform.
void ObjectHeyho::calcCollisionTransform() {
    auto *objCol = collision();
    if (!objCol) {
        return;
    }

    EGG::Matrix34f tm;
    tm.makeT(EGG::Vector3f(0.0f, 100.0f, 0.0f));
    calcTransform();
    EGG::Matrix34f m = transform().multiplyTo(tm);
    objCol->transform(m, scale(), m_initVel);
}

/// @addr{0x806D02B4}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc and @ref eLoadFlags::Draw so that the object is
/// calculated every frame
ObjectBase::LoadFlags ObjectHeyho::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc, eLoadFlags::Draw);
}

/// @addr{0x806CF714}
/// @brief Runs when the Shy Guy enters the jump state
/// @details Resets the spin frame counter for mid-air spinning
void ObjectHeyho::enterJump() {
    m_spinFrame = 0;
}

/// @addr{0x806CF4D0}
/// @brief Runs every frame the Shy Guy is "grounded" (when the rail point's second setting is 0)
/// @details Computes the Shy Guy's movement direction. Checks to see if the Shy Guy is colliding
/// with the floor. If a collision is detected, caches the floor normal and updates the Shy Guy's
/// position so that it visually rests on the floor. If the Shy Guy was in the post-jump animation
/// state and the animation's length has elapsed, updates the Shy Guy's animation to @ref
/// Animation::Move.
void ObjectHeyho::calcMove() {
    m_forward = m_railInterpolator->nextPoint().pos - m_railInterpolator->curPoint().pos;
    m_floorCollision = false;

    CollisionInfo info;

    if (CollisionDirector::Instance()->checkSphereFull(COLLISION_RADIUS, pos() + COLLISION_OFFSET,
                EGG::Vector3f::inf, KCL_TYPE_FLOOR, &info, nullptr, 0)) {
        m_floorCollision = true;
        if (info.floorDist > -std::numeric_limits<f32>::min()) {
            m_floorNrm = info.floorNrm;
        }

        setPos(pos() + info.tangentOff - m_floorNrm * 60.0f);

        if (m_currentAnim == Animation::Jumped) {
            const auto *anim = m_drawMdl->anmMgr()->activeAnim(Render::AnmType::Chr);
            if (anim->frame() >= anim->frameCount()) {
                changeAnimation(Animation::Move);
            }
        }
    }
}

/// @addr{0x806CF72C}
/// @brief Runs every frame the Shy Guy is "jumping" (when the rail point's second setting is 1)
/// @details Detects if the Shy Guy is colliding with an invisible wall or floor. If a collision
/// occurs and caches the floor normal (if a collision with the floor occurred). If the collision
/// was not with an invisible wall and the Shy Guy is not in @ref Animation::Move (i.e. it has
/// landed), points @ref m_forward along the current rail segment and, if it was in @ref
/// Animation::Jump, switches @ref m_currentAnim to @ref Animation::Jumped. Otherwise, the Shy Guy
/// is still airborne: sets @ref m_floorCollision to `false`, points @ref m_forward away from the
/// midpoint, applies the mid-air spin for Red Shy Guys, and increments the @ref m_spinFrame
/// counter. Finally, if the Shy Guy is in the @ref Animation::Move animation and has traversed 60%
/// of the current rail segment, updates the Shy Guy's animation to @ref Animation::Jump.
void ObjectHeyho::calcJump() {
    constexpr s16 SPIN_DELAY_FRAMES = 5;
    constexpr s16 SPIN_RATE = 12; // degrees per frame
    constexpr s16 SPIN_DEGREES = 720;

    m_floorCollision = false;

    CollisionInfo info;
    Field::KCLTypeMask flags = KCL_NONE;

    if (CollisionDirector::Instance()->checkSphereFull(COLLISION_RADIUS, pos() + COLLISION_OFFSET,
                EGG::Vector3f::inf, KCL_TYPE_VEHICLE_COLLIDEABLE, &info, &flags, 0)) {
        m_floorCollision = true;
        if (info.floorDist > -std::numeric_limits<f32>::min()) {
            m_floorNrm = info.floorNrm;
        }

        // Not m_pos += info.tangentOff - m_floorNrm * 60.0f
        // The former requires m_pos to be the last addition to occur
        // TODO: 100.0f - 10.0f - 30.0f? Why is this the case?
        f32 xPos = pos().x + info.tangentOff.x - m_floorNrm.x * 60.0f;
        f32 zPos = pos().z + info.tangentOff.z - m_floorNrm.z * 60.0f;
        setPos(EGG::Vector3f(xPos, pos().y, zPos));

        if (!(flags & KCL_TYPE_BIT(COL_TYPE_INVISIBLE_WALL)) && m_currentAnim != Animation::Move) {
            m_forward = m_railInterpolator->nextPoint().pos - m_railInterpolator->curPoint().pos;
            if (m_currentAnim == Animation::Jump) {
                m_currentAnim = Animation::Jumped;
            }
        } else {
            m_floorCollision = false;
            const auto &curPos = m_railInterpolator->curPoint().pos;
            const auto &nextPos = m_railInterpolator->nextPoint().pos;

            m_forward = (nextPos.y > curPos.y ? nextPos : curPos) - m_midpoint;

            // Red shy guys do a spin. Yes, this is based on color, and no, it's not an animation
            // We couldn't possibly use even one of the six unused settings for this
            if (m_color == Color::Red) {
                s16 frame = m_spinFrame - SPIN_DELAY_FRAMES;
                if (frame >= 0 && frame <= SPIN_DEGREES / SPIN_RATE) {
                    EGG::Matrix34f m;
                    m.setAxisRotation(static_cast<f32>(frame) *
                                    (static_cast<f32>(-SPIN_RATE) * DEG2RAD),
                            m_up);
                    m.setBase(3, EGG::Vector3f::zero);
                    m_forward = m.ps_multVector(m_forward);
                }
            }

            ++m_spinFrame;
        }
    }

    if (m_railInterpolator->segmentT() > 0.6f && m_currentAnim == Animation::Move) {
        changeAnimation(Animation::Jump);
    }
}

/// @brief Sets the specified animation (move or jump)
/// @param anim The animation to set
void ObjectHeyho::changeAnimation(Animation anim) {
    m_drawMdl->anmMgr()->playAnim(0.0f, 1.0f, static_cast<size_t>(anim));
    m_currentAnim = anim;
}

/// @addr{0x806CFD48}
/// @brief Updates the @ref StateManager based off the current and next rail point settings
/// @details If the current or next rail point's second setting is 0, then the Shy Guy
/// transitions to the move state. Otherwise, it transitions to the jump state.
/// @note Contrary to the other classes that inherit from @ref StateManager, this class directly
/// sets @ref m_currentStateId rather than queuing the transition via @ref m_nextStateId.
void ObjectHeyho::calcStateTransition() {
    if (m_railInterpolator->curPoint().setting[1] == 0 ||
            m_railInterpolator->nextPoint().setting[1] == 0) {
        if (m_currentStateId != 0) {
            m_currentStateId = 0;
        }
    } else {
        if (m_currentStateId != 1) {
            m_currentStateId = 1;
        }
    }
}

/// @addr{0x806CFDB0}
/// @brief Updates the position of the Shy Guy along the rail and locks X/Z position when airborne
/// @details There is special handling for an edge case that occurs if the rail endpoint heights
/// aren't the same, in which case the Shy Guy will enter a free fall where the rail changes
/// direction. If the Shy Guy is in freefall, applies a gravitational force of `1.0f` downwards to
/// the Shy Guy's vertical velocity and updates its position accordingly. Otherwise, updates the Shy
/// Guy's current velocity based off the derived @ref m_accel and updates the rail interpolator and
/// Shy Guy's position accordingly, snapping to the rail's height if a floor collision occurred. If
/// the Shy Guy is now changing direction, checks if it should enter free fall. Finally, checks if
/// the Shy Guy is expected to land back on the rail and updates its velocity if so.
void ObjectHeyho::calcMotion() {
    constexpr f32 FREE_FALL_GRAVITY = 1.0f;

    if (!m_freeFall) {
        f32 sqVel = m_maxVelSq - m_accel * (pos().y - m_midpoint.y);
        if (sqVel <= 0.0f) {
            sqVel = 0.001f;
        }
        m_currentSpeed = EGG::Mathf::sqrt(sqVel);
        m_railInterpolator->setSpeed(m_currentSpeed);

        if (m_railInterpolator->calc() == RailInterpolator::Status::RailEnd) {
            // We have an edgecase where the endpoint heights aren't the same
            // While the current velocity reaches 0 at the apex, it's higher on the other side
            // To counter this, the object goes off the rail and into free fall until we land
            m_launchVel = m_currentSpeed;
            if (m_currentSpeed > FREE_FALL_GRAVITY) {
                m_freeFall = true;
            }
        }

        if (m_currentStateId == 0 && !m_floorCollision) {
            const auto &curPos = m_railInterpolator->curPos();
            setPos(EGG::Vector3f(curPos.x, pos().y, curPos.z));
        } else {
            setPos(m_railInterpolator->curPos());
        }
    } else {
        m_currentSpeed -= FREE_FALL_GRAVITY;
        setPos(EGG::Vector3f(pos().x, m_currentSpeed + pos().y, pos().z));
    }

    // m_currentSpeed < 0.0f => either we're in free fall or we just snapped to the rail
    // If we would land back on the rail on the next frame, just let it snap to the rail instead
    if (m_currentSpeed < 0.0f &&
            EGG::Mathf::abs(pos().y - m_railInterpolator->curPos().y) <= -m_currentSpeed) {
        m_currentSpeed = m_launchVel;
        m_freeFall = false;
    }
}

/// @addr{0x806CFFB0}
/// @brief Interpolates the Shy Guy's orientation to reflect the current floor normal
/// @details Interpolates the smoothed up vector, normalises the forward direction vector, and
/// updates the transform matrix accordingly.
void ObjectHeyho::calcInterp() {
    m_up = m_up.interpolate(0.2f, m_floorNrm);
    m_up.normalise2();
    m_forward.normalise2();
    setMatrixTangentTo(m_up, m_forward);
}

} // namespace Kinoko::Field
