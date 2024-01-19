#include "CourseColMgr.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/ResourceManager.hh"

// Credit: em-eight/mkw

namespace Field {

void CourseColMgr::init() {
    void *file = LoadFile("course.kcl");
    m_data = new KColData(file);
}

/* 0x807c3e84 - CourseColMgr::checkCollisionOther */
bool CourseColMgr::checkSphereFullPush(f32 scalar, f32 radius, KColData *data,
        const EGG::Vector3f &v0, const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *info,
        KCLTypeMask *kcl_flags_out) {
    if (!data) {
        data = m_data;
    }

    m_kclScale = scalar;
    EGG::Vector3f scaled_position = v0 / scalar;
    EGG::Vector3f vStack88 = v1 / scalar;
    data->lookupSphere(radius, scaled_position, vStack88, flags);

    if (info) {
        return doCheckWithFullInfoPush(data, &KColData::checkSphere, info, kcl_flags_out);
    }
    return false; // doCheckMaskOnlyPush(data, &KColData::checkSphereCollision,
                  // kcl_flags_out);
}

void *CourseColMgr::LoadFile(const char *filename) {
    auto *resMgr = System::ResourceManager::Instance();
    return resMgr->getFile(filename, nullptr, System::ArchiveId::Course);
}

CourseColMgr *CourseColMgr::CreateInstance() {
    assert(!s_instance);
    s_instance = new CourseColMgr;
    return s_instance;
}

void CourseColMgr::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

CourseColMgr *CourseColMgr::Instance() {
    return s_instance;
}

CourseColMgr::CourseColMgr()
    : m_data(nullptr), m_kclScale(1.0f), m_noBounceWallInfo(nullptr), m_localMtx(nullptr) {}

CourseColMgr::~CourseColMgr() {
    assert(m_data);
    delete m_data;
}

bool CourseColMgr::doCheckWithFullInfoPush(KColData *colMgr, CollisionCheckFunc collisionCheckFunc,
        CollisionInfo *colInfo, u32 *flagsOut) {
    f32 dist;
    EGG::Vector3f fnrm;
    u16 attribute;
    bool hasCol = false;

    while ((colMgr->*collisionCheckFunc)(&dist, &fnrm, &attribute)) {
        dist *= m_kclScale;

        /*if (m_noBounceWallInfo && (attribute & KCL_SOFT_WALL_MASK)) {
            if (m_localMtx) {
                nw4r::math::VEC3TransformNormal(&fnrm, m_localMtx, &fnrm);
            }

            offset = fnrm * dist;
            softWallColInfo->update(dist, offset, fnrm);

            hasCol = true;
        } else {*/
        u32 kclAttributeTypeBit = KCL_ATTRIBUTE_TYPE_BIT(attribute);
        if (flagsOut) {
            CollisionDirector::Instance()->pushCollisionEntry(dist, flagsOut, kclAttributeTypeBit,
                    attribute);
        }
        if (kclAttributeTypeBit & KCL_TYPE_SOLID_SURFACE) {
            colInfo->update(dist, fnrm * dist, fnrm, kclAttributeTypeBit);
        }
        hasCol = true;
        /*}*/
    }
    m_localMtx = nullptr;
    return hasCol;
}

void CourseColMgr::CollisionInfo::updateFloor(f32 dist, const EGG::Vector3f &fnrm) {
    if (dist > m_floorDist) {
        m_floorDist = dist;
        m_floorNrm = fnrm;
    }
}

void CourseColMgr::CollisionInfo::updateWall(f32 dist, const EGG::Vector3f &fnrm) {
    if (dist > m_wallDist) {
        m_wallDist = dist;
        m_wallNrm = fnrm;
    }
}

void CourseColMgr::CollisionInfo::update(f32 now_dist, const EGG::Vector3f &offset,
        const EGG::Vector3f &fnrm, u32 kclAttributeTypeBit) {
    m_bbox.mMin = m_bbox.mMin.minimize(offset);
    m_bbox.mMax = m_bbox.mMax.maximize(offset);

    if (kclAttributeTypeBit & KCL_TYPE_FLOOR) {
        updateFloor(now_dist, fnrm);
    } else if (kclAttributeTypeBit & KCL_TYPE_WALL) {
        if (m_wallDist > -std::numeric_limits<f32>::min()) {
            f32 perpendicularity = 1.0f - m_wallNrm.ps_dot(fnrm);
            if (perpendicularity > m_perpendicularity) {
                m_perpendicularity = std::min(perpendicularity, 1.0f);
            }
        }

        updateWall(now_dist, fnrm);
    }
}

CourseColMgr *CourseColMgr::s_instance = nullptr;

} // namespace Field
