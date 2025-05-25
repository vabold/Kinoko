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

    f32 invScale = 1.0f / scale;
    data->narrowScopeLocal(pos * invScale, radius * invScale, mask);
}

/// @addr{0x807C2A60}
bool CourseColMgr::checkPointPartial(f32 scale, KColData *data, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupPoint(v0 * invScale, v1 * invScale, mask);

    if (info) {
        return doCheckWithPartialInfo(data, &KColData::checkPointCollision, info, maskOut);
    }

    return doCheckMaskOnly(data, &KColData::checkPointCollision, maskOut);
}

/// @addr{0x807C2DA0}
bool CourseColMgr::checkPointPartialPush(f32 scale, KColData *data, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupPoint(v0 * invScale, v1 * invScale, mask);

    if (info) {
        return doCheckWithPartialInfoPush(data, &KColData::checkPointCollision, info, maskOut);
    }
    return doCheckMaskOnlyPush(data, &KColData::checkPointCollision, maskOut);
}

/// @addr{0x807C30E0}
bool CourseColMgr::checkPointFull(f32 scale, KColData *data, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupPoint(v0 * invScale, v1 * invScale, mask);

    if (info) {
        return doCheckWithFullInfo(data, &KColData::checkPointCollision, info, maskOut);
    }
    return doCheckMaskOnly(data, &KColData::checkPointCollision, maskOut);
}

/// @addr{0x807C3554}
bool CourseColMgr::checkPointFullPush(f32 scale, KColData *data, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupPoint(v0 * invScale, v1 * invScale, mask);

    if (info) {
        return doCheckWithFullInfoPush(data, &KColData::checkPointCollision, info, maskOut);
    }
    return doCheckMaskOnlyPush(data, &KColData::checkPointCollision, maskOut);
}

/// @addr{0x807C39C8}
bool CourseColMgr::checkSpherePartial(f32 scale, f32 radius, KColData *data,
        const EGG::Vector3f &v0, const EGG::Vector3f &v1, KCLTypeMask mask,
        CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupSphere(radius * invScale, v0 * invScale, v1 * invScale, mask);

    if (info) {
        return doCheckWithPartialInfo(data, &KColData::checkSphereCollision, info, maskOut);
    }
    return doCheckMaskOnly(data, &KColData::checkSphereCollision, maskOut);
}

/// @addr{0x807C3B5C}
bool CourseColMgr::checkSpherePartialPush(f32 scale, f32 radius, KColData *data,
        const EGG::Vector3f &v0, const EGG::Vector3f &v1, KCLTypeMask mask,
        CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupSphere(radius * invScale, v0 * invScale, v1 * invScale, mask);

    if (info) {
        return doCheckWithPartialInfoPush(data, &KColData::checkSphereCollision, info, maskOut);
    }
    return doCheckMaskOnlyPush(data, &KColData::checkSphereCollision, maskOut);
}

/// @addr{0x807C3CF0}
bool CourseColMgr::checkSphereFull(f32 scale, f32 radius, KColData *data, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupSphere(radius * invScale, v0 * invScale, v1 * invScale, mask);

    if (info) {
        return doCheckWithFullInfo(data, &KColData::checkSphereCollision, info, maskOut);
    }
    return doCheckMaskOnly(data, &KColData::checkSphereCollision, maskOut);
}

/// @addr{0x807C3E84}
bool CourseColMgr::checkSphereFullPush(f32 scale, f32 radius, KColData *data,
        const EGG::Vector3f &v0, const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfo *info,
        KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupSphere(radius * invScale, v0 * invScale, v1 * invScale, mask);

    if (info) {
        return doCheckWithFullInfoPush(data, &KColData::checkSphereCollision, info, maskOut);
    }
    return doCheckMaskOnlyPush(data, &KColData::checkSphereCollision, maskOut);
}

/// @addr{0x807C4018}
bool CourseColMgr::checkPointCachedPartial(f32 scale, KColData *data, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupPoint(v0 * invScale, v1 * invScale, mask);

    if (info) {
        return doCheckWithPartialInfo(data, &KColData::checkPointCollision, info, maskOut);
    }
    return doCheckMaskOnly(data, &KColData::checkPointCollision, maskOut);
}

/// @addr{0x807C41A4}
bool CourseColMgr::checkPointCachedPartialPush(f32 scale, KColData *data, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    if (data->prismCache(0) == 0) {
        return false;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupPoint(v0 * invScale, v1 * invScale, mask);

    if (info) {
        return doCheckWithPartialInfoPush(data, &KColData::checkPointCollision, info, maskOut);
    }
    return doCheckMaskOnlyPush(data, &KColData::checkPointCollision, maskOut);
}

/// @addr{0x807C4330}
bool CourseColMgr::checkPointCachedFull(f32 scale, KColData *data, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfo *pInfo, KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    if (data->prismCache(0) == 0) {
        return false;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupPoint(v0 * invScale, v1 * invScale, mask);

    if (pInfo) {
        return doCheckWithFullInfo(data, &KColData::checkPointCollision, pInfo, maskOut);
    }
    return doCheckMaskOnly(data, &KColData::checkPointCollision, maskOut);
}

/// @addr{0x807C44BC}
bool CourseColMgr::checkPointCachedFullPush(f32 scale, KColData *data, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfo *pInfo, KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    if (data->prismCache(0) == 0) {
        return false;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupPoint(v0 * invScale, v1 * invScale, mask);

    if (pInfo) {
        return doCheckWithFullInfoPush(data, &KColData::checkPointCollision, pInfo, maskOut);
    }
    return doCheckMaskOnlyPush(data, &KColData::checkPointCollision, maskOut);
}

/// @addr{0x807C4648}
bool CourseColMgr::checkSphereCachedPartial(f32 scale, f32 radius, KColData *data,
        const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask,
        CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    if (data->prismCache(0) == 0) {
        return false;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupSphereCached(pos * invScale, prevPos * invScale, mask, radius * invScale);

    if (info) {
        return doCheckWithPartialInfo(data, &KColData::checkSphereCollision, info, maskOut);
    }

    return doCheckMaskOnly(data, &KColData::checkSphereCollision, maskOut);
}

/// @addr{0x807C47F0}
bool CourseColMgr::checkSphereCachedPartialPush(f32 scale, f32 radius, KColData *data,
        const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask,
        CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    if (data->prismCache(0) == 0) {
        return false;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupSphereCached(pos * invScale, prevPos * invScale, mask, radius * invScale);

    if (info) {
        return doCheckWithPartialInfoPush(data, &KColData::checkSphereCollision, info, maskOut);
    }

    return doCheckMaskOnlyPush(data, &KColData::checkSphereCollision, maskOut);
}

/// @addr{0x807C4998}
bool CourseColMgr::checkSphereCachedFull(f32 scale, f32 radius, KColData *data,
        const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask,
        CollisionInfo *pInfo, KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    if (data->prismCache(0) == 0) {
        return false;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupSphereCached(pos * invScale, prevPos * invScale, mask, radius * invScale);

    if (pInfo) {
        return doCheckWithFullInfo(data, &KColData::checkSphereCollision, pInfo, maskOut);
    }

    return doCheckMaskOnly(data, &KColData::checkSphereCollision, maskOut);
}

/// @addr{0x807C4B40}
bool CourseColMgr::checkSphereCachedFullPush(f32 scale, f32 radius, KColData *data,
        const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask,
        CollisionInfo *colInfo, KCLTypeMask *maskOut) {
    if (!data) {
        data = m_data;
    }

    if (data->prismCache(0) == 0) {
        return false;
    }

    m_kclScale = scale;

    f32 invScale = 1.0f / scale;
    data->lookupSphereCached(pos * invScale, prevPos * invScale, mask, radius * invScale);

    if (colInfo) {
        return doCheckWithFullInfoPush(data, &KColData::checkSphereCollision, colInfo, maskOut);
    }

    return doCheckMaskOnlyPush(data, &KColData::checkSphereCollision, maskOut);
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
        CollisionInfoPartial *info, KCLTypeMask *typeMask) {
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
                info->bbox.min = info->bbox.min.minimize(offset);
                info->bbox.max = info->bbox.max.maximize(offset);
            }
        }
    }

    m_localMtx = nullptr;

    return hasCol;
}

/// @addr{0x807C2F18}
bool CourseColMgr::doCheckWithPartialInfoPush(KColData *data, CollisionCheckFunc collisionCheckFunc,
        CollisionInfoPartial *info, KCLTypeMask *typeMask) {
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
                info->bbox.min = info->bbox.min.minimize(offset);
                info->bbox.max = info->bbox.max.maximize(offset);
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

bool CourseColMgr::doCheckMaskOnly(KColData *data, CollisionCheckFunc collisionCheckFunc,
        KCLTypeMask *maskOut) {
    bool hasCol = false;
    f32 dist;
    u16 attribute;

    while ((data->*collisionCheckFunc)(&dist, nullptr, &attribute)) {
        if ((!m_noBounceWallInfo || !(attribute & KCL_SOFT_WALL_MASK)) && maskOut) {
            *maskOut |= KCL_ATTRIBUTE_TYPE_BIT(attribute);
        }
        hasCol = true;
    }

    return hasCol;
}

bool CourseColMgr::doCheckMaskOnlyPush(KColData *data, CollisionCheckFunc collisionCheckFunc,
        KCLTypeMask *maskOut) {
    bool hasCol = false;
    f32 dist;
    u16 attribute;

    while ((data->*collisionCheckFunc)(&dist, nullptr, &attribute)) {
        if ((!m_noBounceWallInfo || !(attribute & KCL_SOFT_WALL_MASK)) && maskOut) {
            CollisionDirector::Instance()->pushCollisionEntry(dist, maskOut,
                    KCL_ATTRIBUTE_TYPE_BIT(attribute), attribute);
        }
        hasCol = true;
    }

    return hasCol;
}

CourseColMgr *CourseColMgr::s_instance = nullptr; ///< @addr{0x809C3C10}

} // namespace Field
