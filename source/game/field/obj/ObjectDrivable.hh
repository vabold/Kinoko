#pragma once

#include "game/field/KCollisionTypes.hh"
#include "game/field/obj/ObjectBase.hh"

namespace Kinoko::Field {

struct CollisionInfo;
struct CollisionInfoPartial;

/// @brief Represents an object that karts can drive on, such as @ref ObjectAurora.
/// @details Exposes collision interfaces to allow for objects to define the criteria for when and
/// how karts react to landing and driving on these objects.
class ObjectDrivable : public ObjectBase {
public:
    ObjectDrivable(const System::MapdataGeoObj &params);
    ~ObjectDrivable() override;

    void load() override;

    [[nodiscard]] f32 getCollisionRadius() const override;

    virtual void initCollision();

    /// @beginPublicColCheckFuncs

    /// @brief Checks collision between a point and the object, writing only partial collision info
    /// @param pos The point to check
    /// @param prevPos The previous position of the point, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving partial collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkPointPartial(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut) = 0;

    /// @brief Checks collision between a point and the object, writing only partial collision info.
    /// Additionally pushes the collision entry into the @ref CollisionDirector cache.
    /// @param pos The point to check
    /// @param prevPos The previous position of the point, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving partial collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkPointPartialPush(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut) = 0;

    /// @brief Checks collision between a point and the object, writing out full collision info
    /// @param pos The point to check
    /// @param prevPos The previous position of the point, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkPointFull(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut) = 0;

    /// @brief Checks collision between a point and the object, writing out full collision info.
    /// Additionally pushes the collision entry into the @ref CollisionDirector cache.
    /// @param pos The point to check
    /// @param prevPos The previous position of the point, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkPointFullPush(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut) = 0;

    /// @brief Checks collision between a sphere and the object, writing only partial collision info
    /// @param radius The radius of the sphere to check
    /// @param pos The position of the sphere to check
    /// @param prevPos The previous position of the sphere, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving partial collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @param timeOffset Optional time offset
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset) = 0;

    /// @brief Checks collision between a sphere and the object, writing partial collision info.
    /// Additionally pushes the collision entry into the @ref CollisionDirector cache.
    /// @param radius The radius of the sphere to check
    /// @param pos The position of the sphere to check
    /// @param prevPos The previous position of the sphere, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving partial collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @param timeOffset Optional time offset
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset) = 0;

    /// @brief Checks collision between a sphere and the object, writing out full collision info
    /// @param radius The radius of the sphere to check
    /// @param pos The position of the sphere to check
    /// @param prevPos The previous position of the sphere, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @param timeOffset Optional time offset
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkSphereFull(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) = 0;

    /// @brief Checks collision between a sphere and the object, writing out full collision info.
    /// Additionally pushes the collision entry into the @ref CollisionDirector cache.
    /// @param radius The radius of the sphere to check
    /// @param pos The position of the sphere to check
    /// @param prevPos The previous position of the sphere, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @param timeOffset Optional time offset
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) = 0;

    /// @brief Narrows the spatial cache of the collision director to only include KCL tris defined
    /// by the provided mask within a certain radius of the given position.
    virtual void narrScLocal(f32 /*radius*/, const EGG::Vector3f & /*pos*/, KCLTypeMask /*mask*/,
            u32 /*timeOffset*/) {}

    /// @brief Checks collision between a point and the object by using the collision director's
    /// local spatial cache, writing only partial collision info
    /// @param pos The point to check
    /// @param prevPos The previous position of the point, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving partial collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkPointCachedPartial(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut) = 0;

    /// @brief Checks collision between a point and the object by using the collision director's
    /// local spatial cache, writing only partial collision info. Additionally pushes the collision
    /// entry into the @ref CollisionDirector cache.
    /// @param pos The point to check
    /// @param prevPos The previous position of the point, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving partial collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkPointCachedPartialPush(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut) = 0;

    /// @brief Checks collision between a point and the object by using the collision director's
    /// local spatial cache, writing out full collision info
    /// @param pos The point to check
    /// @param prevPos The previous position of the point, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkPointCachedFull(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut) = 0;

    /// @brief Checks collision between a point and the object by using the collision director's
    /// local spatial cache, writing out full collision info. Additionally pushes the collision
    /// entry into the @ref CollisionDirector cache.
    /// @param pos The point to check
    /// @param prevPos The previous position of the point, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkPointCachedFullPush(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut) = 0;

    /// @brief Checks collision between a sphere and the object by using the collision director's
    /// local spatial cache, writing only partial collision info
    /// @param radius The radius of the sphere to check
    /// @param pos The position of the sphere to check
    /// @param prevPos The previous position of the sphere, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving partial collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @param timeOffset Optional time offset
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset) = 0;

    /// @brief Checks collision between a sphere and the object by using the collision director's
    /// local spatial cache, writing partial collision info.
    /// Additionally pushes the collision entry into the @ref CollisionDirector cache.
    /// @param radius The radius of the sphere to check
    /// @param pos The position of the sphere to check
    /// @param prevPos The previous position of the sphere, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving partial collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @param timeOffset Optional time offset
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset) = 0;

    /// @brief Checks collision between a sphere and the object by using the collision director's
    /// local spatial cache, writing out full collision info
    /// @param radius The radius of the sphere to check
    /// @param pos The position of the sphere to check
    /// @param prevPos The previous position of the sphere, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @param timeOffset Optional time offset
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkSphereCachedFull(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) = 0;

    /// @brief Checks collision between a sphere and the object by using the collision director's
    /// local spatial cache, writing out full collision info.
    /// Additionally pushes the collision entry into the @ref CollisionDirector cache.
    /// @param radius The radius of the sphere to check
    /// @param pos The position of the sphere to check
    /// @param prevPos The previous position of the sphere, used for calculating collision depth
    /// @param mask The KCL flags to check collision against (other types are ignored)
    /// @param info Out parameter for retrieving collision information (if any)
    /// @param maskOut The KCL flags that were hit during the collision check (if any)
    /// @param timeOffset Optional time offset
    /// @return Whether a collision was detected
    [[nodiscard]] virtual bool checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) = 0;

    /// @endPublicColCheckFuncs

    void loadAABB(f32 radius);
};

} // namespace Kinoko::Field
