#include "ObjColMgr.hh"

#include "game/field/CourseColMgr.hh"

namespace Field {

/// @addr{0x807C4CE8}
ObjColMgr::ObjColMgr(const void *file)
    : m_mtx(EGG::Matrix34f::ident), m_mtxInv(EGG::Matrix34f::ident), m_kclScale(1.0f),
      m_movingObjVel(EGG::Vector3f::zero) {
    m_data = new KColData(file);
}

/// @addr{0x807C4D6C}
ObjColMgr::~ObjColMgr() {
    ASSERT(m_data);
    delete m_data;
}

/// @addr{0x807C4DC8}
void ObjColMgr::narrScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask flags) {
    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    CourseColMgr::Instance()->scaledNarrowScopeLocal(m_kclScale, radius, m_data, posWrtModel,
            flags);
}

/// @addr{0x807C4E4C}
EGG::Vector3f ObjColMgr::kclLowWorld() const {
    EGG::Vector3f posLocal = m_data->bbox().min * m_kclScale;
    return m_mtx.ps_multVector(posLocal);
}

/// @addr{0x807C4E7C}
EGG::Vector3f ObjColMgr::kclHighWorld() const {
    EGG::Vector3f posLocal = m_data->bbox().max * m_kclScale;
    return m_mtx.ps_multVector(posLocal);
}

/// @addr{0x807C4EAC}
bool ObjColMgr::checkPointPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask flags, CollisionInfoPartial *info, KCLTypeMask *typeMaskOut) {
    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfoPartial tempInfo;
        tempInfo.bbox.setZero();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkPointPartial(m_kclScale, m_data, posWrtModel, prevPosWrtModel, flags,
                    &tempInfo, typeMaskOut)) {
            tempInfo.bbox.min = m_mtx.ps_multVector33(tempInfo.bbox.min);
            tempInfo.bbox.max = m_mtx.ps_multVector33(tempInfo.bbox.max);

            EGG::Vector3f min = tempInfo.bbox.min;
            tempInfo.bbox.min = min.minimize(tempInfo.bbox.max);
            tempInfo.bbox.max = min.maximize(tempInfo.bbox.max);

            info->bbox.min = info->bbox.min.minimize(tempInfo.bbox.min);
            info->bbox.max = info->bbox.max.maximize(tempInfo.bbox.max);

            return true;
        }

        return false;
    }

    return courseColMgr->checkPointPartial(m_kclScale, m_data, posWrtModel, prevPosWrtModel, flags,
            info, typeMaskOut);
}

/// @addr{0x807C506C}
bool ObjColMgr::checkPointPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask flags, CollisionInfoPartial *info, KCLTypeMask *typeMaskOut) {
    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfoPartial tempInfo;
        tempInfo.bbox.setZero();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkPointPartialPush(m_kclScale, m_data, posWrtModel, prevPosWrtModel,
                    flags, &tempInfo, typeMaskOut)) {
            tempInfo.bbox.min = m_mtx.ps_multVector33(tempInfo.bbox.min);
            tempInfo.bbox.max = m_mtx.ps_multVector33(tempInfo.bbox.max);

            EGG::Vector3f min = tempInfo.bbox.min;
            tempInfo.bbox.min = min.minimize(tempInfo.bbox.max);
            tempInfo.bbox.max = min.maximize(tempInfo.bbox.max);

            info->bbox.min = info->bbox.min.minimize(tempInfo.bbox.min);
            info->bbox.max = info->bbox.max.maximize(tempInfo.bbox.max);

            return true;
        }

        return false;
    }

    return courseColMgr->checkPointPartialPush(m_kclScale, m_data, posWrtModel, prevPosWrtModel,
            flags, info, typeMaskOut);
}

/// @addr{0x807C522C}
bool ObjColMgr::checkPointFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask flags, CollisionInfo *info, KCLTypeMask *typeMaskOut) {
    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfo tempInfo;
        tempInfo.reset();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkPointFull(m_kclScale, m_data, posWrtModel, prevPosWrtModel, flags,
                    &tempInfo, typeMaskOut)) {
            info->transformInfo(tempInfo, m_mtx, m_movingObjVel);

            return true;
        }

        return false;
    }

    return courseColMgr->checkPointFull(m_kclScale, m_data, posWrtModel, prevPosWrtModel, flags,
            info, typeMaskOut);
}

/// @addr{0x807C53A4}
bool ObjColMgr::checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask flags, CollisionInfo *info, KCLTypeMask *typeMaskOut) {
    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfo tempInfo;
        tempInfo.reset();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkPointFullPush(m_kclScale, m_data, posWrtModel, prevPosWrtModel,
                    flags, &tempInfo, typeMaskOut)) {
            info->transformInfo(tempInfo, m_mtx, m_movingObjVel);

            return true;
        }

        return false;
    }

    return courseColMgr->checkPointFullPush(m_kclScale, m_data, posWrtModel, prevPosWrtModel, flags,
            info, typeMaskOut);
}

/// @addr{0x807C551C}
bool ObjColMgr::checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfoPartial *info,
        KCLTypeMask *typeMaskOut) {
    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfoPartial tempInfo;
        tempInfo.bbox.setZero();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkSpherePartial(m_kclScale, radius, m_data, posWrtModel,
                    prevPosWrtModel, flags, &tempInfo, typeMaskOut)) {
            tempInfo.bbox.min = m_mtx.ps_multVector33(tempInfo.bbox.min);
            tempInfo.bbox.max = m_mtx.ps_multVector33(tempInfo.bbox.max);

            EGG::Vector3f min = tempInfo.bbox.min;
            tempInfo.bbox.min = min.minimize(tempInfo.bbox.max);
            tempInfo.bbox.max = min.maximize(tempInfo.bbox.max);

            info->bbox.min = info->bbox.min.minimize(tempInfo.bbox.min);
            info->bbox.max = info->bbox.max.maximize(tempInfo.bbox.max);

            return true;
        }

        return false;
    }

    return courseColMgr->checkSpherePartial(m_kclScale, radius, m_data, posWrtModel,
            prevPosWrtModel, flags, info, typeMaskOut);
}

/// @addr{0x807C56F8}
bool ObjColMgr::checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfoPartial *info,
        KCLTypeMask *typeMaskOut) {
    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfoPartial tempInfo;
        tempInfo.bbox.setZero();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkSpherePartialPush(m_kclScale, radius, m_data, posWrtModel,
                    prevPosWrtModel, flags, &tempInfo, typeMaskOut)) {
            tempInfo.bbox.min = m_mtx.ps_multVector33(tempInfo.bbox.min);
            tempInfo.bbox.max = m_mtx.ps_multVector33(tempInfo.bbox.max);

            EGG::Vector3f min = tempInfo.bbox.min;
            tempInfo.bbox.min = min.minimize(tempInfo.bbox.max);
            tempInfo.bbox.max = min.maximize(tempInfo.bbox.max);

            info->bbox.min = info->bbox.min.minimize(tempInfo.bbox.min);
            info->bbox.max = info->bbox.max.maximize(tempInfo.bbox.max);

            return true;
        }

        return false;
    }

    return courseColMgr->checkSpherePartialPush(m_kclScale, radius, m_data, posWrtModel,
            prevPosWrtModel, flags, info, typeMaskOut);
}

/// @addr{0x807C58D4}
bool ObjColMgr::checkSphereFull(f32 radius, const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask flags, CollisionInfo *info, KCLTypeMask *typeMaskOut) {
    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfo tempInfo;
        tempInfo.reset();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkSphereFull(m_kclScale, radius, m_data, posWrtModel, prevPosWrtModel,
                    flags, &tempInfo, typeMaskOut)) {
            info->transformInfo(tempInfo, m_mtx, m_movingObjVel);

            return true;
        }

        return false;
    }

    return courseColMgr->checkSphereFull(m_kclScale, radius, m_data, posWrtModel, prevPosWrtModel,
            flags, info, typeMaskOut);
}

/// @addr{0x807C5A68}
bool ObjColMgr::checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask flags, CollisionInfo *info,
        KCLTypeMask *typeMaskOut) {
    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfo tempInfo;
        tempInfo.reset();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkSphereFullPush(m_kclScale, radius, m_data, posWrtModel,
                    prevPosWrtModel, flags, &tempInfo, typeMaskOut)) {
            info->transformInfo(tempInfo, m_mtx, m_movingObjVel);

            return true;
        }

        return false;
    }

    return courseColMgr->checkSphereFullPush(m_kclScale, radius, m_data, posWrtModel,
            prevPosWrtModel, flags, info, typeMaskOut);
}

/// @addr{0x807C5BFC}
bool ObjColMgr::checkPointCachedPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask flags, CollisionInfoPartial *info, KCLTypeMask *typeMaskOut) {
    if (m_data->prismCache(0) == 0) {
        return false;
    }

    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfoPartial tempInfo;
        tempInfo.bbox.setZero();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkPointCachedPartial(m_kclScale, m_data, posWrtModel, prevPosWrtModel,
                    flags, &tempInfo, typeMaskOut)) {
            tempInfo.bbox.min = m_mtx.ps_multVector33(tempInfo.bbox.min);
            tempInfo.bbox.max = m_mtx.ps_multVector33(tempInfo.bbox.max);

            EGG::Vector3f min = tempInfo.bbox.min;
            tempInfo.bbox.min = min.minimize(tempInfo.bbox.max);
            tempInfo.bbox.max = min.maximize(tempInfo.bbox.max);

            info->bbox.min = info->bbox.min.minimize(tempInfo.bbox.min);
            info->bbox.max = info->bbox.max.maximize(tempInfo.bbox.max);

            return true;
        }

        return false;
    }

    return courseColMgr->checkPointCachedPartial(m_kclScale, m_data, posWrtModel, prevPosWrtModel,
            flags, info, typeMaskOut);
}

/// @addr{0x807C5DD4}
bool ObjColMgr::checkPointCachedPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask flags, CollisionInfoPartial *info, KCLTypeMask *typeMaskOut) {
    if (m_data->prismCache(0) == 0) {
        return false;
    }

    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfoPartial tempInfo;
        tempInfo.bbox.setZero();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkPointCachedPartialPush(m_kclScale, m_data, posWrtModel,
                    prevPosWrtModel, flags, &tempInfo, typeMaskOut)) {
            tempInfo.bbox.min = m_mtx.ps_multVector33(tempInfo.bbox.min);
            tempInfo.bbox.max = m_mtx.ps_multVector33(tempInfo.bbox.max);

            EGG::Vector3f min = tempInfo.bbox.min;
            tempInfo.bbox.min = min.minimize(tempInfo.bbox.max);
            tempInfo.bbox.max = min.maximize(tempInfo.bbox.max);

            info->bbox.min = info->bbox.min.minimize(tempInfo.bbox.min);
            info->bbox.max = info->bbox.max.maximize(tempInfo.bbox.max);

            return true;
        }

        return false;
    }

    return courseColMgr->checkPointCachedPartialPush(m_kclScale, m_data, posWrtModel,
            prevPosWrtModel, flags, info, typeMaskOut);
}

/// @addr{0x807C5FAC}
bool ObjColMgr::checkPointCachedFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask flags, CollisionInfo *info, KCLTypeMask *typeMaskOut) {
    if (m_data->prismCache(0) == 0) {
        return false;
    }

    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfo tempInfo;
        tempInfo.reset();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkPointCachedFull(m_kclScale, m_data, posWrtModel, prevPosWrtModel,
                    flags, &tempInfo, typeMaskOut)) {
            info->transformInfo(tempInfo, m_mtx, m_movingObjVel);

            return true;
        }

        return false;
    }

    return courseColMgr->checkPointCachedFull(m_kclScale, m_data, posWrtModel, prevPosWrtModel,
            flags, info, typeMaskOut);
}

/// @addr{0x807C613C}
bool ObjColMgr::checkPointCachedFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask flags, CollisionInfo *info, KCLTypeMask *typeMaskOut) {
    if (m_data->prismCache(0) == 0) {
        return false;
    }

    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfo tempInfo;
        tempInfo.reset();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkPointCachedFullPush(m_kclScale, m_data, posWrtModel, prevPosWrtModel,
                    flags, &tempInfo, typeMaskOut)) {
            info->transformInfo(tempInfo, m_mtx, m_movingObjVel);

            return true;
        }

        return false;
    }

    return courseColMgr->checkPointCachedFullPush(m_kclScale, m_data, posWrtModel, prevPosWrtModel,
            flags, info, typeMaskOut);
}

/// @addr{0x807C62CC}
bool ObjColMgr::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask typeflags, CollisionInfoPartial *info,
        KCLTypeMask *typeMaskOut) {
    if (m_data->prismCache(0) == 0) {
        return false;
    }

    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfoPartial tempInfo;
        tempInfo.bbox.setZero();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkSphereCachedPartial(m_kclScale, radius, m_data, posWrtModel,
                    prevPosWrtModel, typeflags, &tempInfo, typeMaskOut)) {
            tempInfo.bbox.min = m_mtx.ps_multVector33(tempInfo.bbox.min);
            tempInfo.bbox.max = m_mtx.ps_multVector33(tempInfo.bbox.max);

            EGG::Vector3f min = tempInfo.bbox.min;
            tempInfo.bbox.min = min.minimize(tempInfo.bbox.max);
            tempInfo.bbox.max = min.maximize(tempInfo.bbox.max);

            info->bbox.min = info->bbox.min.minimize(tempInfo.bbox.min);
            info->bbox.max = info->bbox.max.maximize(tempInfo.bbox.max);

            return true;
        }

        return false;
    }

    return courseColMgr->checkSphereCachedPartial(m_kclScale, radius, m_data, posWrtModel,
            prevPosWrtModel, typeflags, info, typeMaskOut);
}

/// @addr{0x807C64C0}
bool ObjColMgr::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask typeflags, CollisionInfoPartial *info,
        KCLTypeMask *typeMaskOut) {
    if (m_data->prismCache(0) == 0) {
        return false;
    }

    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfoPartial tempInfo;
        tempInfo.bbox.setZero();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkSphereCachedPartialPush(m_kclScale, radius, m_data, posWrtModel,
                    prevPosWrtModel, typeflags, &tempInfo, typeMaskOut)) {
            tempInfo.bbox.min = m_mtx.ps_multVector33(tempInfo.bbox.min);
            tempInfo.bbox.max = m_mtx.ps_multVector33(tempInfo.bbox.max);

            EGG::Vector3f min = tempInfo.bbox.min;
            tempInfo.bbox.min = min.minimize(tempInfo.bbox.max);
            tempInfo.bbox.max = min.maximize(tempInfo.bbox.max);

            info->bbox.min = info->bbox.min.minimize(tempInfo.bbox.min);
            info->bbox.max = info->bbox.max.maximize(tempInfo.bbox.max);

            return true;
        }

        return false;
    }

    return courseColMgr->checkSphereCachedPartialPush(m_kclScale, radius, m_data, posWrtModel,
            prevPosWrtModel, typeflags, info, typeMaskOut);
}

/// @addr{0x807C66B4}
bool ObjColMgr::checkSphereCachedFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask typeflags, CollisionInfo *info,
        KCLTypeMask *typeMaskOut) {
    if (m_data->prismCache(0) == 0) {
        return false;
    }

    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfo tempInfo;
        tempInfo.reset();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkSphereCachedFull(m_kclScale, radius, m_data, posWrtModel,
                    prevPosWrtModel, typeflags, &tempInfo, typeMaskOut)) {
            info->transformInfo(tempInfo, m_mtx, m_movingObjVel);

            return true;
        }

        return false;
    }

    return courseColMgr->checkSphereCachedFull(m_kclScale, radius, m_data, posWrtModel,
            prevPosWrtModel, typeflags, info, typeMaskOut);
}

/// @addr{0x807C6860}
bool ObjColMgr::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask typeflags, CollisionInfo *info,
        KCLTypeMask *typeMaskOut) {
    if (m_data->prismCache(0) == 0) {
        return false;
    }

    EGG::Vector3f posWrtModel = m_mtxInv.ps_multVector(pos);
    bool hasPrevY = prevPos.y != std::numeric_limits<f32>::infinity();
    EGG::Vector3f prevPosWrtModel = hasPrevY ? m_mtxInv.ps_multVector(prevPos) : EGG::Vector3f::inf;
    auto *courseColMgr = CourseColMgr::Instance();

    if (info) {
        CollisionInfo tempInfo;
        tempInfo.reset();

        if (courseColMgr->noBounceWallInfo()) {
            courseColMgr->setLocalMtx(&m_mtx);
        }

        if (courseColMgr->checkSphereCachedFullPush(m_kclScale, radius, m_data, posWrtModel,
                    prevPosWrtModel, typeflags, &tempInfo, typeMaskOut)) {
            info->transformInfo(tempInfo, m_mtx, m_movingObjVel);

            return true;
        }

        return false;
    }

    return courseColMgr->checkSphereCachedFullPush(m_kclScale, radius, m_data, posWrtModel,
            prevPosWrtModel, typeflags, info, typeMaskOut);
}

} // namespace Field
