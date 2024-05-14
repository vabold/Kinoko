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
    TestDirector(const std::span<u8> &suiteData);
    ~TestDirector();

    void parseSuite(EGG::RamStream &stream);
    void init();
    bool calc();
    void test(const TestData &data);
    void writeTestOutput() const;
    bool popTestCase();

    TestData findNextEntry();
    const TestCase &testCase() const;

    bool sync() const;

private:
    void readHeader();

    template <typename T>
    void checkDesync(const T &t0, const T &t1, const char *name) {
        if (t0 == t1) {
            return;
        }

        if (m_sync) {
            K_LOG("Test Case Failed: %s [%d / %d]", testCase().name.c_str(), m_currentFrame,
                    m_frameCount);
        }

        K_LOG("DESYNC! Name: %s", name);
        std::string s0(t0);
        std::string s1(t1);
        K_LOG("Expected: %s", s0.c_str());
        K_LOG("Observed: %s", s1.c_str());

        m_sync = false;
    }

    void checkDesync(const f32 &t0, const f32 &t1, const char *name) {
        if (t0 == t1) {
            return;
        }

        if (m_sync) {
            K_LOG("Test Case Failed: %s [%d / %d]", testCase().name.c_str(), m_currentFrame,
                    m_frameCount);
        }

        K_LOG("DESYNC! Name: %s", name);
        std::string s0 = std::to_string(t0);
        std::string s1 = std::to_string(t1);
        K_LOG("Expected: 0x%08X | %s", f2u(t0), s0.c_str());
        K_LOG("Observed: 0x%08X | %s", f2u(t1), s1.c_str());

        m_sync = false;
    }

    std::queue<TestCase> m_testCases;

    EGG::RamStream m_stream;

    u16 m_versionMajor;
    u16 m_versionMinor;
    u16 m_frameCount;
    u16 m_currentFrame;
    bool m_sync;
};

} // namespace Test
