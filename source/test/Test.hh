#pragma once

#include <egg/math/Quat.hh>

namespace Test {

struct TestHeader {
    u32 signature;
    u16 byteOrderMark;
    u16 frameCount;
    u16 versionMajor;
    u16 versionMinor;
    u32 dataOffset;
};

struct TestData {
    EGG::Vector3f pos;
    EGG::Quatf fullRot;
    // Added in 0.2
    EGG::Vector3f extVel;
    // Added in 0.3
    EGG::Vector3f intVel;
    // Added in 0.4
    f32 speed;
    f32 acceleration;
    f32 softSpeedLimit;
    // Added in 0.5
    EGG::Quatf mainRot;
    EGG::Vector3f angVel2;
};

} // namespace Test
