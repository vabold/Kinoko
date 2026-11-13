#include "ObjectBasabasa.hh"

#include "game/field/ObjectDirector.hh"
#include "game/field/RailManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x806B5C84}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Uses param setting 8 to determine if collisions with the bat should result in a
/// larger bump effect.
/// @note Param setting 8 only grants a larger bump effect on Dry Dry Ruins. Bats on custom tracks
/// which use a different slot will not be affected by this setting and will have the smaller bump
/// effect.
ObjectBasabasaDummy::ObjectBasabasaDummy(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_bigBump(params.setting(7) == 1) {
    m_active = true;
}

/// @addr{0x806B7630}
/// @brief Default virtual destructor
ObjectBasabasaDummy::~ObjectBasabasaDummy() = default;

/// @addr{0x806B5E80}
/// @copybrief ObjectBase::init()
/// @details Snaps the bat to the beginning of its rail. Calculates the bat's spawn position
/// offset by advancing RNG.
void ObjectBasabasaDummy::init() {
    m_railInterpolator->init(0.0f, 0);
    setPos(m_railInterpolator->curPos());

    auto &rng = System::RaceManager::Instance()->random();
    rng.next();
    f32 y = rng.getF32(ObjectBasabasa::InitialYRange());
    f32 x = rng.getF32(ObjectBasabasa::InitialXRange());

    m_posOffset = EGG::Vector3f(x - ObjectBasabasa::InitialXRange() * 0.5f, y, 0.0f);
}

/// @addr{0x806B602C}
/// @copybrief ObjectBase::calc()
/// @details Simply calls @ref calcState().
/// @note Normally this class inherits from @ref StateManager and this function calls into @ref
/// StateManager::calc() "StateManager::calc()". However, the bats second state is only used when
/// the bat is hit with a star or a shock. For the purposes of Kinoko, we can simplify here and just
/// call @ref calcState().
void ObjectBasabasaDummy::calc() {
    calcState();
}

/// @addr{0x806B7700}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc and @ref eLoadFlags::Draw so that the object is
/// calculated every frame
ObjectBase::LoadFlags ObjectBasabasaDummy::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc, eLoadFlags::Draw);
}

/// @addr{0x806B76F4}
/// @copybrief ObjectBase::getKclName()
/// @return Retuns `"basabasa"` so that the individual bat's collision is used
const char *ObjectBasabasaDummy::getKclName() const {
    return "basabasa";
}

/// @addr{0x806B6874}
/// @copydoc ObjectCollidable::onCollision()
/// @details Adjusts the reaction based on the course and whether the bat has a big bump (indicated
/// by @ref m_bigBump).
/// @note @ref m_bigBump only has an effect on Dry Dry Ruins. Bats on custom tracks which use a
/// different slot will not be affected by this setting and will have the smaller bump effect.
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
/// @brief While spawned, calculates the bat's position based off the @ref RailInterpolator
/// @details If the bat is inactive (despawned), then early returns. Updates the bat's rail
/// interpolator. If the bat has reached the end of the rail, sets @ref m_active to `false` and
/// disables its collision. Otherwise, updates its position and transformation based on the rail
/// interpolator and initial position.
void ObjectBasabasaDummy::calcState() {
    if (!m_active) {
        return;
    }

    if (m_railInterpolator->calc() == RailInterpolator::Status::ChangingDirection) {
        m_active = false;
        unregisterCollision();
    } else {
        setMatrixFromOrthonormalBasisAndPos(m_railInterpolator->curTangentDir());
        calcTransform();

        EGG::Matrix34f mat = transform();
        mat.setBase(3, EGG::Vector3f::zero);
        setPos(m_railInterpolator->curPos() + mat.ps_multVector(m_posOffset));
    }
}

/// @addr{0x806B70D0}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Sets @ref m_initialTimer based on param setting 2, @ref m_batsPerGroup based on param
/// setting 3, and @ref m_startFrame based on param setting 7. Computes @ref m_batSpacing based on
/// param setting 6 divided by the speed of the bats (defined by param setting 1) divided by @ref
/// m_batsPerGroup. Computes the number of bat groups based on the rail length, speed of the bats
/// (defined by param setting 1) and initial timer. Based on the number of groups and @ref
/// m_batsPerGroup, constructs and loads the underlying @ref ObjectBasabasaDummy objects. Finally,
/// sets @ref s_initialXRange and @ref s_initialYRange based on param settings 4 and 5 respectively.
/// @warning If param setting 1 or param setting 3 is zero, this will cause division by zero when
/// computing @ref m_batSpacing and crash the game.
ObjectBasabasa::ObjectBasabasa(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      m_initialTimer(params.setting(1)),
      m_batsPerGroup(params.setting(2)),
      m_startFrame(params.setting(6)),
      m_batSpacing(static_cast<u32>(static_cast<f32>(params.setting(5)) /
              static_cast<f32>(params.setting(0)) / static_cast<f32>(m_batsPerGroup))) {
    f32 railLen = RailManager::Instance()->rail(params.pathId())->getPathLength();
    f32 speed = static_cast<f32>(params.setting(0));
    u32 groupCount = static_cast<u32>(railLen / speed / static_cast<f32>(m_initialTimer)) + 1;

    m_bats = owning_span<ObjectBasabasaDummy *>(groupCount * m_batsPerGroup);
    for (auto *&bat : m_bats) {
        bat = EGG::egg_new<ObjectBasabasaDummy>(params);
        bat->load();
    }

    s_initialXRange = params.setting(3);
    s_initialYRange = params.setting(4);
}

/// @addr{0x806B72F4}
/// @brief Default virtual destructor
ObjectBasabasa::~ObjectBasabasa() = default;

/// @addr{0x806B7334}
/// @copybrief ObjectBase::init()
/// @details Initializes all bats to be inactive with disabled collision and resets the spawner's
/// cycle timer and active bat count.
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
/// @copybrief ObjectBase::calc()
/// @details Spawns bats according to the @ref m_cycleTimer "cycle timer". Bats do not start
/// spawning until @ref m_startFrame frames have elapsed since the countdown started.
void ObjectBasabasa::calc() {
    if (System::RaceManager::Instance()->timer() <= m_startFrame) {
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

/// @addr{0x806B7628}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc and @ref eLoadFlags::Draw so that the object is
/// calculated every frame
ObjectBase::LoadFlags ObjectBasabasa::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc, eLoadFlags::Draw);
}

/// @addr{0x806B761C}
/// @copybrief ObjectBase::createCollision()
/// @details No-op since the spawner itself does not have any collision
void ObjectBasabasa::createCollision() {}

/// @addr{0x806B7620}
/// @copybrief ObjectBase::loadRail()
/// @details No-op since the spawner itself does not have any rail to load
void ObjectBasabasa::loadRail() {}

f32 ObjectBasabasa::s_initialXRange;
f32 ObjectBasabasa::s_initialYRange;

} // namespace Kinoko::Field
