#pragma once

#include "test/Test.hh"

#include <game/system/RaceConfig.hh>

#include <queue>
#include <span>

namespace Test {

struct TestCase {
    std::string name;
    std::string rkgPath;
    std::string krkgPath;
    u16 targetFrame;
};

/// @brief Maintains a set of test cases to check for desyncs.

/// In its current state, the TestDirector takes in a span of data and parses it into a set of test
/// cases. The data is generated via tools/generate_tests.py which itself reads from test cases
/// defined in testCases.json. Each test case contains a name, an RKG file path, a KRKG file path,
/// and the frame Kinoko should try to sync until. Each frame, the TestDirector will compare
/// important datapoints against those defined in the KRKG. If a desync is detected, the
/// TestDirector will log the desyncs from that frame, and move onto the next test case in the
/// queue. After all test cases have been iterated, Kinoko will exit.
class TestDirector {
public:
    TestDirector(const std::span<u8> &suiteData);
    ~TestDirector();

    void parseSuite(EGG::RamStream &stream);
    void init();
    [[nodiscard]] bool calc();
    void test(const TestData &data);
    void writeTestOutput() const;
    bool popTestCase();

    [[nodiscard]] TestData findNextEntry();
    [[nodiscard]] const TestCase &testCase() const;

    [[nodiscard]] bool sync() const;

    static void OnInit(System::RaceConfig *config, void *arg);

private:
    void readHeader();

    template <typename T>
    void checkDesync(const T &t0, const T &t1, const char *name) {
        if (t0 == t1) {
            return;
        }

        if (m_sync) {
            REPORT("Test Case Failed: %s [%d / %d]", testCase().name.c_str(), m_currentFrame,
                    m_frameCount);
        }

        REPORT("DESYNC! Name: %s", name);
        std::string s0(t0);
        std::string s1(t1);
        REPORT("Expected: %s", s0.c_str());
        REPORT("Observed: %s", s1.c_str());

        m_sync = false;
    }

    void checkDesync(const f32 &t0, const f32 &t1, const char *name) {
        if (t0 == t1) {
            return;
        }

        if (m_sync) {
            REPORT("Test Case Failed: %s [%d / %d]", testCase().name.c_str(), m_currentFrame,
                    m_frameCount);
        }

        REPORT("DESYNC! Name: %s", name);
        std::string s0 = std::to_string(t0);
        std::string s1 = std::to_string(t1);
        REPORT("Expected: 0x%08X | %s", f2u(t0), s0.c_str());
        REPORT("Observed: 0x%08X | %s", f2u(t1), s1.c_str());

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
