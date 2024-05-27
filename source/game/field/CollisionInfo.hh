#pragma once
#include <Common.hh>

#include "egg/math/BoundBox.hh"
#include "egg/math/Vector.hh"

namespace Field
{

/// @brief Structure regrouping information on a particular collision
struct CollisionInfo {
    EGG::BoundBox3f bbox;
    EGG::Vector3f tangentOff;
    EGG::Vector3f floorNrm;
    EGG::Vector3f wallNrm;
    EGG::Vector3f _3c;
    f32 floorDist;
    f32 wallDist;
    f32 _50;
    f32 perpendicularity;

    void updateFloor(f32 dist, const EGG::Vector3f &fnrm);
    void updateWall(f32 dist, const EGG::Vector3f &fnrm);
    void update(f32 now_dist, const EGG::Vector3f &offset, const EGG::Vector3f &fnrm,
            u32 kclAttributeTypeBit);
};

} // namespace Field
