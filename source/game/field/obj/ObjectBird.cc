#include "ObjectBird.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x8077BD80}
ObjectBird::ObjectBird(const System::MapdataGeoObj &params) : ObjectCollidable(params) {
    m_leader = new ObjectBirdLeader(params, this);
    m_leader->load();

    u32 count = static_cast<u32>(params.setting(1));
    if (count == 0) {
        count = 5;
    }

    m_followers = std::span<ObjectBirdFollower *>(new ObjectBirdFollower *[count], count);

    for (u32 i = 0; i < count; ++i) {
        auto *bird = new ObjectBirdFollower(params, this, i);
        m_followers[i] = bird;
        bird->load();
    }
}

/// @addr{0x8077CDC8}
ObjectBird::~ObjectBird() {
    delete[] m_followers.data();
}

/// @addr{0x8077BFC8}
void ObjectBird::calc() {
    constexpr f32 MIN_SPACING = 300.0f;

    for (u32 i = 0; i < m_followers.size() - 1; ++i) {
        const EGG::Vector3f &firstPos = m_followers[i]->pos();

        for (u32 j = i + 1; j < m_followers.size(); ++j) {
            const EGG::Vector3f &secondPos = m_followers[j]->pos();
            EGG::Vector3f posDelta = firstPos - secondPos;
            f32 len = posDelta.length();

            if (len >= MIN_SPACING) {
                continue;
            }

            posDelta.normalise();
            m_followers[j]->setPos(secondPos - posDelta * (MIN_SPACING - len));
        }
    }
}

/// @addr{0x8077C2F4}
ObjectBirdLeader::ObjectBirdLeader(const System::MapdataGeoObj &params, ObjectBird *bird)
    : ObjectCollidable(params), m_bird(bird) {}

/// @addr{0x8077CE48}
ObjectBirdLeader::~ObjectBirdLeader() = default;

/// @addr{0x8077C384}
void ObjectBirdLeader::init() {
    auto *anmMgr = m_drawMdl->anmMgr();
    anmMgr->playAnim(0.0f, 1.0f, 0);
    f32 frameCount = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount();

    auto &rand = System::RaceManager::Instance()->random();
    f32 rate = rand.getF32(static_cast<f32>(frameCount));
    anmMgr->playAnim(rate, 1.0f, 0);

    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->calc();
    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);
    ASSERT(m_mapObj);
    m_railInterpolator->setCurrVel(static_cast<f32>(m_mapObj->setting(0)));
}

/// @addr{0x8077C504}
void ObjectBirdLeader::calc() {
    m_railInterpolator->calc();
    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);
}

/// @addr{0x8077CC78}
void ObjectBirdLeader::loadAnims() {
    std::array<const char *, 1> names = {{
            "flying",
    }};

    std::array<Render::AnmType, 1> types = {{
            Render::AnmType::Chr,
    }};

    linkAnims(names, types);
}

/// @addr{0x8077C580}
ObjectBirdFollower::ObjectBirdFollower(const System::MapdataGeoObj &params, ObjectBird *bird,
        u32 idx)
    : ObjectBirdLeader(params, bird), m_idx(idx) {}

/// @addr{0x8077CE88}
ObjectBirdFollower::~ObjectBirdFollower() = default;

/// @addr{0x8077C5E0}
void ObjectBirdFollower::init() {
    constexpr f32 POS_DELTA_RANGE = 1000.0f;
    constexpr f32 POS_DELTA_CENTER = 500.0f;

    auto *anmMgr = m_drawMdl->anmMgr();
    anmMgr->playAnim(0.0f, 1.0f, 0);
    f32 frameCount = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount();

    auto &rand = System::RaceManager::Instance()->random();
    f32 rate = rand.getF32(static_cast<f32>(frameCount));
    anmMgr->playAnim(rate, 1.0f, 0);

    ASSERT(m_mapObj);
    m_baseSpeed = static_cast<f32>(m_mapObj->setting(0));
    m_velocity = EGG::Vector3f::ez * m_baseSpeed;

    f32 z = rand.getF32(POS_DELTA_RANGE) - POS_DELTA_CENTER;
    f32 y = rand.getF32(POS_DELTA_RANGE) - POS_DELTA_CENTER;
    f32 x = rand.getF32(POS_DELTA_RANGE) - POS_DELTA_CENTER;
    EGG::Vector3f delta = EGG::Vector3f(x, y, z);

    m_pos = m_bird->leader()->pos() + delta;
    m_flags.setBit(eFlags::Position);
}

/// @addr{0x8077C7F0}
void ObjectBirdFollower::calc() {
    calcPos();

    CollisionInfo info;

    if (CollisionDirector::Instance()->checkSphereFull(100.0f, m_pos, EGG::Vector3f::inf,
                KCL_TYPE_FLOOR, &info, nullptr, 0)) {
        m_pos += info.tangentOff;
        m_flags.setBit(eFlags::Position);
    }
}

/// @addr{0x8077C8F4}
void ObjectBirdFollower::calcPos() {
    constexpr f32 MAX_SPEED_FACTOR = 1.2f;

    EGG::Vector3f pos = m_bird->leader()->pos();
    const auto &follower = m_bird->followers();

    for (u32 i = 0; i < follower.size(); ++i) {
        if (i != m_idx) {
            pos += follower[i]->pos();
        }
    }

    EGG::Vector3f posDelta = pos * (1.0f / static_cast<f32>(follower.size())) - m_pos;
    posDelta.normalise();
    posDelta *= 0.5f;

    m_velocity += posDelta;

    if (m_velocity.length() > m_baseSpeed * MAX_SPEED_FACTOR) {
        m_velocity.normalise();
        m_velocity *= m_baseSpeed * MAX_SPEED_FACTOR;
    }

    m_pos += m_velocity;
    m_flags.setBit(eFlags::Position);
}

} // namespace Field
