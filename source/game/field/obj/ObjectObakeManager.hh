#pragma once

#include "game/field/ObjectCollisionBox.hh"
#include "game/field/ObjectCollisionSphere.hh"
#include "game/field/obj/ObjectDrivable.hh"
#include "game/field/obj/ObjectObakeBlock.hh"

#include "game/system/map/MapdataGeoObj.hh"

#include <vector>

namespace Field {

/// @brief The manager class for SNES Ghost Valley 2 blocks.
/// @details Blocks are constructed via this manager. When blocks are added, they're placed in a
/// spatially-indexed cache, which reduces the number of collision checks the game would have to
/// perform if blocks were managed like normal objects.
class ObjectObakeManager : public ObjectDrivable {
public:
    ObjectObakeManager(const System::MapdataGeoObj &params);
    ~ObjectObakeManager() override;

    void calc() override;

    /// @addr{0x8080BE9C}
    [[nodiscard]] u32 loadFlags() const override {
        return 3;
    }

    /// @addr{0x8080BE98}
    void createCollision() override {}

    /// @addr{0x8080BE94}
    void calcCollisionTransform() override {}

    /// @addr{0x8080BE84}
    [[nodiscard]] f32 getCollisionRadius() const override {
        return 100000.0f;
    }

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

    void addBlock(const System::MapdataGeoObj &params);

private:
    static constexpr size_t CACHE_SIZE_X = 122;
    static constexpr size_t CACHE_SIZE_Z = 116;
    static constexpr f32 SPECIAL_WALL_BOUNDING_RADIUS = 85.0f;
    static constexpr EGG::Vector3f SPECIAL_WALL_SCALE = EGG::Vector3f(1.0f, 1.1f, 1.0f);
    static constexpr EGG::Vector3f ROAD_SCALE = EGG::Vector3f(1.0f, 0.95f, 1.0f);

    [[nodiscard]] bool checkSpherePartialImpl(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut);
    [[nodiscard]] bool checkSpherePartialPushImpl(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut);
    [[nodiscard]] bool checkSphereFullImpl(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut);
    [[nodiscard]] bool checkSphereFullPushImpl(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut);

    [[nodiscard]] static std::pair<s32, s32> SpatialIndex(const EGG::Vector3f &pos);

    ObjectCollisionBox *m_colBox;       ///< Represents hitbox of a particular block
    ObjectCollisionSphere *m_colSphere; ///< Represents a kart hitbox

    /// Spatially-indexed array of blocks for faster collision lookups.
    std::array<std::array<ObjectObakeBlock *, CACHE_SIZE_X>, CACHE_SIZE_Z> m_blockCache;

    std::vector<ObjectObakeBlock *> m_blocks;     ///< All blocks
    std::vector<ObjectObakeBlock *> m_calcBlocks; ///< Actively falling blocks
};

} // namespace Field
