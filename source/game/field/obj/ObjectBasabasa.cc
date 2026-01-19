#include "ObjectBasabasa.hh"

#include "game/field/ObjectDirector.hh"
#include "game/field/RailManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"

namespace Field {

/// @brief Scoped within the TU so that ObjectBasabasa can set and ObjectBasabasaDummy can access.
static f32 s_initialX;
static f32 s_initialY;

/// @addr{0x806B5C84}
ObjectBasabasaDummy::ObjectBasabasaDummy(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), StateManager(this, STATE_ENTRIES),
      m_bigBump(params.setting(7) == 1) {
    m_active = true;
}

/// @addr{0x806B7630}
ObjectBasabasaDummy::~ObjectBasabasaDummy() = default;

/// @addr{0x806B5E80}
void ObjectBasabasaDummy::init() {
    m_railInterpolator->init(0.0f, 0);
    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);

    auto &rng = System::RaceManager::Instance()->random();
    rng.next();
    f32 y = rng.getF32(s_initialY);
    f32 x = rng.getF32(s_initialX);

    m_initialPos = EGG::Vector3f(x - s_initialX * 0.5f, y, 0.0f);

    m_nextStateId = 0;
}

/// @addr{0x806B6874}
Kart::Reaction ObjectBasabasaDummy::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    auto course = System::RaceConfig::Instance()->raceScenario().course;
    if (course == Course::Dry_Dry_Ruins && m_bigBump) {
        const auto &hitTable = ObjectDirector::Instance()->hitTableKart();
        reactionOnKart = hitTable.reaction(hitTable.slot(ObjectId::BasabasaDummy));
    }

    return reactionOnKart;
}

/// @addr{0x806B6100}
void ObjectBasabasaDummy::calcState0() {
    if (!m_active) {
        return;
    }

    if (m_railInterpolator->calc() == RailInterpolator::Status::ChangingDirection) {
        m_active = false;
        unregisterCollision();
    } else {
        setMatrixFromOrthonormalBasisAndPos(m_railInterpolator->curTangentDir());
        calcTransform();

        EGG::Matrix34f mat = m_transform;
        mat.setBase(3, EGG::Vector3f::zero);
        m_pos = m_railInterpolator->curPos() + mat.ps_multVector(m_initialPos);
        m_flags.setBit(eFlags::Position);
    }
}

/// @addr{0x806B70D0}
ObjectBasabasa::ObjectBasabasa(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_initialTimer(static_cast<s32>(params.setting(1))),
      m_batsPerGroup(static_cast<s32>(params.setting(2))),
      m_startFrame(static_cast<s32>(params.setting(6))),
      m_batSpacing(static_cast<s32>(static_cast<f32>(params.setting(5)) /
              static_cast<f32>(params.setting(0)) / static_cast<f32>(m_batsPerGroup))) {
    f32 railLen = RailManager::Instance()->rail(params.pathId())->getPathLength();
    u32 groupCount = static_cast<u32>(railLen / static_cast<f32>(params.setting(0)) /
                             static_cast<f32>(m_initialTimer)) +
            1;
    u32 batCount = groupCount * static_cast<u32>(m_batsPerGroup);

    m_bats = std::span<ObjectBasabasaDummy *>(new ObjectBasabasaDummy *[batCount], batCount);

    for (auto *&bat : m_bats) {
        bat = new ObjectBasabasaDummy(params);
        bat->load();
    }

    s_initialX = static_cast<f32>(params.setting(3));
    s_initialY = static_cast<f32>(params.setting(4));
}

/// @addr{0x806B72F4}
ObjectBasabasa::~ObjectBasabasa() {
    delete[] m_bats.data();
}

/// @addr{0x806B7334}
void ObjectBasabasa::init() {
    for (auto *&bat : m_bats) {
        if (bat->active()) {
            bat->setActive(false);
            bat->unregisterCollision();
        }
    }

    m_cycleTimer = m_initialTimer;
    m_batsActive = 0;
}

/// @addr{0x806B74C4}
void ObjectBasabasa::calc() {
    if (System::RaceManager::Instance()->timer() <= static_cast<u32>(m_startFrame)) {
        return;
    }

    if (m_cycleTimer == m_initialTimer + m_batSpacing * (m_batsActive % m_batsPerGroup)) {
        auto *&bat = m_bats[m_batsActive++];
        bat->init();
        bat->setActive(true);
        bat->loadAABB(0.0f);

        if ((m_batsActive % m_batsPerGroup) == 0) {
            m_cycleTimer = m_batSpacing * m_batsPerGroup;
        }
    }

    m_batsActive %= m_bats.size();

    ++m_cycleTimer;
}

} // namespace Field
