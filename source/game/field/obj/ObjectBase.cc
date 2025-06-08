#include "ObjectBase.hh"

#include "game/field/ObjectDirector.hh"

#include "game/system/CourseMap.hh"
#include "game/system/ResourceManager.hh"
#include "game/system/map/MapdataPointInfo.hh"

#include <egg/math/Math.hh>

#include <cstring>

namespace Field {

/// @addr{0x8081F828}
ObjectBase::ObjectBase(const System::MapdataGeoObj &params)
    : m_drawMdl(nullptr), m_resFile(nullptr), m_id(static_cast<ObjectId>(params.id())),
      m_flags(0x3), m_pos(params.pos()), m_rot(params.rot() * DEG2RAD), m_scale(params.scale()),
      m_transform(EGG::Matrix34f::ident), m_mapObj(&params) {}

/// @addr{0x8067E3C4}
ObjectBase::~ObjectBase() {
    delete m_resFile;
    delete m_drawMdl;
}

/// @addr{0x808217B8}
void ObjectBase::calcModel() {
    calcTransform();
}

/// @addr{0x80680730}
const char *ObjectBase::getResources() const {
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto *collisionSet = flowTable.set(flowTable.slot(m_id));
    ASSERT(collisionSet);
    return collisionSet->resources;
}

/// @addr{0x8081FD10}
void ObjectBase::loadGraphics() {
    const char *name = getResources();
    if (strcmp(name, "-") == 0) {
        return;
    }

    char filename[128];
    snprintf(filename, sizeof(filename), "%s.brres", name);

    auto *resMgr = System::ResourceManager::Instance();
    const void *resFile = resMgr->getFile(filename, nullptr, System::ArchiveId::Course);
    if (resFile) {
        m_resFile = new Abstract::g3d::ResFile(resFile);
        m_drawMdl = new Render::DrawMdl;
    }
}

/// @addr{0x80820980}
void ObjectBase::loadRail() {
    if (!m_mapObj) {
        return;
    }

    s16 pathId = m_mapObj->pathId();

    if (pathId == -1) {
        return;
    }

    auto *point = System::CourseMap::Instance()->getPointInfo(pathId);
    f32 speed = static_cast<f32>(m_mapObj->setting(0));

    if (point->setting(0) == 0) {
        m_railInterpolator = new RailLinearInterpolator(speed, pathId);
    } else {
        m_railInterpolator = new RailSmoothInterpolator(speed, pathId);
    }
}

/// @addr{0x806806DC}
const char *ObjectBase::getKclName() const {
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto *collisionSet = flowTable.set(flowTable.slot(m_id));
    ASSERT(collisionSet);
    return collisionSet->resources;
}

/// @addr{0x80821640}
void ObjectBase::calcTransform() {
    if (m_flags & 2) {
        m_transform.makeRT(m_rot, m_pos);
        m_flags &= ~0x3;
    } else if (m_flags & 1) {
        m_transform.setBase(3, m_pos);
    }
}

/// @addr{0x80820EB8}
void ObjectBase::linkAnims(const std::span<const char *> &names,
        const std::span<Render::AnmType> types) {
    if (!m_drawMdl) {
        return;
    }

    ASSERT(names.size() == types.size());

    for (size_t i = 0; i < names.size(); ++i) {
        m_drawMdl->linkAnims(i, m_resFile, names[i], types[i]);
    }
}

/// @addr{0x80821910}
void ObjectBase::setMatrixTangentTo(const EGG::Vector3f &up, const EGG::Vector3f &tangent) {
    m_flags |= 4;
    SetRotTangentHorizontal(m_transform, up, tangent);
    m_transform.setBase(3, m_pos);
}

/// @addr{0x806B3AC4}
EGG::Vector3f ObjectBase::RotateAxisAngle(f32 angle, const EGG::Vector3f &axis,
        const EGG::Vector3f &v1) {
    EGG::Matrix34f mat;
    mat.setBase(3, EGG::Vector3f::zero);
    mat.setAxisRotation(angle, axis);
    return mat.ps_multVector(v1);
}

/// @addr{0x806B41E0}
void ObjectBase::SetRotTangentHorizontal(EGG::Matrix34f &mat, const EGG::Vector3f &up,
        const EGG::Vector3f &tangent) {
    EGG::Vector3f vec = tangent - up * tangent.dot(up);
    vec.normalise2();

    mat.setBase(0, up.cross(vec));
    mat.setBase(1, up);
    mat.setBase(2, vec);
}

} // namespace Field
