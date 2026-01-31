#include "ObjectHanachan.hh"

#include "game/field/ObjectDirector.hh"

namespace Field {

/// @addr{0x806F2FE8}
HanachanChainManager::HanachanChainManager(const std::span<const f32> &linkDistances) {
    size_t count = linkDistances.size() + 1;

    m_links = std::span(new SphereLink[count], count);
    m_links[0].initLinkLen(linkDistances[0]);

    for (size_t i = 1; i < count - 1; ++i) {
        m_links[i].initLinkLen(linkDistances[i]);
        m_links[i - 1].setNext(&m_links[i]);
        m_links[i].setPrev(&m_links[i - 1]);
    }

    auto &last = m_links.back();
    auto &secondToLast = m_links[count - 2];
    last.initLinkLen(0.0f);
    secondToLast.setNext(&last);
    last.setPrev(&secondToLast);
}

/// @addr{0x806F31F4}
HanachanChainManager::~HanachanChainManager() {
    delete[] m_links.data();
}

/// @addr{0x806F49BC}
void HanachanChainManager::calc() {
    for (u32 i = 0; i < 2; ++i) {
        for (auto &link : m_links) {
            link.calc();
        }

        for (auto &link : m_links) {
            link.calcPos();
        }
    }

    for (size_t i = 0; i < m_links.size() - 1; ++i) {
        EGG::Vector3f delta = m_links[i].pos() - m_links[i + 1].pos();
        f32 len = m_links[i].linkLen();

        if (delta.squaredLength() - len * len > 0.0f) {
            calcConstraints();
            break;
        }
    }

    for (auto &link : m_links) {
        link.checkCollision();
    }
}

/// @addr{0x806CA5E4}
void ObjectHanachanPart::calcTransformFromUpAndTangent(const EGG::Vector3f &pos,
        const EGG::Vector3f &up, const EGG::Vector3f &tangent) {
    EGG::Matrix34f mat;
    SetRotTangentHorizontal(mat, up, tangent);
    m_flags.setBit(eFlags::Matrix);
    m_transform = mat;
    m_transform.setBase(3, pos);
    m_pos = pos;
}

/// @addr{0x806C7D74}
ObjectHanachanHead::ObjectHanachanHead(const char *name, const EGG::Vector3f &pos,
        const EGG::Vector3f &rot, const EGG::Vector3f &scale)
    : ObjectHanachanPart(name, pos, rot, scale), m_lastPos(EGG::Vector3f::zero) {}

/// @addr{0x806CCB94}
ObjectHanachanHead::~ObjectHanachanHead() = default;

/// @addr{0x806C8450}
void ObjectHanachanHead::calcCollisionTransform() {
    calcTransform();

    EGG::Matrix34f trans = m_transform;
    EGG::Vector3f scaledUp = trans.base(1) * 330.0f * m_scale.y;
    EGG::Vector3f scaledForward = trans.base(2) * 100.0f * m_scale.y;
    trans.setBase(3, m_pos + scaledUp + scaledForward);
    EGG::Vector3f speed = m_pos - m_lastPos;

    m_collision->transform(trans, m_scale, speed);
    m_lastPos = m_pos;
}

ObjectHanachanBody::ObjectHanachanBody(const System::MapdataGeoObj &params, const char *mdlName)
    : ObjectHanachanPart(params), m_mdlName(mdlName), m_lastSegment(false),
      m_lastPos(EGG::Vector3f::zero) {}

ObjectHanachanBody::ObjectHanachanBody(const char *name, const EGG::Vector3f &pos,
        const EGG::Vector3f &rot, const EGG::Vector3f &scale, const char *mdlName)
    : ObjectHanachanPart(name, pos, rot, scale), m_mdlName(mdlName), m_lastSegment(false),
      m_lastPos(EGG::Vector3f::zero) {}

/// @addr{0x806CCAD8}
ObjectHanachanBody::~ObjectHanachanBody() = default;

/// @addr{0x806C8908}
void ObjectHanachanBody::calcCollisionTransform() {
    if (!m_lastSegment) {
        ObjectCollidable::calcCollisionTransform();
        return;
    }

    EGG::Matrix34f trans = m_transform;
    trans.setBase(3, m_pos + trans.base(2) * 80.0f * m_scale.y);
    EGG::Vector3f posDelta = m_pos - m_lastPos;

    m_collision->transform(trans, m_scale, posDelta);
    m_lastPos = m_pos;
}

/// @addr{0x806C8A5C}
ObjectHanachan::ObjectHanachan(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), StateManager(this, STATE_ENTRIES), m_chain(BODY_PART_DISTANCES),
      m_movingVel(static_cast<f32>(static_cast<s16>(params.setting(0)))) {
    constexpr f32 SCALE = 3.0f;
    constexpr EGG::Vector3f SCALE_VEC = EGG::Vector3f(SCALE, SCALE, SCALE);

    auto *&head = headPart();
    head = new ObjectHanachanHead("BossHanachanHead", EGG::Vector3f::zero, EGG::Vector3f::ez,
            EGG::Vector3f::ez);
    head->setScale(SCALE_VEC);

    const auto &mdlNames = ObjectHanachanBody::MDL_NAMES;
    size_t mdlNameCount = ObjectHanachanBody::MDL_NAMES.size();
    auto parts = bodyParts();
    for (size_t i = 0; i < parts.size(); ++i) {
        auto *&part = parts[i];
        part = new ObjectHanachanBody(params, mdlNames[i % mdlNameCount]);
        part->setScale(SCALE_VEC);
    }

    head->load();

    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    for (size_t i = 0; i < parts.size(); ++i) {
        auto *&part = parts[i];
        part->load();

        const auto *collisionSet = flowTable.set(flowTable.slot(part->id()));
        f32 radius = SCALE * static_cast<f32>(parse<s16>(collisionSet->params.sphere.radius));
        part->resize(radius, 0.0f);
    }

    m_partDisplacement[0] = 0.0f;
    for (size_t i = 1; i < m_partDisplacement.size(); ++i) {
        m_partDisplacement[i] = m_partDisplacement[i - 1] + BODY_PART_DISTANCES[i - 1];
    }
}

/// @addr{0x806C9598}
ObjectHanachan::~ObjectHanachan() = default;

/// @addr{0x806C9630}
void ObjectHanachan::init() {
    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setCurrVel(m_movingVel);

    initBody();

    m_still = false;
    m_stillAngVel = 15.0f;
    m_leftMisalignFrame = 0;
    m_right = EGG::Vector3f::ez;
    m_railAlignment = RailAlignment::Unknown;
    m_prevRailAlignment = RailAlignment::Unknown;

    reinterpret_cast<ObjectHanachanBody *>(m_parts.back())->m_lastSegment = true;

    initChain();
}

/// @addr{0x806C9BC0}
void ObjectHanachan::initWalk() {
    setMovingVel();
    m_stillAngVel = 15.0f;
    m_still = false;
    m_leftMisalignFrame = 0;
}

/// @addr{0x806C9D38}
void ObjectHanachan::calcWalk() {
    if (m_still) {
        m_railInterpolator->setCurrVel(0.0f);
        m_nextStateId = 1;
    }

    m_prevRailAlignment = m_railAlignment;
    m_railAlignment = calcRailAlignment();

    clearChain();
    calcRailAlignmentMotion();
    m_chain.calc();

    m_stillAngVel = 15.0f;
}

/// @addr{0x806C9F98}
void ObjectHanachan::calcWait() {
    if (shouldStartMoving()) {
        m_nextStateId = 0;
    }

    clearChain();

    if (m_stillAngVel >= 0.0f) {
        m_stillAngVel -= 0.25f;
    } else {
        m_stillAngVel += 0.25f;
    }

    if (EGG::Mathf::abs(m_stillAngVel) <= 1.0f) {
        m_stillAngVel = 0.0f;
    }

    calcSlowMotion();

    m_chain.calc();
}

/// @addr{0x806CA24C}
void ObjectHanachan::onSegmentEnd() {
    u16 setting = m_railInterpolator->curPoint().setting[0];
    if (setting != 0) {
        m_still = true;
        m_stillDuration = setting;
    }
}

/// @addr{0x806CA27C}
void ObjectHanachan::calcRail() {
    m_right = m_railInterpolator->curTangentDir();

    if (m_railInterpolator->calc() == RailInterpolator::Status::SegmentEnd) {
        onSegmentEnd();
    }
}

/// @addr{0x806CA2F0}
void ObjectHanachan::calcBody() {
    auto *&head = headPart();
    head->calcTransform();
    EGG::Vector3f forward = head->m_transform.base(2);
    EGG::Vector3f dir = Interpolate(0.1f, forward, m_railInterpolator->curTangentDir());
    head->calcTransformFromUpAndTangent(m_chain.pos(0), m_chain.up(0), dir);

    auto parts = bodyParts();
    for (size_t i = 0; i < parts.size(); ++i) {
        const EGG::Vector3f &pos = m_chain.pos(i + 1);
        dir = m_chain.pos(i) - pos;
        dir.normalise2();
        parts[i]->calcTransformFromUpAndTangent(pos, m_chain.up(i + 1), dir);
    }
}

/// @addr{0x806CA72C}
void ObjectHanachan::initBody() {
    headPart()->setPos(m_railInterpolator->curPos());

    const EGG::Vector3f &curTanDir = m_railInterpolator->curTangentDir();
    for (size_t i = 1; i < m_parts.size(); ++i) {
        m_parts[i]->setPos(m_parts[i - 1]->pos() - curTanDir * BODY_PART_DISTANCES[i - 1]);
        m_parts[i]->setMatrixTangentTo(EGG::Vector3f::ey, curTanDir);
    }
}

/// @addr{0x806CA9AC}
void ObjectHanachan::initChain() {
    m_chain.init();

    for (size_t i = 0; i < m_parts.size(); ++i) {
        m_chain.setPos(i, m_parts[i]->pos());
    }
}

/// @addr{0x806CAAD0}
void ObjectHanachan::clearChain() {
    m_chain.setPos(0, m_railInterpolator->curPos());
    m_chain.setVel(0, EGG::Vector3f::zero);
    m_chain.addSpringForce(0, EGG::Vector3f::ey * SphereLink::Gravity());
}

/// @addr{0x806CAB5C}
/// @brief If the wiggler has moved out of alignment of the rail, then applies a lateral adjustment
/// to the wiggler's body part segments.
void ObjectHanachan::calcRailAlignmentMotion() {
    constexpr u16 MISALIGNMENT_CORRECTION_DURATION = 80;

    bool becameUnaligned = m_prevRailAlignment == RailAlignment::Aligned &&
            m_railAlignment != RailAlignment::Aligned;

    if (becameUnaligned && m_railAlignment == RailAlignment::MisalignedLeft) {
        m_leftMisalignFrame = m_currentFrame;
    }

    if (m_leftMisalignFrame != 0 && m_currentFrame >= m_leftMisalignFrame &&
            m_currentFrame <
                    static_cast<u32>(m_leftMisalignFrame + MISALIGNMENT_CORRECTION_DURATION)) {
        calcFastMotion(m_currentFrame - m_leftMisalignFrame);
    } else {
        calcSlowMotion();
    }
}

/// @addr{0x806CACD0}
void ObjectHanachan::calcLateralMotion(f32 amplitude, f32 period, f32 wavelength, s16 frame) {
    f32 velAmplitude = F_TAU * amplitude / period;

    auto parts = bodyParts();
    for (size_t i = 0; i < parts.size(); ++i) {
        auto *&part = parts[i];
        f32 phase =
                F_TAU * (static_cast<f32>(frame) / period - m_partDisplacement[i + 1] / wavelength);
        part->calcTransform();
        EGG::Vector3f right = RotateXZByYaw(HALF_PI, part->m_transform.base(2));
        EGG::Vector3f posOffset = right * (amplitude * EGG::Mathf::SinFIdx(RAD2FIDX * phase));
        m_chain.setPos(i + 1, m_parts[i + 1]->pos() + posOffset);
        m_chain.setVel(i + 1, right * (velAmplitude * EGG::Mathf::CosFIdx(RAD2FIDX * phase)));
    }
}

/// @addr{0x806CAFB8}
ObjectHanachan::RailAlignment ObjectHanachan::calcRailAlignment() const {
    constexpr f32 EPSILON = 0.9995f;

    EGG::Vector3f curTanDir = m_railInterpolator->curTangentDir();
    EGG::Vector2f railTan = EGG::Vector2f(curTanDir.x, curTanDir.z);
    EGG::Vector2f right = EGG::Vector2f(m_right.x, m_right.z);
    railTan.normalise2();
    right.normalise2();

    if (railTan.dot(right) >= EPSILON) {
        return RailAlignment::Aligned;
    }

    return right.cross(railTan) < 0.0f ? RailAlignment::MisalignedLeft :
                                         RailAlignment::MisalignedRight;
}

} // namespace Field
