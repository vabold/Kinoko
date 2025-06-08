#include "BoxColManager.hh"

#include "game/field/obj/ObjectCollidable.hh"
#include "game/field/obj/ObjectDrivable.hh"

#include <numeric>

namespace Field {

/// @addr{0x80786ED0}
BoxColUnit::BoxColUnit() : m_pos(nullptr), m_radius(0.0f), m_range(0.0f), m_userData(nullptr) {}

/// @addr{0x80786EF4}
BoxColUnit::~BoxColUnit() = default;

/// @addr{0x80786F34}
void BoxColUnit::init(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos, const BoxColFlag &flag,
        void *userData) {
    m_pos = pos;
    m_radius = radius;
    m_range = radius + maxSpeed;
    m_flag = flag;
    m_flag.setBit(eBoxColFlag::Active);
    m_userData = userData;
    m_xMax = pos->x + m_range;
    m_xMin = pos->x - m_range;
}

/// @addr{0x80786F6C}
void BoxColUnit::makeInactive() {
    m_flag.resetBit(eBoxColFlag::Active);
}

/// @addr{0x80786F7C}
void BoxColUnit::resize(f32 radius, f32 maxSpeed) {
    m_radius = radius;
    m_range = radius + maxSpeed;
    m_flag.setBit(eBoxColFlag::TempRecalcAABB);
}

/// @addr{0x80786F98}
void BoxColUnit::reinsert() {
    BoxColManager::Instance()->reinsertUnit(this);
}

/// @addr{0x80786FA8}
void BoxColUnit::search(const BoxColFlag &flag) {
    BoxColManager::Instance()->search(this, flag);
}

/// @brief Creates two intangible units to represent the spatial bounds.
/// @addr{0x807856E0}
BoxColManager::BoxColManager() {
    constexpr f32 SPATIAL_BOUND = 999999.9f;

    static const EGG::Vector3f upperBound(SPATIAL_BOUND, SPATIAL_BOUND, SPATIAL_BOUND);
    static const EGG::Vector3f lowerBound(-SPATIAL_BOUND, -SPATIAL_BOUND, -SPATIAL_BOUND);

    std::iota(m_unitIDs.begin(), m_unitIDs.end(), 1);

    m_unitCount = 0;
    m_nextUnitID = 0;

    clear();

    BoxColFlag flags;
    insert(1.0f, 0.0f, &upperBound, flags, nullptr)->m_flag.setBit(eBoxColFlag::Intangible);
    insert(1.0f, 0.0f, &lowerBound, flags, nullptr)->m_flag.setBit(eBoxColFlag::Intangible);
}

/// @addr{0x807854E4}
BoxColManager::~BoxColManager() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("BoxColManager instance not explicitly handled!");
    }
}

/// @addr{0x8078597C}
void BoxColManager::clear() {
    m_nextObjectID = MAX_UNIT_COUNT;
    m_nextDrivableID = MAX_UNIT_COUNT;
    m_maxID = 0;
    m_cacheQueryUnit = nullptr;
    m_cacheRadius = -1.0f;
    m_cacheFlag.makeAllZero();
}

/// @brief Recalculate the bounds of all active units having @ref PermRecalcAABB or @ref
/// TempRecalcAABB flag, and then update the low and high points accordingly.
/// @addr{0x807859B0}
void BoxColManager::calc() {
    clear();

    // Update the AABB if necessary
    int activeUnitIdx = 0;
    for (auto &unit : m_unitPool) {
        // Unit is not active, so it's not factored into mUnitCount, skip it
        if (unit.m_flag.offBit(eBoxColFlag::Active)) {
            continue;
        }

        // Unit is active, but we don't need to recalc its AABB, move to the next unit
        if (unit.m_flag.onBit(eBoxColFlag::PermRecalcAABB, eBoxColFlag::TempRecalcAABB)) {
            unit.m_xMax = unit.m_pos->x + unit.m_range;
            unit.m_xMin = unit.m_pos->x - unit.m_range;
            m_highPoints[unit.m_highPointIdx].z = unit.m_pos->z + unit.m_range;
            m_lowPoints[unit.m_lowPointIdx].z = unit.m_pos->z - unit.m_range;

            unit.m_flag.resetBit(eBoxColFlag::TempRecalcAABB);
        }

        // We're done with all of the active units, so we avoid iterating over the remaining
        // inactive units
        if (++activeUnitIdx >= m_unitCount) {
            break;
        }
    }

    // The loops use insertion sort.
    // We assume the units in the spatial index only change in small portions at a time per frame.
    // The time complexity is closer to O(n) as a result rather than O(n^2).

    // Reorganize the high points
    for (size_t i = 1; i < static_cast<size_t>(m_unitCount); ++i) {
        for (size_t j = i; m_highPoints[j - 1].z > m_highPoints[j].z && j >= 1; --j) {
            BoxColHighPoint &upper = m_highPoints[j];
            BoxColHighPoint &lower = m_highPoints[j - 1];

            std::swap(upper, lower);

            BoxColLowPoint &upperLow = m_lowPoints[upper.lowPoint];
            BoxColLowPoint &lowerLow = m_lowPoints[lower.lowPoint];
            ++upperLow.highPoint;
            --lowerLow.highPoint;

            ++m_unitPool[upperLow.unitID].m_highPointIdx;
            --m_unitPool[lowerLow.unitID].m_highPointIdx;

            u8 &nextMinLowPoint = upper.minLowPoint;
            if (nextMinLowPoint == lower.lowPoint) {
                do {
                    ++nextMinLowPoint;
                } while (m_lowPoints[nextMinLowPoint].highPoint < j);
            }

            lower.minLowPoint = std::min(lower.minLowPoint, upper.lowPoint);
        }
    }

    // Reorganize the low points
    for (size_t i = 1; i < static_cast<size_t>(m_unitCount); ++i) {
        for (size_t j = i; m_lowPoints[j - 1].z > m_lowPoints[j].z && j >= 1; --j) {
            BoxColLowPoint &upper = m_lowPoints[j];
            BoxColLowPoint &lower = m_lowPoints[j - 1];

            std::swap(upper, lower);

            ++m_highPoints[upper.highPoint].lowPoint;
            --m_highPoints[lower.highPoint].lowPoint;

            ++m_unitPool[upper.unitID].m_lowPointIdx;
            --m_unitPool[lower.unitID].m_lowPointIdx;

            if (upper.highPoint > lower.highPoint) {
                int k = upper.highPoint;

                while (k > lower.highPoint && m_highPoints[k].minLowPoint == j - 1) {
                    ++m_highPoints[k--].minLowPoint;
                }
            } else {
                int k = lower.highPoint;

                while (k > upper.highPoint && m_highPoints[k].minLowPoint == j) {
                    --m_highPoints[k--].minLowPoint;
                }
            }
        }
    }
}

/// @addr{0x80785E5C}
ObjectCollidable *BoxColManager::getNextObject() {
    return reinterpret_cast<ObjectCollidable *>(getNextImpl(m_nextObjectID, eBoxColFlag::Object));
}

/// @addr{0x80785EC4}
ObjectDrivable *BoxColManager::getNextDrivable() {
    return reinterpret_cast<ObjectDrivable *>(getNextImpl(m_nextDrivableID, eBoxColFlag::Drivable));
}

/// @addr{0x80785F2C}
void BoxColManager::resetIterators() {
    m_nextObjectID = -1;
    iterate(m_nextObjectID, eBoxColFlag::Object);

    m_nextDrivableID = -1;
    iterate(m_nextDrivableID, eBoxColFlag::Drivable);
}

/// @addr{0x80786050}
BoxColUnit *BoxColManager::insertDriver(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos,
        bool alwaysRecalc, Kart::KartObject *kartObject) {
    BoxColFlag flag = BoxColFlag(eBoxColFlag::Driver);

    if (alwaysRecalc) {
        flag.setBit(eBoxColFlag::PermRecalcAABB);
    }

    return insert(radius, maxSpeed, pos, flag, kartObject);
}

/// @addr{0x80786078}
BoxColUnit *BoxColManager::insertObject(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos,
        bool alwaysRecalc, void *userData) {
    BoxColFlag flag = BoxColFlag(eBoxColFlag::Object);

    if (alwaysRecalc) {
        flag.setBit(eBoxColFlag::PermRecalcAABB);
    }

    return insert(radius, maxSpeed, pos, flag, userData);
}

/// @addr{0x80786120}
BoxColUnit *BoxColManager::insertDrivable(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos,
        bool alwaysRecalc, void *userData) {
    BoxColFlag flag = BoxColFlag(eBoxColFlag::Drivable);

    if (alwaysRecalc) {
        flag.setBit(eBoxColFlag::PermRecalcAABB);
    }

    return insert(radius, maxSpeed, pos, flag, userData);
}

/// @addr{0x80786DBC}
void BoxColManager::reinsertUnit(BoxColUnit *unit) {
    f32 radius = unit->m_radius;
    f32 maxSpeed = unit->m_range - radius;
    const EGG::Vector3f *pos = unit->m_pos;
    BoxColFlag flag = unit->m_flag;
    void *userData = unit->m_userData;

    remove(unit);
    insert(radius, maxSpeed, pos, BoxColFlag(), userData)->m_flag = flag;
}

/// @addr{0x80786774}
void BoxColManager::search(BoxColUnit *unit, const BoxColFlag &flag) {
    searchImpl(unit, flag);
    resetIterators();
}

/// @addr{0x80786B14}
void BoxColManager::search(f32 radius, const EGG::Vector3f &pos, const BoxColFlag &flag) {
    searchImpl(radius, pos, flag);
    resetIterators();
}

/// @addr{0x80786E60}
bool BoxColManager::isSphereInSpatialCache(f32 radius, const EGG::Vector3f &pos,
        const BoxColFlag &flag) const {
    if (m_cacheRadius == -1.0f) {
        return false;
    }

    if (!m_cacheFlag.onAll(flag)) {
        return false;
    }

    f32 radiusDiff = m_cacheRadius - radius;
    EGG::Vector3f posDiff = pos - m_cachePoint;

    return EGG::Mathf::abs(posDiff.x) <= radiusDiff && EGG::Mathf::abs(posDiff.z) <= radiusDiff;
}

/// @addr{0x807855DC}
BoxColManager *BoxColManager::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new BoxColManager;
    return s_instance;
}

/// @addr{0x8078562C}
void BoxColManager::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

BoxColManager *BoxColManager::Instance() {
    return s_instance;
}

/// @brief Helper function since the getters share all code except the flag.
void *BoxColManager::getNextImpl(s32 &id, const BoxColFlag &flag) {
    if (id == MAX_UNIT_COUNT) {
        return nullptr;
    }

    BoxColUnit *unit = m_units[id];
    iterate(id, flag);

    return unit->m_userData;
}

/// @addr{Inlined}
void BoxColManager::iterate(s32 &iter, const BoxColFlag &flag) {
    while (++iter < m_maxID) {
        if (m_units[iter]->m_flag.on(flag)) {
            return;
        }
    }

    iter = MAX_UNIT_COUNT;
}

/// @addr{0x80786134}
BoxColUnit *BoxColManager::insert(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos,
        const BoxColFlag &flag, void *userData) {
    if (m_unitCount >= static_cast<s32>(MAX_UNIT_COUNT)) {
        return nullptr;
    }

    s32 unitID = m_nextUnitID;
    BoxColUnit &unit = m_unitPool[unitID];
    unit.init(radius, maxSpeed, pos, flag, userData);
    m_nextUnitID = m_unitIDs[unitID];
    f32 range = radius + maxSpeed;
    f32 zHigh = pos->z + range;
    f32 zLow = pos->z - range;

    if (m_unitCount == 0) {
        m_highPoints[0].lowPoint = 0;
        m_highPoints[0].minLowPoint = 0;
        m_lowPoints[0].unitID = unitID;
        m_highPoints[0].z = zHigh;
        m_lowPoints[0].highPoint = 0;
        m_lowPoints[0].unitID = unitID;
        m_lowPoints[0].z = zLow;
        m_unitPool[0].m_highPointIdx = 0;
        m_unitPool[0].m_lowPointIdx = 0;
        m_unitCount = 1;

        return &unit;
    }

    // Binary search
    int highPointIdx = 0;
    int lowPointIdx = 0;
    int i = m_unitCount;

    while (true) {
        int highSearch = highPointIdx + i;
        int lowSearch = lowPointIdx + i;

        if (highSearch <= m_unitCount && zHigh > m_highPoints[highSearch - 1].z) {
            highPointIdx = highSearch;
        }

        if (lowSearch <= m_unitCount && zLow > m_lowPoints[lowSearch - 1].z) {
            lowPointIdx = lowSearch;
        }

        if (i == 1) {
            break;
        }

        i = (i + 1) / 2;
    }

    unit.m_highPointIdx = highPointIdx;
    unit.m_lowPointIdx = lowPointIdx;

    // Update high points
    for (int i = m_unitCount; i > highPointIdx; --i) {
        BoxColHighPoint &high = m_highPoints[i];
        m_highPoints[i] = m_highPoints[i - 1];
        BoxColLowPoint &low = m_lowPoints[high.lowPoint];

        ++low.highPoint;
        ++m_unitPool[low.unitID].m_highPointIdx;

        if (high.minLowPoint >= lowPointIdx) {
            ++high.minLowPoint;
        }
    }

    m_highPoints[highPointIdx].lowPoint = lowPointIdx;
    m_highPoints[highPointIdx].z = zHigh;

    // Update min low point
    if (highPointIdx == m_unitCount || m_highPoints[highPointIdx + 1].minLowPoint > lowPointIdx) {
        m_highPoints[highPointIdx].minLowPoint = lowPointIdx;

        for (int i = highPointIdx - 1; i >= 0 && m_highPoints[i].minLowPoint > lowPointIdx; --i) {
            m_highPoints[i].minLowPoint = lowPointIdx;
        }
    } else {
        m_highPoints[highPointIdx].minLowPoint = m_highPoints[highPointIdx + 1].minLowPoint;
    }

    // Update low points
    for (int i = m_unitCount; i > lowPointIdx; --i) {
        BoxColLowPoint &low = m_lowPoints[i];
        m_lowPoints[i] = m_lowPoints[i - 1];
        BoxColHighPoint &high = m_highPoints[low.highPoint];
        ++high.lowPoint;
        ++m_unitPool[low.unitID].m_lowPointIdx;
    }

    m_lowPoints[lowPointIdx].highPoint = highPointIdx;
    m_lowPoints[lowPointIdx].unitID = unitID;
    m_lowPoints[lowPointIdx].z = zLow;
    ++m_unitCount;

    return &unit;
}

/// @addr{0x80786578}
void BoxColManager::remove(BoxColUnit *&pUnit) {
    if (!pUnit || pUnit->m_flag.offBit(eBoxColFlag::Active)) {
        return;
    }

    int highPointIdx = pUnit->m_highPointIdx;
    int lowPointIdx = pUnit->m_lowPointIdx;

    // Update high points
    for (int i = highPointIdx; i < m_unitCount - 1; ++i) {
        BoxColHighPoint &high = m_highPoints[i];
        m_highPoints[i] = m_highPoints[i + 1];
        BoxColLowPoint &low = m_lowPoints[high.lowPoint];
        --low.highPoint;
        --m_unitPool[low.unitID].m_highPointIdx;
        if (high.minLowPoint > lowPointIdx) {
            --high.minLowPoint;
        }
    }

    // Update low points
    for (int i = lowPointIdx; i < m_unitCount - 1; ++i) {
        BoxColLowPoint &low = m_lowPoints[i];
        m_lowPoints[i] = m_lowPoints[i + 1];
        BoxColHighPoint &high = m_highPoints[low.highPoint];
        --high.lowPoint;
        --m_unitPool[low.unitID].m_lowPointIdx;

        if (low.highPoint >= highPointIdx) {
            continue;
        }

        int minLowPoint = m_highPoints[low.highPoint].minLowPoint;

        if (minLowPoint != lowPointIdx) {
            continue;
        }

        for (BoxColLowPoint *pLowPoint = &m_lowPoints[minLowPoint];
                pLowPoint->highPoint < low.highPoint; ++minLowPoint) {
            ++pLowPoint;
        }

        m_highPoints[low.highPoint].minLowPoint = minLowPoint;
    }

    pUnit->makeInactive();
    int nextID = pUnit - m_unitPool.data();
    m_unitIDs[nextID] = m_nextUnitID;
    m_nextUnitID = nextID;
    --m_unitCount;
    pUnit = nullptr;
}

/// @addr{0x807868C0}
void BoxColManager::searchImpl(BoxColUnit *unit, const BoxColFlag &flag) {
    if (unit->m_flag.offBit(eBoxColFlag::Active)) {
        return;
    }

    int highPointIdx = unit->m_highPointIdx;
    int lowPointIdx = unit->m_lowPointIdx;
    int origLowPointIdx = unit->m_lowPointIdx;

    f32 highZPos = m_highPoints[highPointIdx].z;
    f32 lowZPos = m_lowPoints[origLowPointIdx].z;

    f32 xMax = unit->m_xMax;
    f32 xMin = unit->m_xMin;

    const EGG::Vector3f *pos = unit->m_pos;
    f32 radius = unit->m_radius;

    f32 zHigh = pos->z + radius;
    f32 zLow = pos->z - radius;
    f32 xHigh = pos->x + radius;
    f32 xLow = pos->x - radius;

    int maxIdx = m_unitCount - 1;

    m_maxID = 0;
    m_cacheQueryUnit = unit;
    m_cacheRadius = -1.0f;
    m_cacheFlag = flag;

    for (; highPointIdx > 7 && m_highPoints[highPointIdx - 8].z >= lowZPos;) {
        highPointIdx -= 8;
    }

    for (; highPointIdx > 0 && m_highPoints[highPointIdx - 1].z >= lowZPos;) {
        --highPointIdx;
    }

    for (; lowPointIdx < maxIdx - 7 && m_lowPoints[lowPointIdx + 8].z <= highZPos;) {
        lowPointIdx += 8;
    }

    for (; lowPointIdx < maxIdx && m_lowPoints[lowPointIdx + 1].z <= highZPos;) {
        ++lowPointIdx;
    }

    u8 minLowPoint = m_highPoints[highPointIdx].minLowPoint;

    for (int i = lowPointIdx; i >= minLowPoint; --i, --lowPointIdx) {
        BoxColLowPoint &low = m_lowPoints[i];

        if (low.highPoint >= highPointIdx && lowPointIdx != origLowPointIdx) {
            BoxColUnit &lowUnit = m_unitPool[low.unitID];

            if (lowUnit.m_xMax < xMin || lowUnit.m_xMin > xMax) {
                continue;
            }

            if (lowUnit.m_flag.off(flag) || lowUnit.m_flag.onBit(eBoxColFlag::Intangible)) {
                continue;
            }

            f32 radius = lowUnit.m_radius;
            if (lowUnit.m_pos->z + radius < zLow || lowUnit.m_pos->z - radius > zHigh) {
                continue;
            }

            if (lowUnit.m_pos->x + radius < xLow || lowUnit.m_pos->x - radius > xHigh) {
                continue;
            }

            m_units[m_maxID++] = &lowUnit;

            if (m_maxID == MAX_UNIT_COUNT) {
                break;
            }
        }

        if (lowPointIdx == 0) {
            break;
        }
    }
}

/// @addr{0x80786C60}
void BoxColManager::searchImpl(f32 radius, const EGG::Vector3f &pos, const BoxColFlag &flag) {
    // Binary search
    int highPointIdx = 0;
    int lowPointIdx = 0;
    f32 zHigh = pos.z + radius;
    f32 zLow = pos.z - radius;
    f32 xHigh = pos.x + radius;
    f32 xLow = pos.x - radius;

    m_maxID = 0;
    m_cacheQueryUnit = nullptr;
    m_cachePoint = pos;
    m_cacheRadius = radius;
    m_cacheFlag = flag;

    int i = m_unitCount - 1;
    while (true) {
        int highSearch = highPointIdx + i;
        int lowSearch = lowPointIdx + i;
        if (highSearch <= m_unitCount && zLow > m_highPoints[highSearch - 1].z) {
            highPointIdx = highSearch;
        }

        if (lowSearch <= m_unitCount && zHigh >= m_lowPoints[lowSearch].z) {
            lowPointIdx = lowSearch;
        }

        if (i == 1) {
            break;
        }

        i = (i + 1) / 2;
    }

    u8 minLowPoint = m_highPoints[highPointIdx].minLowPoint;

    for (i = lowPointIdx; i >= minLowPoint; --i, --lowPointIdx) {
        BoxColLowPoint &low = m_lowPoints[i];
        if (low.highPoint >= highPointIdx) {
            BoxColUnit &unit = m_unitPool[low.unitID];

            if (unit.m_xMax < xLow || unit.m_xMin > xHigh) {
                continue;
            }

            if (unit.m_flag.off(flag) || unit.m_flag.onBit(eBoxColFlag::Intangible)) {
                continue;
            }

            m_units[m_maxID++] = &unit;

            if (m_maxID == MAX_UNIT_COUNT) {
                break;
            }
        }

        if (lowPointIdx == 0) {
            break;
        }
    }
}

BoxColManager *BoxColManager::s_instance = nullptr; ///< @addr{0x809C2EF0}

} // namespace Field
