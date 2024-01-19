#include "CollisionDirector.hh"

namespace Field {

bool CollisionDirector::checkSphereFullPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CourseColMgr::CollisionInfo *pInfo,
        KCLTypeMask *pFlagsOut, u32 /*param_8*/) {
    if (pInfo) {
        pInfo->m_bbox.mMin = EGG::Vector3f::zero;
        pInfo->m_bbox.mMax = EGG::Vector3f::zero;
        pInfo->_50 = -std::numeric_limits<f32>::min();
        pInfo->m_wallDist = -std::numeric_limits<f32>::min();
        pInfo->m_floorDist = -std::numeric_limits<f32>::min();
        pInfo->m_perpendicularity = 0.0f;
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
        pInfo->m_minPlusMax = pInfo->m_bbox.mMin + pInfo->m_bbox.mMax;
    }

    return colliding;
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
