#pragma once

#include <Types.hh>

#include "game/field/obj/ObjectDrivable.hh"

namespace Kinoko::Field {

class ObjectObakeBlock;
class ObjectCollisionBox;
class ObjectCollisionSphere;

/// @brief The manager class for SNES Ghost Valley 2 blocks
/// @details Blocks are constructed via this manager. When blocks are added, they're placed in a
/// spatially-indexed cache, which reduces the number of collision checks the game would have to
/// perform if blocks were managed like normal objects. When a collision check occurs, the manager
/// uses the cache to find the blocks that require collision checks.
class ObjectObakeManager final : public ObjectDrivable {
public:
    ObjectObakeManager(const System::MapdataGeoObj &params);
    ~ObjectObakeManager() override;

    void calc() override;
    void createCollision() override;
    void calcCollisionTransform() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] f32 getCollisionRadius() const override;

    /// @beginPublicColCheckFuncs

    [[nodiscard]] bool checkPointPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) override;
    [[nodiscard]] bool checkPointPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) override;
    [[nodiscard]] bool checkPointFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) override;
    [[nodiscard]] bool checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) override;

    [[nodiscard]] bool checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereFull(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;

    [[nodiscard]] bool checkPointCachedPartial(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut) override;
    [[nodiscard]] bool checkPointCachedPartialPush(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut) override;
    [[nodiscard]] bool checkPointCachedFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) override;
    [[nodiscard]] bool checkPointCachedFullPush(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut) override;

    [[nodiscard]] bool checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereCachedFull(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;

    /// @endPublicColCheckFuncs

    void addBlock(const System::MapdataGeoObj &params);

private:
    static constexpr size_t CACHE_SIZE_X = 122;        ///< Width of the spatial cache
    static constexpr size_t CACHE_SIZE_Z = 116;        ///< Depth of the spatial cache
    static constexpr f32 WALL_BOUNDING_RADIUS = 85.0f; ///< Radius of a block's GJK collision box

    /// @beginPrivateColCheckFuncs

    [[nodiscard]] bool checkSpherePartialImpl(f32 radius, const EGG::Vector3f &pos,
            KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut);
    [[nodiscard]] bool checkSpherePartialPushImpl(f32 radius, const EGG::Vector3f &pos,
            KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut);
    [[nodiscard]] bool checkSphereFullImpl(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
            CollisionInfo *info, KCLTypeMask *maskOut);
    [[nodiscard]] bool checkSphereFullPushImpl(f32 radius, const EGG::Vector3f &pos,
            KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut);

    template <typename T>
        requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
    [[nodiscard]] bool checkSphereImpl(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
            T *info, KCLTypeMask *maskOut, bool push);

    /// @endPrivateColCheckFuncs

    ObjectCollisionBox *m_colBox;       ///< The hitbox of the block to check collision against
    ObjectCollisionSphere *m_colSphere; ///< The kart hitbox to check collision against

    /// @brief Spatially-indexed 122-by-116 array of blocks for faster collision lookups
    std::array<std::array<ObjectObakeBlock *, CACHE_SIZE_X>, CACHE_SIZE_Z> m_blockCache;

    /// @brief Vector of all blocks
    /// @details OPT: Sorted by the frame each block starts falling, with blocks that
    /// never fall placed last. This way, the falling blocks are always the first @ref
    /// m_fallingBlockCount blocks, so the base game's separate array of falling blocks isn't
    /// needed, and @ref calc() only checks the blocks that are next to fall.
    fixed_vector<ObjectObakeBlock *> m_blocks;
    size_t m_fallingBlockCount; ///< Number of blocks that have started falling

    [[nodiscard]] static std::pair<s32, s32> SpatialIndex(const EGG::Vector3f &pos);

    /// @brief Scale of a block's wall collision
    static constexpr EGG::Vector3f WALL_SCALE = EGG::Vector3f(1.0f, 1.1f, 1.0f);

    /// @brief Scale of a block's road collision
    static constexpr EGG::Vector3f ROAD_SCALE = EGG::Vector3f(1.0f, 0.95f, 1.0f);

    static constexpr size_t MAX_BLOCKS = 656; ///< Allocated size of m_blocks
};

} // namespace Kinoko::Field
