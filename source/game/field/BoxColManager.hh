#pragma once

#include <egg/core/BitFlag.hh>
#include <egg/core/Heap.hh>
#include <egg/math/Vector.hh>

namespace Kinoko {

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
    Driver = 0,          ///< Indicates the unit is a kart
    Object = 3,          ///< Indicates the unit is a collidable object
    Drivable = 4,        ///< Indicates the unit is a drivable object
    PermRecalcAABB = 8,  ///< Recalculate this unit's spatial indexing every frame
    Intangible = 9,      ///< @ref BoxColManager should ignore collision with the unit
    Active = 10,         ///< Indicates the unit is active
    TempRecalcAABB = 11, ///< Only recalculate this unit's spatial indexing once
};

/// @brief A bitfield of @ref eBoxColFlag that represents the state and type of a given BoxColUnit
typedef EGG::TBitFlag<u16, eBoxColFlag> BoxColFlag;

/// @brief A representation of the boundaries of an entity that has dynamic collision
/// @details Used by the @ref BoxColManager which spatially indexes these units to allow for more
/// efficient collision checks.
struct BoxColUnit {
    BoxColUnit();
    ~BoxColUnit();

    void init(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos, const BoxColFlag &flag,
            void *userData);

    void makeInactive();
    void resize(f32 radius, f32 maxSpeed);
    void reinsert();
    void search(const BoxColFlag &flag);

    const EGG::Vector3f *m_pos; ///< Pointer to the position of the unit in 3D space
    f32 m_radius;               ///< The radius of the unit's bounding sphere
    f32 m_range;                ///< Expanded radius accounting for max speed
    BoxColFlag m_flag;          ///< The flags representing the state and type of the unit
    void *m_userData;           ///< Pointer to the object that this BoxColUnit represents
    s16 m_highPointIdx;         ///< Index of the unit's high point in the spatial index
    s16 m_lowPointIdx;          ///< Index of the unit's low point in the spatial index
    f32 m_xMax;                 ///< Maximum X coordinate of the unit's bounding box
    f32 m_xMin;                 ///< Minimum X coordinate of the unit's bounding box
};

/// @brief Represents the lower Z-axis boundary of a collision unit
struct BoxColLowPoint {
    f32 z;        ///< Z-coordinate of the unit's lower bound
    u8 highPoint; ///< The associated high point index
    u8 unitID;    ///< The ID of the unit associated with this low point
};

/// @brief Represents the upper Z-axis boundary of a collision unit
struct BoxColHighPoint {
    f32 z;          ///< Z-coordinate of the unit's upper bound
    u8 lowPoint;    ///< The associated low point index
    u8 minLowPoint; ///< Minimum low point index for this unit
};

/// @brief Spatial indexing manager for entities with dynamic collision
/// @details Implements a sweep-and-prune algorithm for efficient collision detection. Each unit has
/// two boundary points: the high point (Z-pos + range) and the low point (Z-pos - range), which are
/// sorted in @ref m_highPoints and @ref m_lowPoints respectively. When searching for collisions
/// with a unit, the manager first determines the range of low point indices to search within,
/// eliminating candidates which lie entirely outside the unit's Z-axis range. It then checks the
/// X-axis bounds to eliminate candidates which lie entirely outside the unit's X-axis range. It
/// then checks the candidate's position ± its radius against the query's Z and X bounds. Also
/// implements result caching so that searches with the same parameters can be quickly resolved.
/// Also checks each unit's @ref BoxColFlag bits to determine if it should be considered in
/// collision checks. Once the collision checks have been completed, resulting collision objects can
/// be iterated using the @ref getNextObject and @ref getNextDrivable methods.
class BoxColManager : EGG::Disposer {
    /// @brief Grants access to the singleton so a @ref Host::Context can restore the instance's
    /// state on context switch
    friend class Host::Context;

public:
    void clear();
    void calc();

    [[nodiscard]] BoxColUnit *insertDriver(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos,
            bool alwaysRecalc, Kart::KartObject *kartObject);
    [[nodiscard]] BoxColUnit *insertObject(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos,
            bool alwaysRecalc, void *userData);
    [[nodiscard]] BoxColUnit *insertDrivable(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos,
            bool alwaysRecalc, void *userData);

    [[nodiscard]] ObjectCollidable *getNextObject();
    [[nodiscard]] ObjectDrivable *getNextDrivable();
    [[nodiscard]] bool hasNextObject() const;

    void resetIterators();
    void reinsertUnit(BoxColUnit *unit);
    void remove(BoxColUnit *&unit);
    void search(BoxColUnit *unit, const BoxColFlag &flag);
    void search(f32 radius, const EGG::Vector3f &pos, const BoxColFlag &flag);

    [[nodiscard]] bool isSphereInSpatialCache(f32 radius, const EGG::Vector3f &pos,
            const BoxColFlag &flag) const;

    /// @beginSingletonFuncs

    static BoxColManager *CreateInstance();
    static void DestroyInstance();

    /// @brief Returns the singleton instance of the @ref BoxColManager
    /// @return A pointer to the singleton instance of the @ref BoxColManager
    [[nodiscard]] static BoxColManager *Instance() {
        return s_instance;
    }

    /// @endSingletonFuncs

private:
    EGG_NEW_DELETE_FRIEND

    BoxColManager();
    ~BoxColManager() override;

    [[nodiscard]] BoxColUnit *insert(f32 radius, f32 maxSpeed, const EGG::Vector3f *pos,
            const BoxColFlag &flag, void *userData);

    void iterate(s32 &iter, const BoxColFlag &flag);
    void searchImpl(BoxColUnit *unit, const BoxColFlag &flag);
    void searchImpl(f32 radius, const EGG::Vector3f &pos, const BoxColFlag &flag);
    [[nodiscard]] void *getNextImpl(s32 &id, const BoxColFlag &flag);

    /// @brief The maximum number of collision units that can be managed
    static constexpr size_t MAX_UNIT_COUNT = 0x100;

    std::array<BoxColHighPoint, MAX_UNIT_COUNT> m_highPoints; ///< Upper Z-axis boundary per unit
    std::array<BoxColLowPoint, MAX_UNIT_COUNT> m_lowPoints;   ///< Lower Z-axis boundary per unit
    std::array<BoxColUnit, MAX_UNIT_COUNT> m_unitPool;        ///< Pool of all allocated units
    std::array<BoxColUnit *, MAX_UNIT_COUNT> m_units;         ///< Units within our search bounds

    /// @brief Free list of available pool indices; each entry points to the next free slot
    std::array<u32, MAX_UNIT_COUNT> m_unitIDs;

    s32 m_unitCount;            ///< Number of currently active units
    s32 m_nextUnitID;           ///< Head of the free list; next pool index to allocate
    s32 m_nextObjectID;         ///< Iterator index into @ref m_units for object traversal
    s32 m_nextDrivableID;       ///< Iterator index into @ref m_units for drivable traversal
    s32 m_maxID;                ///< Number of valid entries in @ref m_units after a search
    BoxColUnit *m_cacheUnit;    ///< Unit from the last unit search; `nullptr` for sphere searches
    EGG::Vector3f m_cachePoint; ///< Center position of the last sphere search
    f32 m_cacheRadius; ///< Radius of last sphere search; `-1` for unit searches or invalid cache
    BoxColFlag m_cacheFlag; ///< Flag filter used in the last search

    /// @addr{0x809C2EF0}
    /// @brief The pointer to the singleton instance of the class
    static BoxColManager *s_instance;
};

} // namespace Field

} // namespace Kinoko
