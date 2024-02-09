#pragma once

#include "test/Test.hh"

#include <egg/util/Stream.hh>

#include "game/system/RaceManager.hh"

namespace Test {

class TestDirector {
public:
    TestDirector();
    ~TestDirector();

    bool init();
    bool calc();
    bool test(const TestData &data);

    TestData findNextEntry();

    bool sync() const;

private:
    void readHeader();

    void *m_file;
    EGG::RamStream m_stream;

    u16 m_versionMajor;
    u16 m_versionMinor;
    u16 m_frameCount;
    u16 m_currentFrame;
    bool m_sync;
};

} // namespace Test
