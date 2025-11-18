#include "ObjectHeyhoBall.hh"

#include "game/field/ObjectDirector.hh"
#include "game/field/ObjectFlowTable.hh"

namespace Field {

/// @addr{0x806D02C4}
ObjectHeyhoBall::ObjectHeyhoBall(const System::MapdataGeoObj &params)
    : ObjectProjectile(params), StateManager(this), m_airtime(static_cast<f32>(params.setting(1))),
      m_initPos(params.pos()) {
    registerManagedObject();
}

/// @addr{0x806D1820}
ObjectHeyhoBall::~ObjectHeyhoBall() = default;

/// @addr{0x806D05F0}
void ObjectHeyhoBall::init() {
    constexpr f32 RADIUS = 1500.0f;

    m_nextStateId = 0;
    m_shipPos.setZero();
    m_workingPos = m_pos;
    m_intensity = ExplosionIntensity::ExplosionLoseItem;

    resize(RADIUS, 0.0f);

    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    m_blastRadiusRatio = RADIUS /
            static_cast<f32>(parse<s16>(flowTable.set(flowTable.slot(id()))->params.sphere.radius));
}

/// @addr{0x806D0780}
void ObjectHeyhoBall::calc() {
    StateManager::calc();

    m_flags.setBit(eFlags::Position);
    m_pos = m_workingPos;
}

/// @addr{0x806D0880}
Kart::Reaction ObjectHeyhoBall::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction /*reactionOnKart*/, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f &hitDepth) {
    if (m_currentStateId == 3) {
        if (m_intensity == ExplosionIntensity::ExplosionLoseItem) {
            return Kart::Reaction::ExplosionLoseItem;
        }

        hitDepth.setZero();
        return Kart::Reaction::SpinSomeSpeed;
    }

    return Kart::Reaction::WallAllSpeed;
}

/// @addr{0x806D10A4}
void ObjectHeyhoBall::initProjectile(const EGG::Vector3f &pos) {
    m_shipPos = pos;
    m_xzDir = (m_initPos + EGG::Vector3f::ey * -50.0f) - m_shipPos;
    m_xzDir.y = 0.0f;
    m_xzDir.normalise2();

    EGG::Vector2f xzDist = EGG::Vector2f(m_shipPos.x - m_initPos.x, m_shipPos.z - m_initPos.z);
    m_xzSpeed = EGG::Mathf::sqrt(xzDist.dot()) / m_airtime;
    m_yDist = m_initPos.y + -50.0f - m_shipPos.y;
    m_initYSpeed = m_yDist / m_airtime + 0.5f * 4.0f * m_airtime;
}

/// @addr{0x806D0A00}
void ObjectHeyhoBall::enterState0() {}

/// @addr{0x806D0A3C}
void ObjectHeyhoBall::enterState1() {
    constexpr f32 RADIUS = 1500.0f;

    if (!getUnit()) {
        loadAABB(0.0f);
        resize(RADIUS, 0.0f);
    }
}

/// @addr{0x806D0C0C}
void ObjectHeyhoBall::enterState2() {
    m_workingPos = m_initPos + EGG::Vector3f::ey * -50.0f;
}

/// @addr{0x806D0D84}
void ObjectHeyhoBall::enterState3() {
    m_scaleChangeRate = (1.2f * m_blastRadiusRatio - 1.0f) / 40.0f / 40.0f;
}

/// @addr{0x806D0A14}
void ObjectHeyhoBall::calcState0() {
    m_workingPos = m_shipPos;
}

/// @addr{0x806D0AD8}
void ObjectHeyhoBall::calcState1() {
    f32 currentHeight = m_workingPos.y + (m_initYSpeed - 4.0f * static_cast<f32>(m_currentFrame));
    if (currentHeight > m_initPos.y + -50.0f) {
        m_workingPos.y = currentHeight;
        m_workingPos.x += m_xzDir.x * m_xzSpeed;
        m_workingPos.z += m_xzDir.z * m_xzSpeed;
    } else {
        m_nextStateId = 2;
        m_workingPos = ((m_workingPos + m_initPos) + EGG::Vector3f::ey * -50.0f) * 0.5f;
    }
}

/// @addr{0x806D0CD8}
void ObjectHeyhoBall::calcState2() {
    constexpr u32 REST_FRAMES = 180;

    if (m_currentFrame >= REST_FRAMES) {
        m_nextStateId = 3;
    }
}

/// @addr{0x806D0F24}
void ObjectHeyhoBall::calcState3() {
    if (m_currentFrame >= 46) {
        m_flags.setBit(eFlags::Scale);
        m_scale = EGG::Vector3f(1.001f, 1.001f, 1.001f);

        if (getUnit()) {
            unregisterCollision();
        }

        m_nextStateId = 0;
        m_intensity = ExplosionIntensity::ExplosionLoseItem;
    } else {
        f32 shrinkFrames = static_cast<f32>(m_currentFrame) - 40.0f;
        f32 scale = 1.2f * m_blastRadiusRatio + -m_scaleChangeRate * shrinkFrames * shrinkFrames;

        scale = std::min(m_blastRadiusRatio, scale);
        m_flags.setBit(eFlags::Scale);
        m_scale = EGG::Vector3f(scale, scale, scale);

        m_intensity = static_cast<ExplosionIntensity>(m_currentFrame >= 32);
    }
}

} // namespace Field
