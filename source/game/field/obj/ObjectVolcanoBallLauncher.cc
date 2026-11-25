#include "ObjectVolcanoBallLauncher.hh"

#include "game/field/Rail.hh"
#include "game/field/RailInterpolator.hh"
#include "game/field/RailManager.hh"
#include "game/field/obj/ObjectVolcanoBall.hh"

#include "game/system/RaceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x806E3458}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Sets @ref m_initDelay based on param setting 2 and @ref m_cycleDuration based on param
/// setting 3. Finds the height of the fireball's apex, which is the first rail point whose first
/// setting is `1`. Computes the initial velocity by scaling the direction from the first rail point
/// to the second rail point by param setting 1. Then computes the acceleration needed for the
/// vertical velocity to reach zero at the apex. Solves the kinematic equation for the time it takes
/// the fireball to fall from the first rail point to the last rail point, and computes the squared
/// velocity at impact from the change in height between the apex and the last rail point. Creates
/// enough @ref ObjectVolcanoBall objects for every fireball whose fall and burn can overlap, plus
/// two extra. Finally, initializes the launcher to inactive.
ObjectVolcanoBallLauncher::ObjectVolcanoBallLauncher(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      m_initDelay(static_cast<f32>(static_cast<s16>(params.setting(1)))),
      m_cycleDuration(static_cast<f32>(static_cast<s16>(params.setting(2)))) {
    const auto *rail = RailManager::Instance()->rail(params.pathId());
    ASSERT(rail);
    const auto &points = rail->points();

    f32 pos = 0.0f;

    for (auto &point : points) {
        if (point.setting[0] == 1) {
            pos = point.pos.y;
            break;
        }
    }

    EGG::Vector3f dir = points[1].pos - points[0].pos;
    dir.normalise2();
    EGG::Vector3f vel = dir * static_cast<f32>(static_cast<s16>(params.setting(0)));
    f32 accel = vel.y * vel.y / (2.0f * (pos - points[0].pos.y));

    f32 endPosY = points.back().pos.y;
    f32 t1 = -1.0f;
    f32 t2 = -1.0f;

    // Solve standard kinematic equation to find the time it takes for the fireball to land.
    EGG::Mathf::FindRootsQuadratic(0.5f * accel, -vel.y, endPosY - points[0].pos.y, t1, t2);
    f32 fallTime = (t2 > 0.0f) ? t2 : (t1 > 0.0f) ? t1 : -1.0f;

    f32 finalVelSq = 2.0f * accel * (pos - endPosY);
    f32 burnDuration = static_cast<f32>(static_cast<s16>(params.setting(3)));
    u32 ballCount = static_cast<u32>((fallTime + burnDuration) / m_cycleDuration) + 2;

    m_balls = owning_span<ObjectVolcanoBall *>(ballCount);

    for (auto *&ball : m_balls) {
        ball = EGG::egg_new<ObjectVolcanoBall>(accel, finalVelSq, endPosY, params, vel);
        ball->load();
    }

    m_active = false;
}

/// @addr{0x806E384C}
/// @brief Default virtual destructor
ObjectVolcanoBallLauncher::~ObjectVolcanoBallLauncher() = default;

/// @addr{0x806E388C}
/// @copybrief ObjectBase::init()
/// @details Initializes all managed fireballs and sets the index of the next fireball to be
/// launched to 0.
void ObjectVolcanoBallLauncher::init() {
    for (auto *&ball : m_balls) {
        ball->init();
        ball->m_nextStateId = 0;
    }

    m_currBallIdx = 0;
}

/// @addr{0x806E3920}
/// @copybrief ObjectBase::calc()
/// @details If the launcher is inactive, then checks to see if @ref m_initDelay frames have elapsed
/// in the race. If so, transitions the first fireball to the falling state and sets @ref m_active
/// to `true`. Otherwise if the launcher is already active, then checks to see if the cycle duration
/// has elapsed and launches the next fireball accordingly.
void ObjectVolcanoBallLauncher::calc() {
    u32 t = System::RaceManager::Instance()->timer();

    if (m_active) {
        t = (t - static_cast<s32>(m_initDelay)) % static_cast<s32>(m_cycleDuration);

        if (static_cast<f32>(t) == m_cycleDuration - 1.0f) {
            m_balls[m_currBallIdx++]->m_nextStateId = 1;
        }
    } else {
        if (static_cast<f32>(t) == m_initDelay) {
            m_balls[m_currBallIdx++]->m_nextStateId = 1;
            m_active = true;
        }
    }

    if (m_currBallIdx == m_balls.size()) {
        m_currBallIdx = 0;
    }
}

/// @addr{0x806E3A70}
/// @copybrief ObjectBase::loadGraphics()
/// @details No-op because the launcher does not have any graphics to load
void ObjectVolcanoBallLauncher::loadGraphics() {}

/// @addr{0x806E3A68}
/// @copybrief ObjectBase::createCollision()
/// @details No-op because the launcher itself does not have any collision
void ObjectVolcanoBallLauncher::createCollision() {}

/// @addr{0x806E3A6C}
/// @copybrief ObjectBase::loadRail()
/// @details No-op because the launcher itself does not follow any rail path
void ObjectVolcanoBallLauncher::loadRail() {}

/// @addr{0x806E3A74}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame
ObjectBase::LoadFlags ObjectVolcanoBallLauncher::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

} // namespace Kinoko::Field
