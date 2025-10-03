#pragma once

#include <egg/core/BitFlag.hh>
#include <egg/math/Vector.hh>

namespace Host {

class Context;

} // namespace Host

namespace Kart {

class KartObject;

} // namespace Kart

namespace Field {

class ObjectCollidable;
class ObjectDrivable;

/// @brief A bitfield that represents the state and type of a given BoxColUnit.
/// @details The lower 8 bits represent the type, while the remaining bits represent the state.
/// There are originally two flags for objects, but one is for CPUs, which we can ignore for now.
enum class eBoxColFlag {
    Driver = 0,
    Object = 3,
    Drivable = 4,
    PermRecalcAABB = 8, ///< Recalculate this unit's spatial indexing every frame.
    Intangible = 9,     ///< Ignore collision with the unit.
    Active = 10,
    TempRecalcAABB = 11, ///< Only recalculate once.
};
typedef EGG::TBitFlag<u32, eBoxColFlag> BoxColFlag;

/// @brief A representation of the boundaries of an entity that has dynamic collision.
struct BoxColUnit {
    BoxColUnit();
    ~BoxColUnit();

    void init(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos, const BoxColFlag &flag,
            void *userData);
    void makeInactive();
    void resize(f32 radius, f32 maxSpeed);
    void reinsert();
    void search(const BoxColFlag &flag);

    const EGG::Vector3f *m_pos;
    f32 m_radius;
    f32 m_range;
    BoxColFlag m_flag;
    void *m_userData;
    s16 m_highPointIdx;
    s16 m_lowPointIdx;
    f32 m_xMax;
    f32 m_xMin;
};

struct BoxColLowPoint {
    f32 z;
    u8 highPoint;
    u8 unitID;
};

struct BoxColHighPoint {
    f32 z;
    u8 lowPoint;
    u8 minLowPoint;
};

/// @brief Spatial indexing manager for entities with dynamic collision.
class BoxColManager : EGG::Disposer {
    friend class Host::Context;

public:
    BoxColManager();
    ~BoxColManager() override;

    void clear();
    void calc();

    [[nodiscard]] ObjectCollidable *getNextObject();
    [[nodiscard]] ObjectDrivable *getNextDrivable();

    void resetIterators();

    [[nodiscard]] BoxColUnit *insertDriver(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos,
            bool alwaysRecalc, Kart::KartObject *kartObject);
    [[nodiscard]] BoxColUnit *insertObject(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos,
            bool alwaysRecalc, void *userData);
    [[nodiscard]] BoxColUnit *insertDrivable(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos,
            bool alwaysRecalc, void *userData);

    void reinsertUnit(BoxColUnit *unit);
    void remove(BoxColUnit *&unit);
    void search(BoxColUnit *unit, const BoxColFlag &flag);
    void search(f32 radius, const EGG::Vector3f &pos, const BoxColFlag &flag);

    [[nodiscard]] bool isSphereInSpatialCache(f32 radius, const EGG::Vector3f &pos,
            const BoxColFlag &flag) const;

    static BoxColManager *CreateInstance();
    static void DestroyInstance();
    [[nodiscard]] static BoxColManager *Instance();

private:
    [[nodiscard]] void *getNextImpl(s32 &id, const BoxColFlag &flag);
    void iterate(s32 &iter, const BoxColFlag &flag);
    [[nodiscard]] BoxColUnit *insert(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos,
            const BoxColFlag &flag, void *userData);
    void searchImpl(BoxColUnit *unit, const BoxColFlag &flag);
    void searchImpl(f32 radius, const EGG::Vector3f &pos, const BoxColFlag &flag);

    static constexpr size_t MAX_UNIT_COUNT = 0x100;

    std::array<BoxColHighPoint, MAX_UNIT_COUNT> m_highPoints; ///< A unit's rightmost Z-axis point.
    std::array<BoxColLowPoint, MAX_UNIT_COUNT> m_lowPoints;   ///< A unit's leftmost Z-axis point;
    std::array<BoxColUnit, MAX_UNIT_COUNT> m_unitPool;        ///< Where all the units live.
    std::array<BoxColUnit *, MAX_UNIT_COUNT> m_units;         ///< Units within our search bounds.

    /// Specifies what unit to retrieve from the pool during allocation.
    std::array<u32, MAX_UNIT_COUNT> m_unitIDs;

    s32 m_unitCount;
    s32 m_nextUnitID;
    s32 m_nextObjectID;
    s32 m_nextDrivableID;
    s32 m_maxID;
    BoxColUnit *m_cacheQueryUnit;
    EGG::Vector3f m_cachePoint;
    f32 m_cacheRadius;
    BoxColFlag m_cacheFlag;

    static BoxColManager *s_instance;
};

} // namespace Field
