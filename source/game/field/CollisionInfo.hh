#pragma once

#include "game/field/KCollisionTypes.hh"

#include <egg/math/BoundBox.hh>
#include <egg/math/Matrix.hh>

namespace Kinoko::Field {

/// @brief Stores partial information pertaining to a collision
/// @details Each collision updates the bounding box to include a new "push out" vector. The net
/// "push out" vector is calculated as the vector between the minimum and maximum points of the
/// bounding box.
///
/// Some collision queries do not care about which surfaces were hit or how they were oriented. For
/// these simpler queries, we only care about the overall displacement caused by collisions, which
/// is captured by the bounding box.
struct CollisionInfoPartial {
    EGG::BoundBox3f bbox;     ///< Bounding box of "push out" vectors
    EGG::Vector3f tangentOff; ///< The net "push out" vector

    void reset();
    void updateBBox(const EGG::Vector3f &offset);
    void transformInfo(const CollisionInfoPartial &rhs, const EGG::Matrix34f &mtx);
};

/// @brief Stores information pertaining to a collision
/// @details Each collision updates the bounding box to include a new "push out" vector. The net
/// "push out" vector is calculated as the vector between the minimum and maximum points of the
/// bounding box. In addition, tracks the distance to the closest floor, wall, and moving floor
/// KCL during a collision query.
struct CollisionInfo {
    EGG::BoundBox3f bbox;       ///< Bounding box of "push out" vectors
    EGG::Vector3f tangentOff;   ///< The net "push out" vector
    EGG::Vector3f floorNrm;     ///< Colliding floor's up vector
    EGG::Vector3f wallNrm;      ///< Colliding wall face's normal vector
    EGG::Vector3f roadVelocity; ///< Velocity induced by moving object collisions
    f32 floorDist;              ///< Distance from the colliding floor tri
    f32 wallDist;               ///< Distance from the colliding wall tri
    f32 movingFloorDist;        ///< Distance from the colliding moving floor tri
    f32 perpendicularity;       ///< Measures how much two colliding wall normals diverge

    void updateBBox(const EGG::Vector3f &offset);
    void updateFloor(f32 dist, const EGG::Vector3f &fnrm);
    void updateWall(f32 dist, const EGG::Vector3f &fnrm);
    void reset();
    void update(f32 dist, const EGG::Vector3f &offset, const EGG::Vector3f &fnrm, KCLTypeMask mask);
    void transformInfo(const CollisionInfo &rhs, const EGG::Matrix34f &mtx, const EGG::Vector3f &v);
};

} // namespace Kinoko::Field
