#include "game/field/obj/ObjectKoopaFigure64.hh"

#include "game/field/ObjectDirector.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x806DA914}
ObjectKoopaFigure64::ObjectKoopaFigure64(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_isBigStatue(params.setting(1) == 1),
      m_startDelay(static_cast<s32>(params.setting(2))) {}

/// @addr{0x806DB114}
ObjectKoopaFigure64::~ObjectKoopaFigure64() = default;

/// @addr{0x806DAA44}
void ObjectKoopaFigure64::init() {
    constexpr f32 BIG_SCALE = 20.0f;

    if (m_isBigStatue) {
        const auto &flowTable = ObjectDirector::Instance()->flowTable();
        const auto *collisionSet = flowTable.set(flowTable.slot(id()));
        ASSERT(collisionSet);
        s16 radius = parse<s16>(collisionSet->params.cylinder.radius);
        resize(BIG_SCALE * static_cast<f32>(radius), 0.0f);
    }

    m_cycleFrame = FIRE_DURATION + COOLDOWN_DURATION;

    if (m_isBigStatue) {
        disableCollision();
    }
}

/// @addr{0x806DAB5C}
void ObjectKoopaFigure64::calc() {
    u32 timer = System::RaceManager::Instance()->timer();
    if (timer < static_cast<u32>(m_startDelay)) {
        return;
    }

    bool endOfCycle = CYCLE_DURATION == m_cycleFrame;
    if (endOfCycle) {
        m_cycleFrame = 0;

        if (m_isBigStatue) {
            enableCollision();
        }
    }

    if (m_cycleFrame >= FIRE_DURATION && !endOfCycle && m_isBigStatue) {
        disableCollision();
    }

    ++m_cycleFrame;
}

/// @addr{0x806DAFB8}
void ObjectKoopaFigure64::calcCollisionTransform() {
    constexpr EGG::Vector3f FIRE_POS_OFFSET = EGG::Vector3f(0.0f, -280.0f, 4000.0f);

    calcTransform();
    EGG::Matrix34f rotMat = m_transform;
    rotMat.setBase(3, EGG::Vector3f::zero);

    EGG::Matrix34f targetMat = EGG::Matrix34f::ident;
    targetMat.setBase(3, FIRE_POS_OFFSET);

    EGG::Matrix34f posMat = EGG::Matrix34f::ident;
    posMat.setBase(3, m_pos);

    m_collision->transform(posMat.multiplyTo(rotMat.multiplyTo(targetMat)), m_scale);
}

} // namespace Field
