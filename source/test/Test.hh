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
};

} // namespace Test
