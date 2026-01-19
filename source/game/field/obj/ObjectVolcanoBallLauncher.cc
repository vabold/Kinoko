#include "ObjectVolcanoBallLauncher.hh"

#include "game/field/RailManager.hh"
#include "game/field/obj/ObjectVolcanoBall.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x806E3458}
ObjectVolcanoBallLauncher::ObjectVolcanoBallLauncher(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_initDelay(static_cast<f32>(params.setting(1))),
      m_cycleDuration(static_cast<f32>(params.setting(2))) {
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
    EGG::Vector3f vel = dir * static_cast<f32>(params.setting(0));
    f32 accel = vel.y * vel.y / (2.0f * (pos - points[0].pos.y));

    f32 endPosY = points.back().pos.y;
    f32 t1 = -1.0f;
    f32 t2 = -1.0f;

    // Solve standard kinematic equation to find the time it takes for the fireball to land.
    EGG::Mathf::FindRootsQuadratic(0.5f * accel, -vel.y, endPosY - points[0].pos.y, t1, t2);
    f32 fallTime = (t2 > 0.0f) ? t2 : (t1 > 0.0f) ? t1 : -1.0f;

    f32 finalVel = 2.0f * accel * (pos - endPosY);
    f32 burnDuration = static_cast<f32>(params.setting(3));
    s32 ballCount = static_cast<s32>((fallTime + burnDuration) / m_cycleDuration) + 2;

    m_balls = std::span<ObjectVolcanoBall *>(new ObjectVolcanoBall *[ballCount], ballCount);

    for (auto *&ball : m_balls) {
        ball = new ObjectVolcanoBall(accel, finalVel, endPosY, params, vel);
        ball->load();
    }

    m_active = false;
}

/// @addr{0x806E384C}
ObjectVolcanoBallLauncher::~ObjectVolcanoBallLauncher() {
    delete[] m_balls.data();
}

/// @addr{0x806E388C}
void ObjectVolcanoBallLauncher::init() {
    for (auto *&ball : m_balls) {
        ball->init();
        ball->m_nextStateId = 0;
    }

    m_currBallIdx = 0;
}

/// @addr{0x806E3920}
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

} // namespace Field
