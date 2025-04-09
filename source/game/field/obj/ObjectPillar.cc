
#include "ObjectPillar.hh"

#include "game/field/ObjectDirector.hh"

namespace Field {

/// @addr{Inlined in 0x807FED80}
ObjectPillarBase::ObjectPillarBase(const System::MapdataGeoObj &params) : ObjectKCL(params) {}

/// @addr{0x807FFAA0}
ObjectPillarBase::~ObjectPillarBase() = default;

/// @addr{0x807FEB68}
ObjectPillarC::ObjectPillarC(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_fallStart(static_cast<u32>(params.setting(0))) {}

/// @addr{0x807FFAE0}
ObjectPillarC::~ObjectPillarC() = default;

/// @addr{0x807FEC30}
void ObjectPillarC::calcCollisionTransform() {
    constexpr f32 HEIGHT = 1900.0f;

    if (!m_collision) {
        return;
    }

    EGG::Matrix34f mat = EGG::Matrix34f::zero;
    mat.makeT(EGG::Vector3f(0.0f, 2.0f * (HEIGHT * m_scale.y) / 3.0f, 0.0f));

    calcTransform();

    EGG::Vector3f speed = m_transform.ps_multVector(EGG::Vector3f::ez * 1000.0f);
    m_collision->transform(m_transform.multiplyTo(mat), m_scale, speed);
}

/// @addr0x807FED80}
ObjectPillar::ObjectPillar(const System::MapdataGeoObj &params)
    : ObjectKCL(params), m_state(State::Upright), m_fallStart(static_cast<u32>(params.setting(0))),
      m_fallRotation(static_cast<f32>(params.setting(1)) * DEG2RAD) {
    m_startRot = m_rot;
    m_base = new ObjectPillarBase(params);
    m_collidable = new ObjectPillarC(params);

    m_base->load();
    m_collidable->load();

    m_fallFrame = std::numeric_limits<s32>::max();
}

/// @addr{0x807FFA34}
ObjectPillar::~ObjectPillar() = default;

/// @addr{0x807FEFD8}
void ObjectPillar::init() {
    ObjectBase::init();

    m_collidable->disableCollision();
    m_startRot = m_rot;
    disableCollision();
}

/// @addr{0x807FF17C}
void ObjectPillar::calc() {
    u32 time = System::RaceManager::Instance()->timer();

    if (m_state == State::Upright && time < m_fallStart) {
        m_collidable->enableCollision();
    } else if (m_state == State::Upright && time == m_fallStart) {
        // The pillar has now started to fall.
        m_state = State::Falling;
    } else if (m_state == State::Falling) {
        f32 rot = calcRot(static_cast<s32>(time));
        if (rot < m_fallRotation) {
            m_transform = getUpdatedMatrix(0);
            m_pos = m_transform.base(3);

            m_collidable->setTransform(m_transform);
            m_collidable->setPos(m_pos);

            m_flags.setBit(eFlags::Matrix, eFlags::Rotation);
            m_rot = EGG::Vector3f(rot, m_startRot.y, m_startRot.z);
        } else {
            // The pillar has finished falling.
            // We can now drive on top of the base and on the pillar itself.
            m_state = State::Landed;

            m_collidable->disableCollision();
            enableCollision();
            m_fallFrame = time;
        }

    } else {
        m_state = State::Landed;
    }
}

/// @addr{0x807FF83C}
const EGG::Matrix34f &ObjectPillar::getUpdatedMatrix(u32 timeOffset) {
    f32 rot = calcRot(System::RaceManager::Instance()->timer() - timeOffset);
    m_workMat.makeRT(EGG::Vector3f(rot, m_startRot.y, m_startRot.z), m_pos);
    return m_workMat;
}

/// @addr{0x807FF90C}
f32 ObjectPillar::calcRot(s32 frame) const {
    constexpr f32 STEP = 1e-7f;

    if (m_fallFrame < frame) {
        return m_fallRotation;
    }

    frame -= m_fallStart;
    return std::min(m_fallRotation, m_startRot.x + STEP * static_cast<f32>(frame * frame * frame));
}

} // namespace Field
