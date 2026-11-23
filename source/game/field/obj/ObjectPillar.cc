
#include "ObjectPillar.hh"

#include "game/field/ObjectCollisionBase.hh"

#include "game/kart/Reaction.hh"

#include "game/system/RaceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{Inlined in 0x807FED80}
/// @copybrief ObjectKCL::ObjectKCL(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
ObjectPillarBase::ObjectPillarBase(const System::MapdataGeoObj &params) : ObjectKCL(params) {}

/// @addr{0x807FFAA0}
/// @brief Default virtual destructor
ObjectPillarBase::~ObjectPillarBase() = default;

/// @addr{0x807FFA94}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the pillar base, `dc_pillar_base`
const char *ObjectPillarBase::getKclName() const {
    return "dc_pillar_base";
}

/// @addr{0x807FEB68}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes @ref m_fallStart based on param setting 1.
ObjectPillarC::ObjectPillarC(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_fallStart(static_cast<u32>(params.setting(0))) {}

/// @addr{0x807FFAE0}
/// @brief Default virtual destructor
ObjectPillarC::~ObjectPillarC() = default;

/// @addr{0x807FEC30}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details Extends the falling pillar's collision transform upwards and in the direction of its
/// fall.
void ObjectPillarC::calcCollisionTransform() {
    constexpr f32 HEIGHT = 1900.0f;

    if (!m_collision) {
        return;
    }

    EGG::Matrix34f mat = EGG::Matrix34f::zero;
    mat.makeT(EGG::Vector3f(0.0f, 2.0f * (HEIGHT * scale().y) / 3.0f, 0.0f));

    calcTransform();

    EGG::Vector3f speed = transform().ps_multVector(EGG::Vector3f::ez * 1000.0f);
    m_collision->transform(transform().multiplyTo(mat), scale(), speed);
}

/// @addr{0x807FFA74}
/// @copybrief ObjectBase::getCollisionRadius()
/// @return The collision radius of the falling pillar, `3000.0f`.
f32 ObjectPillarC::getCollisionRadius() const {
    return 3000.0f;
}

/// @addr{0x807FFA80}
/// @copybrief ObjectBase::id()
/// @return The ID of the falling pillar object, @ref ObjectId::DCPillarC
ObjectId ObjectPillarC::id() const {
    return ObjectId::DCPillarC;
}

/// @addr{0x807FED5C}
/// @copybrief ObjectCollidable::onCollision()
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @return @ref Kart::Reaction::WallAllSpeed if the pillar has not started falling, otherwise @ref
/// Kart::Reaction::LaunchSpin
Kart::Reaction ObjectPillarC::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    auto *raceMgr = System::RaceManager::Instance();
    return raceMgr->timer() < m_fallStart ? Kart::Reaction::WallAllSpeed : reactionOnKart;
}

/// @addr{0x807FED80}
/// @copybrief ObjectKCL::ObjectKCL(const System::MapdataGeoObj &)
/// @details Initializes the pillar to the @ref State::Upright state. Sets @ref m_fallStart based on
/// param setting 1 and sets @ref m_targetRot by converting param setting 2 from degrees to
/// radians. Caches the object's initial X-axis rotation to @ref m_initRotX and initializes @ref
/// m_initRot to zero. Constructs and loads the pillar base (@ref ObjectPillarBase) and the
/// upright/falling pillar collision object (@ref ObjectPillarC). Finally, sets @ref
/// m_groundFrame to the maximum possible value.
ObjectPillar::ObjectPillar(const System::MapdataGeoObj &params)
    : ObjectKCL(params),
      m_state(State::Upright),
      m_fallStart(static_cast<u32>(params.setting(0))),
      m_targetRot(F_PI * static_cast<f32>(params.setting(1)) / 180.0f),
      m_initRotX(rot().x),
      m_initRot(EGG::Vector3f::zero) {
    m_base = EGG::egg_new<ObjectPillarBase>(params);
    m_collidable = EGG::egg_new<ObjectPillarC>(params);

    m_base->load();
    m_collidable->load();

    m_groundFrame = std::numeric_limits<s32>::max();
}

/// @addr{0x807FFA34}
/// @brief Default virtual destructor
ObjectPillar::~ObjectPillar() = default;

/// @addr{0x807FEFD8}
/// @copybrief ObjectBase::init()
/// @details Calls @ref ObjectBase::init(). Disables collision for @ref ObjectPillarC and this
/// object (since it has not yet fallen). Finally, caches the initial rotation to @ref
/// m_initRot.
void ObjectPillar::init() {
    ObjectBase::init();

    m_collidable->disableCollision();
    m_initRot = rot();
    disableCollision();
}

/// @addr{0x807FF17C}
/// @copybrief ObjectBase::calc()
/// @details Checks the current race duration to update the pillar's fall state and rotation. If the
/// pillar is upright, then the @ref ObjectPillarC collision is enabled. If the pillar is falling,
/// then the rotation is updated and the collision transform is updated. If the pillar has finished
/// falling, then the @ref ObjectPillarC collision is disabled, the @ref ObjectPillar collision
/// is enabled, and the current framecount is saved to @ref m_groundFrame.
void ObjectPillar::calc() {
    u32 time = System::RaceManager::Instance()->timer();

    if (m_state == State::Upright && time < m_fallStart) {
        m_collidable->enableCollision();
    } else if (m_state == State::Upright && time == m_fallStart) {
        // The pillar has now started to fall.
        m_state = State::Break;
    } else if (m_state == State::Break) {
        f32 rot = calcRot(static_cast<s32>(time));
        if (rot < m_targetRot) {
            setTransform(getUpdatedMatrix(0));
            m_collidable->setTransform(transform());

            setRot(EGG::Vector3f(rot, m_initRot.y, m_initRot.z));
        } else {
            // The pillar has finished falling.
            // We can now drive on top of the base and on the pillar itself.
            m_state = State::Ground;

            m_collidable->disableCollision();
            enableCollision();
            m_groundFrame = time;
        }

    } else {
        m_state = State::Ground;
    }
}

/// @addr{0x807FFA2C}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame
ObjectBase::LoadFlags ObjectPillar::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x807FF980}
/// @copybrief ObjectKCL::colRadiusAdditionalLength()
/// @return Additional length to be added to the collision radius of the pillar, `4000.0f`
f32 ObjectPillar::colRadiusAdditionalLength() const {
    return 4000.0f;
}

/// @addr{0x807FF83C}
/// @copybrief ObjectKCL::getUpdatedMatrix()
/// @param timeOffset The time offset used to calculate the current frame's transformation
/// @return Const ref to the updated transformation matrix for the current frame, taking into
/// account the time offset
const EGG::Matrix34f &ObjectPillar::getUpdatedMatrix(u32 timeOffset) {
    f32 rot = calcRot(System::RaceManager::Instance()->timer() - timeOffset);
    m_workMat.makeRT(EGG::Vector3f(rot, m_initRot.y, m_initRot.z), pos());
    return m_workMat;
}

/// @addr{0x807FF90C}
/// @brief Calculates the pillar's rotation based on the number of frames elapsed in the race
/// @param frame The current framecount of the race
/// @return The current rotation of the pillar in radians
/// @details If the pillar has already fallen, then returns @ref m_targetRot. Otherwise, the
/// rotation is computed as:
/// \f[ rot = \min(m\_targetRot,\ m\_initRotX + 10^{-7} \cdot (frame - m\_fallStart)^3) \f]
f32 ObjectPillar::calcRot(s32 frame) const {
    constexpr f32 STEP = 1e-7f;

    if (m_groundFrame < frame) {
        return m_targetRot;
    }

    frame -= m_fallStart;
    return std::min(m_targetRot, m_initRotX + STEP * static_cast<f32>(frame * frame * frame));
}

} // namespace Kinoko::Field
