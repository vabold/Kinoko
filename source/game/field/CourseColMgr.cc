#include "CourseColMgr.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/ResourceManager.hh"

// Credit: em-eight/mkw

namespace Field {

/// @addr{0x807C28D8}
void CourseColMgr::init() {
    // In the base game, this file is loaded in CollisionDirector::CreateInstance and passed into
    // this function. It's simpler to just keep it here.
    void *file = LoadFile("course.kcl");
    m_data = new KColData(file);
}

/// @addr{0x807C293C}
void CourseColMgr::scaledNarrowScopeLocal(f32 scale, f32 radius, KColData *data,
        const EGG::Vector3f &pos, KCLTypeMask mask) {
    if (!data) {
        data = m_data;
    }

    data->narrowScopeLocal(pos / scale, radius / scale, mask);
}

/// @addr{0x807C3CF0}
bool CourseColMgr::checkSphereFull(f32 scalar, f32 radius, KColData *data, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *info,
        KCLTypeMask *kcl_flags_out) {
    if (!data) {
        data = m_data;
    }

    m_kclScale = scalar;
    EGG::Vector3f scaled_position = v0 / scalar;
    EGG::Vector3f vStack88 = v1 / scalar;
    data->lookupSphere(radius, scaled_position, vStack88, flags);

    if (info) {
        return doCheckWithFullInfo(data, &KColData::checkSphereCollision, info, kcl_flags_out);
    }
    return false; // doCheckMaskOnly(data, &KColData::checkSphereCollision, kcl_flags_out);
}

/// @addr{0x807C3E84}
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
        return doCheckWithFullInfoPush(data, &KColData::checkSphereCollision, info, kcl_flags_out);
    }
    return doCheckMaskOnlyPush(data, &KColData::checkSphereCollision, kcl_flags_out);
}

/// @addr{0x807C4648}
bool CourseColMgr::checkSphereCachedPartial(KColData *data, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfo *colInfo,
        KCLTypeMask *typeMaskOut, f32 scale, f32 radius) {
    if (!data) {
        data = m_data;
    }

    if (data->prismCache(0) == 0) {
        return false;
    }

    m_kclScale = scale;

    data->lookupSphereCached(pos / scale, prevPos / scale, typeMask, radius / scale);

    if (colInfo) {
        return doCheckWithPartialInfo(data, &KColData::checkSphereCollision, colInfo, typeMaskOut);
    }

    // Not required atm
    return false;
}

/// @addr{0x807C47F0}
bool CourseColMgr::checkSphereCachedPartialPush(KColData *data, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfo *colInfo,
        KCLTypeMask *typeMaskOut, f32 scale, f32 radius) {
    if (!data) {
        data = m_data;
    }

    if (data->prismCache(0) == 0) {
        return false;
    }

    m_kclScale = scale;

    data->lookupSphereCached(pos / scale, prevPos / scale, typeMask, radius / scale);

    if (colInfo) {
        return doCheckWithPartialInfoPush(data, &KColData::checkSphereCollision, colInfo,
                typeMaskOut);
    }

    return doCheckMaskOnlyPush(data, &KColData::checkSphereCollision, typeMaskOut);
}

/// @addr{0x807C4B40}
bool CourseColMgr::checkSphereCachedFullPush(KColData *data, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfo *colInfo,
        KCLTypeMask *typeMaskOut, f32 scale, f32 radius) {
    if (!data) {
        data = m_data;
    }

    if (data->prismCache(0) == 0) {
        return false;
    }

    m_kclScale = scale;

    data->lookupSphereCached(pos / scale, prevPos / scale, typeMask, radius / scale);

    if (colInfo) {
        return doCheckWithFullInfoPush(data, &KColData::checkSphereCollision, colInfo, typeMaskOut);
    } else {
        // Not needed currently
        return false;
    }
}

void CourseColMgr::setNoBounceWallInfo(NoBounceWallColInfo *info) {
    m_noBounceWallInfo = info;
}

void CourseColMgr::clearNoBounceWallInfo() {
    m_noBounceWallInfo = nullptr;
}

CourseColMgr::NoBounceWallColInfo *CourseColMgr::noBounceWallInfo() const {
    return m_noBounceWallInfo;
}

/// @brief Loads a particular section of a .szs file
void *CourseColMgr::LoadFile(const char *filename) {
    auto *resMgr = System::ResourceManager::Instance();
    return resMgr->getFile(filename, nullptr, System::ArchiveId::Course);
}

/// @addr{0x807C2824}
CourseColMgr *CourseColMgr::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new CourseColMgr;
    return s_instance;
}

/// @addr{0x807C2884}
void CourseColMgr::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

CourseColMgr *CourseColMgr::Instance() {
    return s_instance;
}

/// @addr{0x807C29E4}
CourseColMgr::CourseColMgr()
    : m_data(nullptr), m_kclScale(1.0f), m_noBounceWallInfo(nullptr), m_localMtx(nullptr) {}

/// @addr{0x807C2A04}
CourseColMgr::~CourseColMgr() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("CourseColMgr instance not explicitly handled!");
    }

    ASSERT(m_data);
    delete m_data;
}

/// @addr{0x807C2BD8}
bool CourseColMgr::doCheckWithPartialInfo(KColData *data, CollisionCheckFunc collisionCheckFunc,
        CollisionInfo *colInfo, KCLTypeMask *typeMask) {
    f32 dist;
    EGG::Vector3f fnrm;
    u16 attribute;
    bool hasCol = false;

    while ((data->*collisionCheckFunc)(&dist, &fnrm, &attribute)) {
        hasCol = true;
        dist *= m_kclScale;

        if (m_noBounceWallInfo && (attribute & KCL_SOFT_WALL_MASK)) {
            if (m_localMtx) {
                fnrm = m_localMtx->multVector33(fnrm);
            }
            EGG::Vector3f offset = fnrm * dist;
            m_noBounceWallInfo->bbox.min = m_noBounceWallInfo->bbox.min.minimize(offset);
            m_noBounceWallInfo->bbox.max = m_noBounceWallInfo->bbox.max.maximize(offset);
            if (m_noBounceWallInfo->dist < dist) {
                m_noBounceWallInfo->dist = dist;
                m_noBounceWallInfo->fnrm = fnrm;
            }
        } else {
            u32 flags = KCL_ATTRIBUTE_TYPE_BIT(attribute);
            if (typeMask) {
                *typeMask = *typeMask | flags;
            }
            if (flags & KCL_TYPE_SOLID_SURFACE) {
                EGG::Vector3f offset = fnrm * dist;
                colInfo->bbox.min = colInfo->bbox.min.minimize(offset);
                colInfo->bbox.max = colInfo->bbox.max.maximize(offset);
            }
        }
    }

    m_localMtx = nullptr;

    return hasCol;
}

/// @addr{0x807C2F18}
bool CourseColMgr::doCheckWithPartialInfoPush(KColData *data, CollisionCheckFunc collisionCheckFunc,
        CollisionInfo *colInfo, KCLTypeMask *typeMask) {
    f32 dist;
    EGG::Vector3f fnrm;
    u16 attribute;
    bool hasCol = false;

    while ((data->*collisionCheckFunc)(&dist, &fnrm, &attribute)) {
        hasCol = true;
        dist *= m_kclScale;

        if (!m_noBounceWallInfo || !(attribute & KCL_SOFT_WALL_MASK)) {
            u32 flags = KCL_ATTRIBUTE_TYPE_BIT(attribute);
            if (typeMask) {
                CollisionDirector::Instance()->pushCollisionEntry(dist, typeMask, flags, attribute);
            }
            if (flags & KCL_TYPE_SOLID_SURFACE) {
                EGG::Vector3f offset = fnrm * dist;
                colInfo->bbox.min = colInfo->bbox.min.minimize(offset);
                colInfo->bbox.max = colInfo->bbox.max.maximize(offset);
            }
        } else {
            if (m_localMtx) {
                fnrm = m_localMtx->multVector33(fnrm);
            }
            EGG::Vector3f offset = fnrm * dist;
            m_noBounceWallInfo->bbox.min = m_noBounceWallInfo->bbox.min.minimize(offset);
            m_noBounceWallInfo->bbox.max = m_noBounceWallInfo->bbox.max.maximize(offset);
            if (m_noBounceWallInfo->dist < dist) {
                m_noBounceWallInfo->dist = dist;
                m_noBounceWallInfo->fnrm = fnrm;
            }
        }
    }

    m_localMtx = nullptr;

    return hasCol;
}

/// @addr{0x807C3258}
bool CourseColMgr::doCheckWithFullInfo(KColData *data, CollisionCheckFunc collisionCheckFunc,
        CollisionInfo *colInfo, KCLTypeMask *flagsOut) {
    f32 dist;
    EGG::Vector3f fnrm;
    u16 attribute;
    bool hasCol = false;

    while ((data->*collisionCheckFunc)(&dist, &fnrm, &attribute)) {
        dist *= m_kclScale;

        if (m_noBounceWallInfo && attribute & KCL_SOFT_WALL_MASK) {
            if (m_localMtx) {
                fnrm = m_localMtx->multVector33(fnrm);
            }
            EGG::Vector3f offset = fnrm * dist;
            m_noBounceWallInfo->bbox.min = m_noBounceWallInfo->bbox.min.minimize(offset);
            m_noBounceWallInfo->bbox.max = m_noBounceWallInfo->bbox.max.maximize(offset);
            if (m_noBounceWallInfo->dist < dist) {
                m_noBounceWallInfo->dist = dist;
                m_noBounceWallInfo->fnrm = fnrm;
            }
        } else {
            u32 kclAttributeTypeBit = KCL_ATTRIBUTE_TYPE_BIT(attribute);
            if (flagsOut) {
                *flagsOut |= kclAttributeTypeBit;
            }
            if (kclAttributeTypeBit & KCL_TYPE_SOLID_SURFACE) {
                colInfo->update(dist, fnrm * dist, fnrm, kclAttributeTypeBit);
            }
        }

        hasCol = true;
    }

    m_localMtx = nullptr;

    return hasCol;
}

/// @addr{0x807C36CC}
bool CourseColMgr::doCheckWithFullInfoPush(KColData *data, CollisionCheckFunc collisionCheckFunc,
        CollisionInfo *colInfo, KCLTypeMask *flagsOut) {
    f32 dist;
    EGG::Vector3f fnrm;
    u16 attribute;
    bool hasCol = false;

    while ((data->*collisionCheckFunc)(&dist, &fnrm, &attribute)) {
        dist *= m_kclScale;

        if (m_noBounceWallInfo && attribute & KCL_SOFT_WALL_MASK) {
            if (m_localMtx) {
                fnrm = m_localMtx->multVector33(fnrm);
            }
            EGG::Vector3f offset = fnrm * dist;
            m_noBounceWallInfo->bbox.min = m_noBounceWallInfo->bbox.min.minimize(offset);
            m_noBounceWallInfo->bbox.max = m_noBounceWallInfo->bbox.max.maximize(offset);
            if (m_noBounceWallInfo->dist < dist) {
                m_noBounceWallInfo->dist = dist;
                m_noBounceWallInfo->fnrm = fnrm;
            }
        } else {
            u32 kclAttributeTypeBit = KCL_ATTRIBUTE_TYPE_BIT(attribute);
            if (flagsOut) {
                CollisionDirector::Instance()->pushCollisionEntry(dist, flagsOut,
                        kclAttributeTypeBit, attribute);
            }
            if (kclAttributeTypeBit & KCL_TYPE_SOLID_SURFACE) {
                colInfo->update(dist, fnrm * dist, fnrm, kclAttributeTypeBit);
            }
        }

        hasCol = true;
    }

    m_localMtx = nullptr;

    return hasCol;
}

bool CourseColMgr::doCheckMaskOnlyPush(KColData *data, CollisionCheckFunc collisionCheckFunc,
        KCLTypeMask *typeMaskOut) {
    bool hasCol = false;
    f32 dist;
    u16 attribute;

    while ((data->*collisionCheckFunc)(&dist, nullptr, &attribute)) {
        KCLTypeMask mask = KCL_ATTRIBUTE_TYPE_BIT(attribute);

        if ((!m_noBounceWallInfo || !(attribute & KCL_SOFT_WALL_MASK)) && typeMaskOut) {
            CollisionDirector::Instance()->pushCollisionEntry(dist, typeMaskOut, mask, attribute);
        }
        hasCol = true;
    }

    return hasCol;
}

void CollisionInfo::updateFloor(f32 dist, const EGG::Vector3f &fnrm) {
    if (dist > floorDist) {
        floorDist = dist;
        floorNrm = fnrm;
    }
}

void CollisionInfo::updateWall(f32 dist, const EGG::Vector3f &fnrm) {
    if (dist > wallDist) {
        wallDist = dist;
        wallNrm = fnrm;
    }
}

void CollisionInfo::update(f32 now_dist, const EGG::Vector3f &offset,
        const EGG::Vector3f &fnrm, u32 kclAttributeTypeBit) {
    bbox.min = bbox.min.minimize(offset);
    bbox.max = bbox.max.maximize(offset);

    if (kclAttributeTypeBit & KCL_TYPE_FLOOR) {
        updateFloor(now_dist, fnrm);
    } else if (kclAttributeTypeBit & KCL_TYPE_WALL) {
        if (wallDist > -std::numeric_limits<f32>::min()) {
            f32 dot = 1.0f - wallNrm.ps_dot(fnrm);
            if (dot > perpendicularity) {
                perpendicularity = std::min(dot, 1.0f);
            }
        }

        updateWall(now_dist, fnrm);
    }
}

CourseColMgr *CourseColMgr::s_instance = nullptr; ///< @addr{0x809C3C10}

} // namespace Field
