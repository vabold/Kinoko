#pragma once

#include "host/KSystem.hh"

#include <egg/core/SceneManager.hh>
#include <egg/math/Quat.hh>

#include <game/system/RaceConfig.hh>

#include <queue>

/// @brief Kinoko system designed to execute tests.
class KTestSystem final : public KSystem {
public:
    void init() override;
    void calc() override;
    bool run() override;
    void parseOptions(int argc, char **argv) override;

    static KTestSystem *CreateInstance();
    static void DestroyInstance();

    static KTestSystem *Instance() {
        return static_cast<KTestSystem *>(s_instance);
    }

private:
    struct TestCase {
        std::string name;
        std::string rkgPath;
        std::string krkgPath;
        u16 targetFrame;
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
        // Added in 0.6
        f32 raceCompletion;
        u16 checkpointId;
        u8 jugemId;
    };

    KTestSystem();
    KTestSystem(const KTestSystem &) = delete;
    KTestSystem(KTestSystem &&) = delete;
    ~KTestSystem() override;

    template <IntegralType T>
    void checkDesync(const T &t0, const T &t1, const char *name) {
        if (t0 == t1) {
            return;
        }

        if (m_sync) {
            REPORT("Test Case Failed: %s [%d / %d]", getCurrentTestCase().name.c_str(),
                    m_currentFrame, m_frameCount);
        }

        REPORT("DESYNC! Name: %s", name);
        REPORT("Expected: %d", t0);
        REPORT("Observed: %d", t1);

        m_sync = false;
    }

    template <typename T>
    void checkDesync(const T &t0, const T &t1, const char *name) {
        if (t0 == t1) {
            return;
        }

        m_sceneMgr->currentScene()->heap()->enableAllocation();

        if (m_sync) {
            REPORT("Test Case Failed: %s [%d / %d]", getCurrentTestCase().name.c_str(),
                    m_currentFrame, m_frameCount);
        }

        REPORT("DESYNC! Name: %s", name);
        std::string s0(t0);
        std::string s1(t1);
        REPORT("Expected: %s", s0.c_str());
        REPORT("Observed: %s", s1.c_str());

        m_sceneMgr->currentScene()->heap()->disableAllocation();

        m_sync = false;
    }

    void checkDesync(const f32 &t0, const f32 &t1, const char *name) {
        if (t0 == t1) {
            return;
        }

        m_sceneMgr->currentScene()->heap()->enableAllocation();

        if (m_sync) {
            REPORT("Test Case Failed: %s [%d / %d]", getCurrentTestCase().name.c_str(),
                    m_currentFrame, m_frameCount);
        }

        REPORT("DESYNC! Name: %s", name);
        std::string s0 = std::to_string(t0);
        std::string s1 = std::to_string(t1);
        REPORT("Expected: 0x%08X | %s", f2u(t0), s0.c_str());
        REPORT("Observed: 0x%08X | %s", f2u(t1), s1.c_str());

        m_sceneMgr->currentScene()->heap()->disableAllocation();

        m_sync = false;
    }

    void startNextTestCase();
    bool popTestCase();

    bool calcTest();
    TestData findCurrentFrameEntry();
    void testFrame(const TestData &data);

    bool runTest();
    void writeTestOutput() const;

    const TestCase &getCurrentTestCase() const;

    static void OnInit(System::RaceConfig *config, void *arg);

    EGG::SceneManager *m_sceneMgr;
    EGG::RamStream m_stream;
    std::queue<TestCase> m_testCases;

    u16 m_versionMajor;
    u16 m_versionMinor;
    u16 m_frameCount;
    u16 m_currentFrame;
    bool m_sync;
};
