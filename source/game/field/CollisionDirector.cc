#include "CollisionDirector.hh"

namespace Field {

void CollisionDirector::checkCourseColNarrScLocal(f32 radius, const EGG::Vector3f &pos,
        KCLTypeMask mask, u32 /*unused*/) {
    CourseColMgr::Instance()->scaledNarrowScopeLocal(1.0f, radius, nullptr, pos, mask);
}

bool CollisionDirector::checkSphereFullPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CourseColMgr::CollisionInfo *pInfo,
        KCLTypeMask *pFlagsOut, u32 /*param_8*/) {
    if (pInfo) {
        pInfo->bbox.min = EGG::Vector3f::zero;
        pInfo->bbox.max = EGG::Vector3f::zero;
        pInfo->_50 = -std::numeric_limits<f32>::min();
        pInfo->wallDist = -std::numeric_limits<f32>::min();
        pInfo->floorDist = -std::numeric_limits<f32>::min();
        pInfo->perpendicularity = 0.0f;
    }

    if (pFlagsOut) {
        resetCollisionEntries(pFlagsOut);
    }

    bool colliding = false;

    if (flags &&
            CourseColMgr::Instance()->checkSphereFullPush(1.0f, radius, nullptr, v0, v1, flags,
                    pInfo, pFlagsOut)) {
        colliding = true;
    }

    if (colliding && pInfo) {
        pInfo->tangentOff = pInfo->bbox.min + pInfo->bbox.max;
    }

    return colliding;
}

bool CollisionDirector::checkSphereCachedPartialPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CourseColMgr::CollisionInfo *colInfo,
        KCLTypeMask *typeMaskOut, f32 radius, u32 /*start*/) {
    if (colInfo) {
        colInfo->bbox.setZero();
    }

    if (typeMaskOut) {
        resetCollisionEntries(typeMaskOut);
    }

    bool hasCourseCol = CourseColMgr::Instance()->checkSphereCachedPartialPush(nullptr, pos,
            prevPos, typeMask, colInfo, typeMaskOut, 1.0f, radius);

    return hasCourseCol;
}

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

    bool hasCourseCol = CourseColMgr::Instance()->checkSphereCachedFullPush(nullptr, pos, prevPos,
            typeMask, colInfo, typeMaskOut, 1.0f, radius);

    if (hasCourseCol && colInfo) {
        colInfo->tangentOff = colInfo->bbox.min + colInfo->bbox.max;
    }

    return hasCourseCol;
}

void CollisionDirector::resetCollisionEntries(KCLTypeMask *ptr) {
    *ptr = 0;
    m_collisionEntryCount = 0;
    m_closestCollisionEntry = nullptr;
}

void CollisionDirector::pushCollisionEntry(f32 dist, KCLTypeMask *typeMask, KCLTypeMask kclTypeBit,
        u16 attribute) {
    *typeMask = *typeMask | kclTypeBit;
    if (m_collisionEntryCount >= m_entries.size()) {
        m_collisionEntryCount = m_entries.size() - 1;
    }

    m_entries[m_collisionEntryCount++] = CollisionEntry(kclTypeBit, attribute, dist);
}

const CollisionDirector::CollisionEntry *CollisionDirector::closestCollisionEntry() const {
    return m_closestCollisionEntry;
}

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

CollisionDirector *CollisionDirector::CreateInstance() {
    assert(!s_instance);
    s_instance = new CollisionDirector;
    return s_instance;
}

CollisionDirector *CollisionDirector::Instance() {
    return s_instance;
}

void CollisionDirector::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

CollisionDirector::CollisionDirector() {
    m_collisionEntryCount = 0;
    m_closestCollisionEntry = nullptr;
    CourseColMgr::CreateInstance()->init();
}

CollisionDirector::~CollisionDirector() {
    CourseColMgr::DestroyInstance();
}

CollisionDirector *CollisionDirector::s_instance = nullptr;

} // namespace Field
