#include "ObjectHeyho.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/RailManager.hh"

namespace Field {

/// @addr{0x806CE828}
ObjectHeyho::ObjectHeyho(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), StateManager(this, STATE_ENTRIES),
      m_color(static_cast<s32>(params.setting(1))) {
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
    f32 maxVel = static_cast<f32>(params.setting(0));
    m_maxVelSq = maxVel * maxVel;
    m_accel = m_maxVelSq / (m_apex - m_midpoint.y);
}

/// @addr{0x806CEB24}
ObjectHeyho::~ObjectHeyho() = default;

/// @addr{0x806CEB90}
void ObjectHeyho::init() {
    ASSERT(m_railInterpolator);
    m_railInterpolator->init(0.0f, m_railInterpolator->pointCount() / 2);
    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);
    m_currentVel = EGG::Mathf::sqrt(
            m_maxVelSq - m_accel * (m_railInterpolator->curPos().y - m_midpoint.y));

    m_transformOffset = m_railInterpolator->curTangentDir() * m_currentVel;
    m_floorCollision = false;
    m_up = EGG::Vector3f::ey;
    m_forward = EGG::Vector3f::ez;
    m_freeFall = false;
    m_spinFrame = 0;

    changeAnimation(Animation::Move);
}

/// @addr{0x806CEDF8}
void ObjectHeyho::calc() {
    calcStateTransition();
    calcMotion();
    StateManager::calc();
    calcInterp();
}

/// @addr{0x806D013C}
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
void ObjectHeyho::calcCollisionTransform() {
    auto *objCol = collision();
    if (!objCol) {
        return;
    }

    EGG::Matrix34f tm;
    tm.makeT(EGG::Vector3f(0.0f, 100.0f, 0.0f));
    calcTransform();
    EGG::Matrix34f m = m_transform.multiplyTo(tm);
    objCol->transform(m, m_scale, m_transformOffset);
}

/// @addr{0x806CF4D0}
void ObjectHeyho::calcMove() {
    m_forward = m_railInterpolator->nextPoint().pos - m_railInterpolator->curPoint().pos;
    m_floorCollision = false;

    CollisionInfo info;

    if (CollisionDirector::Instance()->checkSphereFull(COLLISION_RADIUS, m_pos + COLLISION_OFFSET,
                EGG::Vector3f::inf, KCL_TYPE_FLOOR, &info, nullptr, 0)) {
        m_floorCollision = true;
        if (info.floorDist > -std::numeric_limits<f32>::min()) {
            m_floorNrm = info.floorNrm;
        }

        // Not m_pos += info.tangentOff - m_floorNrm * 60.0f
        // The former requires m_pos to be the last addition to occur
        // TODO: 100.0f - 10.0f - 30.0f? Why is this the case?
        m_pos = m_pos + info.tangentOff - m_floorNrm * 60.0f;
        m_flags.setBit(eFlags::Position);

        if (m_currentAnim == Animation::Jumped) {
            const auto *anim = m_drawMdl->anmMgr()->activeAnim(Render::AnmType::Chr);
            if (anim->frame() >= anim->frameCount()) {
                changeAnimation(Animation::Move);
            }
        }
    }
}

/// @addr{0x806CF72C}
void ObjectHeyho::calcJump() {
    constexpr s16 SPIN_DELAY_FRAMES = 5;
    constexpr s16 SPIN_RATE = 12; // degrees per frame
    constexpr s16 SPIN_DEGREES = 720;

    m_floorCollision = false;

    CollisionInfo info;
    Field::KCLTypeMask flags = KCL_NONE;

    if (CollisionDirector::Instance()->checkSphereFull(COLLISION_RADIUS, m_pos + COLLISION_OFFSET,
                EGG::Vector3f::inf, KCL_TYPE_VEHICLE_COLLIDEABLE, &info, &flags, 0)) {
        m_floorCollision = true;
        if (info.floorDist > -std::numeric_limits<f32>::min()) {
            m_floorNrm = info.floorNrm;
        }

        // Not m_pos += info.tangentOff - m_floorNrm * 60.0f
        // The former requires m_pos to be the last addition to occur
        // TODO: 100.0f - 10.0f - 30.0f? Why is this the case?
        m_pos.x = m_pos.x + info.tangentOff.x - m_floorNrm.x * 60.0f;
        m_pos.z = m_pos.z + info.tangentOff.z - m_floorNrm.z * 60.0f;
        m_flags.setBit(eFlags::Position);

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
            if (m_color == 0) {
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

/// @addr{0x806CFD48}
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
void ObjectHeyho::calcMotion() {
    if (!m_freeFall) {
        f32 sqVel = m_maxVelSq - m_accel * (m_pos.y - m_midpoint.y);
        if (sqVel <= 0.0f) {
            sqVel = 0.001f;
        }
        m_currentVel = EGG::Mathf::sqrt(sqVel);
        m_railInterpolator->setCurrVel(m_currentVel);

        if (m_railInterpolator->calc() == RailInterpolator::Status::ChangingDirection) {
            // We have an edgecase where the endpoint heights aren't the same
            // While the current velocity reaches 0 at the apex, it's higher on the other side
            // To counter this, the object goes off the rail and into free fall until we land
            m_launchVel = m_currentVel;
            if (m_currentVel > 1.0f) {
                m_freeFall = true;
            }
        }

        if (m_currentStateId == 0 && !m_floorCollision) {
            const auto &curPos = m_railInterpolator->curPos();
            m_pos.x = curPos.x;
            m_pos.z = curPos.z;
            m_flags.setBit(eFlags::Position);
        } else {
            m_pos = m_railInterpolator->curPos();
            m_flags.setBit(eFlags::Position);
        }
    } else {
        m_currentVel -= 1.0f;
        m_pos.y = m_currentVel + m_pos.y;
        m_flags.setBit(eFlags::Position);
    }

    // m_currentVel < 0.0f => either we're in free fall or we just snapped to the rail
    // If we would land back on the rail on the next frame, just let it snap to the rail instead
    if (m_currentVel < 0.0f &&
            EGG::Mathf::abs(m_pos.y - m_railInterpolator->curPos().y) <= -m_currentVel) {
        m_currentVel = m_launchVel;
        m_freeFall = false;
    }
}

/// @addr{0x806CFFB0}
void ObjectHeyho::calcInterp() {
    m_up = Interpolate(0.2f, m_up, m_floorNrm);
    m_up.normalise2();
    m_forward.normalise2();
    setMatrixTangentTo(m_up, m_forward);
}

} // namespace Field
