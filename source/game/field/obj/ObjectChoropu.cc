#include "ObjectChoropu.hh"

#include "game/field/ObjectDirector.hh"
#include "game/field/RailManager.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x806B96A0}
ObjectChoropu::ObjectChoropu(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), StateManager(this, STATE_ENTRIES),
      m_startFrameOffset(params.setting(1)), m_idleDuration(params.setting(0)),
      m_isStationary(strcmp(getName(), "choropu") != 0) {
    constexpr f32 MAX_SPEED = 20.0f;

    s16 railIdx = params.pathId();
    if (railIdx != -1) {
        auto *rail = RailManager::Instance()->rail(railIdx);
        rail->checkSphereFull();
    }

    // If the mole moves around, then we need to create the dirt trail.
    if (!m_isStationary) {
        const auto &flowTable = ObjectDirector::Instance()->flowTable();
        const auto *collisionSet =
                flowTable.set(flowTable.slot(flowTable.getIdFromName("choropu_ground")));
        ASSERT(collisionSet);

        s16 height = parse<s16>(collisionSet->params.cylinder.height);
        size_t groundCount = static_cast<size_t>(MAX_GROUND_LEN / EGG::Mathf::abs(height * 2)) + 1;
        m_groundObjs = std::span<ObjectChoropuGround *>(new ObjectChoropuGround *[groundCount],
                groundCount);

        for (auto *&obj : m_groundObjs) {
            obj = new ObjectChoropuGround(m_pos, m_rot, m_scale);
            obj->load();
            obj->resize(RADIUS, MAX_SPEED);
        }

        m_groundHeight = m_groundObjs.front()->height();
    }

    m_objHoll = new ObjectChoropuHoll(params);
    m_objHoll->load();
}

/// @addr{0x806B9B8C}
ObjectChoropu::~ObjectChoropu() {
    delete[] m_groundObjs.data();
}

/// @addr{0x806B9BF8}
void ObjectChoropu::init() {
    if (m_isStationary) {
        disableCollision();

        m_scale.x = 1.0f;
        m_objHoll->setScale(EGG::Vector3f(1.0f, m_objHoll->scale().y, 1.0f));
        m_nextStateId = 0;
        m_groundLength = 0.0f;

        calcTransform();
        m_transMat = m_transform;
    } else {
        if (m_mapObj->pathId() == -1) {
            return;
        }

        m_railInterpolator->init(0.0f, 0);
        m_railInterpolator->setPerPointVelocities(true);

        m_railMat = RailOrthonormalBasis(*m_railInterpolator);
        m_pos = m_railMat.base(3);
        m_flags.setBit(eFlags::Position);

        disableCollision();
        m_objHoll->disableCollision();

        m_nextStateId = 0;
    }
}

/// @addr{0x806B9E60}
void ObjectChoropu::calc() {
    constexpr u32 START_DELAY = 300;

    // Nothing to do if the mole hasn't spawned yet
    u32 t = System::RaceManager::Instance()->timer();
    if (t < m_startFrameOffset + START_DELAY) {
        return;
    }

    if (!m_isStationary) {
        if (m_mapObj->pathId() == -1) {
            return;
        }

        m_railMat = RailOrthonormalBasis(*m_railInterpolator);
    }

    StateManager::calc();

    m_objHoll->setScale(EGG::Vector3f(1.0f, m_objHoll->scale().y, 1.0f));
}

/// @addr{0x806BA144}
Kart::Reaction ObjectChoropu::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    return m_currentStateId == 1 ? Kart::Reaction::SmallBump : reactionOnKart;
}

void ObjectChoropu::enterStateStub() {}

/// @addr{0x806BA6D8}
void ObjectChoropu::enterDigging() {
    if (m_isStationary) {
        disableCollision();
    } else {
        for (auto *&obj : m_groundObjs) {
            obj->enableCollision();
        }

        disableCollision();
        m_objHoll->disableCollision();
        m_groundLength = 0.0f;
    }
}

/// @addr{0x806BABEC}
void ObjectChoropu::enterPeeking() {
    if (m_isStationary) {
        m_pos = m_transMat.base(3);
        m_flags.setBit(eFlags::Position, eFlags::Rotation);
        m_rot.z = 0.0f;

        enableCollision();
    } else {
        m_pos = m_railMat.base(3);
        m_flags.setBit(eFlags::Position, eFlags::Rotation);
        m_rot.z = 0.0f;
        m_rot.y = 0.0f;

        enableCollision();

        const auto &curTanDir = m_railInterpolator->curTangentDir();
        s16 curPointIdx = m_railInterpolator->curPointIdx();
        EGG::Matrix34f mat;
        SetRotTangentHorizontal(mat, m_railInterpolator->floorNrm(curPointIdx), curTanDir);
        mat.setBase(3, m_railInterpolator->curPos());

        m_objHoll->setTransform(mat);
        m_objHoll->setPos(mat.base(3));
        m_objHoll->enableCollision();
    }
}

/// @addr{0x806BB39C}
void ObjectChoropu::enterJumping() {
    enableCollision();

    m_pos = m_isStationary ? m_transMat.base(3) : m_railMat.base(3);
    m_flags.setBit(eFlags::Position, eFlags::Rotation);
    m_rot.z = 0.0f;
}

void ObjectChoropu::calcStateStub() {}

/// @addr{0x806BA7FC}
void ObjectChoropu::calcDigging() {
    if (m_isStationary) {
        if (m_currentFrame > static_cast<u32>(m_idleDuration)) {
            m_nextStateId = 1;
        }

        return;
    }

    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);

    if (m_railInterpolator->calc() == RailInterpolator::Status::SegmentEnd) {
        if (m_railInterpolator->curPoint().setting[1] == 1) {
            m_nextStateId = 1;
        } else {
            calcGround();
        }
    } else {
        bool skipGroundCalc = false;

        if (m_railInterpolator->nextPoint().setting[1] == 1) {
            f32 invT = 1.0f - m_railInterpolator->segmentT();
            if (invT * m_railInterpolator->getCurrSegmentLength() < 250.0f) {
                skipGroundCalc = true;
            }
        }

        if (!skipGroundCalc) {
            calcGround();
        }
    }
}

/// @addr{0x806BB144}
void ObjectChoropu::calcPeeking() {
    constexpr s16 PEEK_DURATION = 40;
    constexpr s16 STATE_DURATION = 100;

    if (!m_isStationary) {
        m_groundLength = std::max(0.0f, m_groundLength - m_railInterpolator->speed());

        calcGroundObjs();
    }

    if (m_currentFrame > STATE_DURATION) {
        m_nextStateId = 3;
    }

    if (m_currentFrame > PEEK_DURATION) {
        disableCollision();
    }
}

/// @addr{0x806BB5F0}
void ObjectChoropu::calcJumping() {
    constexpr f32 JUMP_LINEAR_COEFFICIENT = 65.0f;
    constexpr f32 JUMP_QUADRATIC_COEFFICIENT = 2.7f;

    if (!m_isStationary) {
        m_groundLength = std::max(0.0f, m_groundLength - m_railInterpolator->speed());
        calcGroundObjs();
    }

    // y = -1.35t^2 + 65.0t
    f32 pos = JUMP_LINEAR_COEFFICIENT * static_cast<f32>(m_currentFrame) -
            static_cast<f32>(m_currentFrame) * 0.5f * JUMP_QUADRATIC_COEFFICIENT *
                    static_cast<f32>(m_currentFrame);

    if (pos < 0.0f) {
        m_nextStateId = 0;
    }

    m_pos.y = pos + (m_isStationary ? m_transMat.base(3).y : m_railInterpolator->curPos().y);
    m_flags.setBit(eFlags::Position);
}

/// @addr{0x806BBA7C}
void ObjectChoropu::calcGround() {
    m_groundLength += m_railInterpolator->getCurrVel();
    if (m_groundLength > MAX_GROUND_LEN) {
        m_groundLength = MAX_GROUND_LEN - 1.0f;
    }

    calcGroundObjs();
}

/// @addr{0x806BB840}
void ObjectChoropu::calcGroundObjs() {
    size_t idx =
            std::min(static_cast<size_t>(m_groundLength / m_groundHeight) + 1, m_groundObjs.size());

    for (auto *&obj : m_groundObjs) {
        obj->enableCollision();
    }

    for (size_t i = idx; i < m_groundObjs.size(); ++i) {
        m_groundObjs[i]->disableCollision();
    }

    if (m_groundLength > RADIUS) {
        f32 height = std::min(m_groundHeight, m_groundLength) - RADIUS;
        m_groundObjs[0]->calcPosAndMat(height, calcInterpolatedPose(RADIUS + 0.5f * height));
    }

    for (size_t i = 1; i < idx - 1; ++i) {
        f32 height = 0.5f * m_groundHeight + m_groundHeight * static_cast<f32>(i);
        m_groundObjs[i]->calcPosAndMat(m_groundHeight, calcInterpolatedPose(height));
    }

    f32 height = m_groundLength - m_groundHeight * static_cast<f32>(idx - 1);
    EGG::Matrix34f mat =
            calcInterpolatedPose(0.5f * height + m_groundHeight * static_cast<f32>(idx - 1));
    m_groundObjs[idx - 1]->calcPosAndMat(height, mat);
}

/// @addr{0x806B46F8}
EGG::Matrix34f ObjectChoropu::calcInterpolatedPose(f32 t) const {
    EGG::Vector3f curDir;
    EGG::Vector3f curTanDir;
    m_railInterpolator->evalCubicBezierOnPath(t, curDir, curTanDir);
    EGG::Matrix34f mat = OrthonormalBasis(curTanDir);
    mat.setBase(3, curDir);
    return mat;
}

/// @addr{0x806B8F94}
ObjectChoropuGround::ObjectChoropuGround(const EGG::Vector3f &pos, const EGG::Vector3f &rot,
        const EGG::Vector3f &scale)
    : ObjectCollidable("choropu_ground", pos, rot, scale) {
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto *collisionSet =
            flowTable.set(flowTable.slot(flowTable.getIdFromName("choropu_ground")));
    ASSERT(collisionSet);

    s16 height = parse<s16>(collisionSet->params.cylinder.height);
    m_height = 2.0f * EGG::Mathf::abs(static_cast<f32>(height));
}

/// @addr{0x806BBE6C}
ObjectChoropuGround::~ObjectChoropuGround() = default;

/// @addr{0x806B9274}
void ObjectChoropuGround::calcPosAndMat(f32 height, const EGG::Matrix34f &mat) {
    EGG::Matrix34f matTemp;
    SetRotTangentHorizontal(matTemp, mat.base(2), EGG::Vector3f::ey);
    matTemp.setBase(1, matTemp.base(1) * (height / m_height));
    matTemp.setBase(3, mat.base(3));
    m_flags.setBit(eFlags::Matrix);
    m_transform = matTemp;
    m_pos = matTemp.base(3);
}

/// @addr{0x806B93CC}
ObjectChoropuHoll::ObjectChoropuHoll(const System::MapdataGeoObj &params)
    : ObjectCollidable(params) {}

/// @addr{0x806BBE6C}
ObjectChoropuHoll::~ObjectChoropuHoll() = default;

} // namespace Field
