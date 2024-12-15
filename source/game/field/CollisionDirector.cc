#include "CollisionDirector.hh"

#include "game/field/ObjectDrivableDirector.hh"

namespace Field {

/// @addr{0x8078E4F0}
void CollisionDirector::checkCourseColNarrScLocal(f32 radius, const EGG::Vector3f &pos,
        KCLTypeMask mask, bool scaledUp) {
    CourseColMgr::Instance()->scaledNarrowScopeLocal(1.0f, radius, nullptr, pos, mask);
    ObjectDrivableDirector::Instance()->colNarScLocal(radius, radius, pos, mask, scaledUp);
}

/// @addr{0x8078F500}
bool CollisionDirector::checkSphereFull(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CourseColMgr::CollisionInfo *pInfo,
        KCLTypeMask *pFlagsOut, u32 /*start*/) {
    if (pInfo) {
        pInfo->bbox.min = EGG::Vector3f::zero;
        pInfo->bbox.max = EGG::Vector3f::zero;
        pInfo->_50 = -std::numeric_limits<f32>::min();
        pInfo->wallDist = -std::numeric_limits<f32>::min();
        pInfo->floorDist = -std::numeric_limits<f32>::min();
        pInfo->perpendicularity = 0.0f;
    }

    if (pFlagsOut) {
        *pFlagsOut = KCL_NONE;
    }

    auto *courseColMgr = CourseColMgr::Instance();
    auto *noBounceInfo = courseColMgr->noBounceWallInfo();
    if (noBounceInfo) {
        noBounceInfo->bbox.setZero();
        noBounceInfo->dist = std::numeric_limits<f32>::min();
    }

    bool colliding = false;

    if (flags &&
            courseColMgr->checkSphereFull(1.0f, radius, nullptr, v0, v1, flags, pInfo, pFlagsOut)) {
        colliding = true;
    }

    if (colliding) {
        if (pInfo) {
            pInfo->tangentOff = pInfo->bbox.min + pInfo->bbox.max;
        }

        if (noBounceInfo) {
            noBounceInfo->tangentOff = noBounceInfo->bbox.min + noBounceInfo->bbox.max;
        }
    }

    courseColMgr->clearNoBounceWallInfo();

    return colliding;
}

/// @addr{0x8078F784}
bool CollisionDirector::checkSphereFullPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CourseColMgr::CollisionInfo *pInfo,
        KCLTypeMask *pFlagsOut, u32 /*param_8*/) {
    if (pInfo) {
        pInfo->bbox.setZero();
        pInfo->_50 = -std::numeric_limits<f32>::min();
        pInfo->wallDist = -std::numeric_limits<f32>::min();
        pInfo->floorDist = -std::numeric_limits<f32>::min();
        pInfo->perpendicularity = 0.0f;
    }

    if (pFlagsOut) {
        resetCollisionEntries(pFlagsOut);
    }

    auto *courseColMgr = CourseColMgr::Instance();
    auto *noBounceInfo = courseColMgr->noBounceWallInfo();
    if (noBounceInfo) {
        noBounceInfo->bbox.setZero();
        noBounceInfo->dist = std::numeric_limits<f32>::min();
    }

    bool colliding = false;

    if (flags &&
            courseColMgr->checkSphereFullPush(1.0f, radius, nullptr, v0, v1, flags, pInfo,
                    pFlagsOut)) {
        colliding = true;
    }

    if (colliding) {
        if (pInfo) {
            pInfo->tangentOff = pInfo->bbox.min + pInfo->bbox.max;
        }

        if (noBounceInfo) {
            noBounceInfo->tangentOff = noBounceInfo->bbox.min + noBounceInfo->bbox.max;
        }
    }

    courseColMgr->clearNoBounceWallInfo();

    return colliding;
}

/// @addr{0x807901F0}
bool CollisionDirector::checkSphereCachedPartial(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CourseColMgr::CollisionInfo *colInfo,
        KCLTypeMask *typeMaskOut, f32 radius) {
    if (colInfo) {
        colInfo->bbox.setZero();
    }

    if (typeMaskOut) {
        *typeMaskOut = KCL_NONE;
    }

    auto *courseColMgr = CourseColMgr::Instance();
    auto *noBounceInfo = courseColMgr->noBounceWallInfo();
    if (noBounceInfo) {
        noBounceInfo->bbox.setZero();
        noBounceInfo->dist = std::numeric_limits<f32>::min();
    }

    bool hasCourseCol = courseColMgr->checkSphereCachedPartial(nullptr, pos, prevPos, typeMask,
            colInfo, typeMaskOut, 1.0f, radius);

    if (hasCourseCol) {
        if (colInfo) {
            colInfo->tangentOff = colInfo->bbox.min + colInfo->bbox.max;
        }

        if (noBounceInfo) {
            noBounceInfo->tangentOff = noBounceInfo->bbox.min + noBounceInfo->bbox.max;
        }
    }

    courseColMgr->clearNoBounceWallInfo();

    return hasCourseCol;
}

/// @addr{0x807903BC}
bool CollisionDirector::checkSphereCachedPartialPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CourseColMgr::CollisionInfo *colInfo,
        KCLTypeMask *typeMaskOut, f32 radius, u32 /*start*/) {
    if (colInfo) {
        colInfo->bbox.setZero();
    }

    if (typeMaskOut) {
        resetCollisionEntries(typeMaskOut);
    }

    auto *courseColMgr = CourseColMgr::Instance();
    auto *noBounceInfo = courseColMgr->noBounceWallInfo();
    if (noBounceInfo) {
        noBounceInfo->bbox.setZero();
        noBounceInfo->dist = std::numeric_limits<f32>::min();
    }

    bool hasCourseCol = courseColMgr->checkSphereCachedPartialPush(nullptr, pos, prevPos, typeMask,
            colInfo, typeMaskOut, 1.0f, radius);

    courseColMgr->clearNoBounceWallInfo();

    return hasCourseCol;
}

/// @addr{0x807907F8}
bool CollisionDirector::checkSphereCachedFullPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CourseColMgr::CollisionInfo *colInfo,
        KCLTypeMask *typeMaskOut, f32 radius, u32 /*start*/) {
    if (colInfo) {
        colInfo->bbox.min.setZero();
        colInfo->bbox.max.setZero();
        colInfo->_50 = -std::numeric_limits<f32>::min();
        colInfo->wallDist = -std::numeric_limits<f32>::min();
        colInfo->floorDist = -std::numeric_limits<f32>::min();
        colInfo->perpendicularity = 0.0f;
    }

    if (typeMaskOut) {
        resetCollisionEntries(typeMaskOut);
    }

    auto *courseColMgr = CourseColMgr::Instance();
    auto *info = courseColMgr->noBounceWallInfo();
    if (info) {
        info->bbox.setZero();
        info->dist = std::numeric_limits<f32>::min();
    }

    bool hasCourseCol = courseColMgr->checkSphereCachedFullPush(nullptr, pos, prevPos, typeMask,
            colInfo, typeMaskOut, 1.0f, radius);

    if (hasCourseCol) {
        if (colInfo) {
            colInfo->tangentOff = colInfo->bbox.min + colInfo->bbox.max;
        }

        if (info) {
            info->tangentOff = info->bbox.min + info->bbox.max;
        }
    }

    courseColMgr->clearNoBounceWallInfo();

    return hasCourseCol;
}

/// @addr{0x807BDA7C}
void CollisionDirector::resetCollisionEntries(KCLTypeMask *ptr) {
    *ptr = 0;
    m_collisionEntryCount = 0;
    m_closestCollisionEntry = nullptr;
}

/// @brief Called when we find a piece of collision we are touching and want to save it temporarily.
/// @addr{0x807BDA9C}
/// @param dist Distance from player to the KCL traingle center
/// @param typeMask Updated to include kclTypeBit
/// @param kclTypeBit The attribute and additional info about the tri we are colliding with
/// @param attribute The base type of the tri we are colliding with
void CollisionDirector::pushCollisionEntry(f32 dist, KCLTypeMask *typeMask, KCLTypeMask kclTypeBit,
        u16 attribute) {
    *typeMask = *typeMask | kclTypeBit;
    if (m_collisionEntryCount >= m_entries.size()) {
        m_collisionEntryCount = m_entries.size() - 1;
    }

    m_entries[m_collisionEntryCount++] = CollisionEntry(kclTypeBit, attribute, dist);
}

/// @brief Finds the closest KCL triangle out of the list of tris we are colliding with
/// @addr{0x807BD96C}
/// @param type Filters the result for particular KCL types
/// @return Whether there was a collision entry for the provided type
bool CollisionDirector::findClosestCollisionEntry(KCLTypeMask * /*typeMask*/, KCLTypeMask type) {
    m_closestCollisionEntry = nullptr;
    f32 minDist = -std::numeric_limits<f32>::min();

    for (size_t i = 0; i < m_collisionEntryCount; ++i) {
        const auto &entry = m_entries[i];
        u32 typeMask = entry.typeMask & type;
        if (typeMask != 0 && entry.dist > minDist) {
            minDist = entry.dist;
            m_closestCollisionEntry = &entry;
        }
    }

    return !!m_closestCollisionEntry;
}

const CollisionDirector::CollisionEntry *CollisionDirector::closestCollisionEntry() const {
    return m_closestCollisionEntry;
}

/// @addr{0x8078DFE8}
CollisionDirector *CollisionDirector::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new CollisionDirector;
    return s_instance;
}

CollisionDirector *CollisionDirector::Instance() {
    return s_instance;
}

/// @addr{0x8078E124}
void CollisionDirector::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

/// @addr{0x8078E33C}
CollisionDirector::CollisionDirector() {
    m_collisionEntryCount = 0;
    m_closestCollisionEntry = nullptr;
    CourseColMgr::CreateInstance()->init();
}

/// @addr{0x8078E454}
CollisionDirector::~CollisionDirector() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("CollisionDirector instance not explicitly handled!");
    }

    CourseColMgr::DestroyInstance();
}

CollisionDirector *CollisionDirector::s_instance = nullptr; ///< @addr{0x809C2F44}

} // namespace Field
