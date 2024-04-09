#pragma once

#include "test/Test.hh"

#include <egg/util/Stream.hh>

#include <queue>
#include <span>

namespace Test {

struct TestCase {
    std::string name;
    std::string rkgPath;
    std::string krkgPath;
    u16 targetFrame;
};

class TestDirector {
public:
    TestDirector(std::span<u8> &suiteData);
    ~TestDirector();

    void parseSuite(EGG::RamStream &stream);
    void init();
    bool calc();
    bool test(const TestData &data);
    void writeTestOutput() const;

    TestData findNextEntry();
    const TestCase &testCase() const;

    bool sync() const;

private:
    void readHeader();

    template <typename T>
    bool checkDesync(const T &t0, const T &t1, const char *name) const {
        if (t0 == t1) {
            return true;
        }

        K_LOG("DESYNC! Frame: %d; Name: %s", m_currentFrame, name);
        std::string s0(t0);
        std::string s1(t1);
        K_LOG("Expected: %s", s0.c_str());
        K_LOG("Observed: %s", s1.c_str());

        return false;
    }

    bool checkDesync(const f32 &t0, const f32 &t1, const char *name) const {
        if (t0 == t1) {
            return true;
        }

        K_LOG("DESYNC! Frame: %d; Name: %s", m_currentFrame, name);
        std::string s0 = std::to_string(t0);
        std::string s1 = std::to_string(t1);
        K_LOG("Expected: 0x%08X | %s", f2u(t0), s0.c_str());
        K_LOG("Observed: 0x%08X | %s", f2u(t1), s1.c_str());

        return false;
    }

    std::queue<TestCase> m_testCases;
    u16 m_curTestIdx;

    void *m_file;
    EGG::RamStream m_stream;

    u16 m_versionMajor;
    u16 m_versionMinor;
    u16 m_frameCount;
    u16 m_currentFrame;
    bool m_sync;
};

} // namespace Test
