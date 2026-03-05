#include "KTestSystem.hh"

#include "host/SceneCreatorDynamic.hh"

#include <game/kart/KartObjectManager.hh>
#include <game/system/RaceManager.hh>

#include <abstract/File.hh>

// We use an unscoped enum to avoid static_casting in all usecases
// This is defined in the source due to its lack of scoping
enum Changelog {
    Initial = 1,
    AddedExtVel = 2,
    AddedIntVel = 3,
    AddedSpeed = 4,
    AddedRotation = 5,
    AddedCheckpoints = 6,
};

struct TestHeader {
    u32 signature;
    u16 byteOrderMark;
    u16 frameCount;
    u16 versionMajor;
    u16 versionMinor;
    u32 dataOffset;
};

/// @brief Initializes the system.
void KTestSystem::init() {
    auto *sceneCreator = new Host::SceneCreatorDynamic;
    m_sceneMgr = new EGG::SceneManager(sceneCreator);

    System::RaceConfig::RegisterInitCallback(OnInit, nullptr);
    Abstract::File::Remove("results.txt");

    if (m_testMode == Host::EOption::Suite) {
        initSuite();
    }

    startNextTestCase();
    m_sceneMgr->changeScene(0);
}

/// @details This reads over the KRKG and generates the list of test cases,
/// before starting the first test case and initializing the race scene.
void KTestSystem::initSuite() {
    constexpr u32 TEST_HEADER_SIGNATURE = 0x54535448; // TSTH
    constexpr u32 TEST_FOOTER_SIGNATURE = 0x54535446; // TSTF
    constexpr u16 SUITE_MAJOR_VER = 1;
    constexpr u16 SUITE_MAX_MINOR_VER = 0;

    u16 numTestCases = m_stream.read_u16();
    u16 testMajorVer = m_stream.read_u16();
    u16 testMinorVer = m_stream.read_u16();

    if (testMajorVer != SUITE_MAJOR_VER || testMinorVer > SUITE_MAX_MINOR_VER) {
        PANIC("Version not supported! Provided file is %d.%d while Kinoko supports up to %d.%d",
                testMajorVer, testMinorVer, SUITE_MAJOR_VER, SUITE_MAX_MINOR_VER);
    }

    for (u16 i = 0; i < numTestCases; ++i) {
        // Validate alignment
        if (m_stream.read_u32() != TEST_HEADER_SIGNATURE) {
            PANIC("Invalid binary data for test case!");
        }

        u16 totalSize = m_stream.read_u16();
        TestCase testCase;

        u16 nameLen = m_stream.read_u16();
        testCase.name = m_stream.read_string();
        if (nameLen != testCase.name.size() + 1) {
            PANIC("Test case name length mismatch!");
        }

        u16 rkgPathLen = m_stream.read_u16();
        testCase.rkgPath = m_stream.read_string();
        if (rkgPathLen != testCase.rkgPath.size() + 1) {
            PANIC("Test case RKG Path length mismatch!");
        }

        u16 krkgPathLen = m_stream.read_u16();
        testCase.krkgPath = m_stream.read_string();
        if (krkgPathLen != testCase.krkgPath.size() + 1) {
            PANIC("Test case KRKG Path length mismatch!");
        }

        testCase.targetFrame = m_stream.read_u16();

        // Validate alignment
        if (m_stream.read_u32() != TEST_FOOTER_SIGNATURE) {
            PANIC("Invalid binary data for test case!");
        }

        if (totalSize != sizeof(u16) * 4 + nameLen + rkgPathLen + krkgPathLen) {
            PANIC("Unexpected bytes in test case");
        }

        m_testCases.push(testCase);
    }
}

/// @brief Executes a frame.
void KTestSystem::calc() {
    m_sceneMgr->calc();
}

/// @brief Executes a run.
/// @details A run consists of iterating over all tests.
/// @return Whether the run was successful or not.
bool KTestSystem::run() {
    bool success = true;

    while (true) {
        success &= runTest();

        if (!popTestCase()) {
            break;
        }

        // TODO: Use a system heap! We currently have a dependency on the scene heap
        m_sceneMgr->destroyScene(m_sceneMgr->currentScene());
        startNextTestCase();
        m_sceneMgr->createScene(2, m_sceneMgr->currentScene());
    }

    return success;
}

/// @brief Parses non-generic command line options.
/// @details Test mode currently accepts either a suite of tests or a RKG + KRKG.
/// @param argc The number of arguments.
/// @param argv The arguments.
void KTestSystem::parseOptions(int argc, char **argv) {
    if (argc < 2) {
        PANIC("Expected suite/ghost/krkg argument!");
    }

    std::optional<char *> rkgPath;
    std::optional<char *> krkgPath;
    std::optional<u16> target;

    for (int i = 0; i < argc; ++i) {
        std::optional<Host::EOption> flag = Host::Option::CheckFlag(argv[i]);
        if (!flag || *flag == Host::EOption::Invalid) {
            WARN("Expected a flag! Got: %s", argv[i]);
            continue;
        }

        switch (*flag) {
        case Host::EOption::Suite: {
            if (m_testMode != Host::EOption::Invalid) {
                PANIC("Mode was already set!");
            }

            m_testMode = Host::EOption::Suite;

            ASSERT(i + 1 < argc);

            size_t size;
            u8 *data = Abstract::File::LoadHost(argv[++i], size);

            if (size == 0) {
                PANIC("Failed to load suite data!");
            }

            m_stream = EGG::RamStream(data, size);
            m_stream.setEndian(std::endian::big);

        } break;
        case Host::EOption::Ghost:
            if (m_testMode != Host::EOption::Invalid && m_testMode != Host::EOption::Ghost) {
                PANIC("Mode was already set!");
            }

            m_testMode = Host::EOption::Ghost;
            ASSERT(i + 1 < argc);
            rkgPath = argv[++i];

            break;
        case Host::EOption::KRKG:
            if (m_testMode != Host::EOption::Invalid && m_testMode != Host::EOption::Ghost) {
                PANIC("Mode was already set!");
            }

            m_testMode = Host::EOption::Ghost;
            ASSERT(i + 1 < argc);
            krkgPath = argv[++i];

            break;
        case Host::EOption::TargetFrame:
            ASSERT(i + 1 < argc);
            {
                if (strlen(argv[++i]) > 5) {
                    PANIC("Target has too many digits");
                }
                target = atoi(argv[i]);
                if (target < 0 || target > std::numeric_limits<u16>::max()) {
                    PANIC("Target is out of bounds (expected 0-65535), got %d\n", *target);
                }
            }

            break;
        case Host::EOption::Invalid:
        default:
            PANIC("Invalid flag!");
            break;
        }
    }

    if (target && m_testMode != Host::EOption::Ghost) {
        PANIC("'--framecount' is only supported in a single ghost test");
    }

    if (m_testMode == Host::EOption::Ghost) {
        if (!rkgPath) {
            PANIC("Missing ghost argument!");
        }

        if (!krkgPath) {
            PANIC("Missing KRKG argument!");
        }

        if (!target) {
            target = 0;
        }

        m_testCases.emplace(*rkgPath, *rkgPath, *krkgPath, *target);
    }
}

KTestSystem *KTestSystem::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new KTestSystem;
    return static_cast<KTestSystem *>(s_instance);
}

void KTestSystem::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

KTestSystem::KTestSystem() : m_testMode(Host::EOption::Invalid) {}

KTestSystem::~KTestSystem() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("KTestSystem instance not explicitly handled!");
    }
}

/// @brief Starts the next test case.
void KTestSystem::startNextTestCase() {
    constexpr u32 KRKG_SIGNATURE = 0x4b524b47; // KRKG

    size_t size;
    u8 *krkg = Abstract::File::LoadHost(getCurrentTestCase().krkgPath.data(), size);
    m_stream = EGG::RamStream(krkg, static_cast<u32>(size));
    m_currentFrame = -1;
    m_sync = true;

    // Initialize endianness for the RAM stream
    u16 mark = reinterpret_cast<TestHeader *>(krkg)->byteOrderMark;
    std::endian endian = parse<u16>(mark) == 0xfeff ? std::endian::big : std::endian::little;
    m_stream.setEndian(endian);

    ASSERT(m_stream.read_u32() == KRKG_SIGNATURE);
    m_stream.skip(2);
    m_frameCount = m_stream.read_u16();
    m_versionMajor = m_stream.read_u16();
    m_versionMinor = m_stream.read_u16();

    ASSERT(m_stream.read_u32() == m_stream.index());

    // If we're in Ghost mode instead of Suite mode and framecount not specified, then target the
    // total framecount of the KRKG.
    if (m_testMode == Host::EOption::Ghost) {
        ASSERT(m_testCases.size() == 1);
        auto &front = m_testCases.front();
        if (front.targetFrame == 0) {
            front.targetFrame = m_frameCount;
        }

        front.targetFrame = std::min(front.targetFrame, m_frameCount);
    }
}

/// @brief Pops the current test case and frees the KRKG buffer.
/// @return Whether the queue still has elements remaining.
bool KTestSystem::popTestCase() {
    ASSERT(m_testCases.size() > 0);
    m_testCases.pop();
    delete[] m_stream.data();

    return !m_testCases.empty();
}

/// @brief Checks one frame in the test.
/// @return Whether the test can continue.
bool KTestSystem::calcTest() {
    ++m_currentFrame;

    // Check if we're out of frames
    u16 targetFrame = getCurrentTestCase().targetFrame;
    ASSERT(targetFrame <= m_frameCount);
    if (m_currentFrame > targetFrame) {
        REPORT("Test Case Passed: %s [%d / %d]", getCurrentTestCase().name.c_str(), targetFrame,
                m_frameCount);
        return false;
    }

    // Test the current frame
    testFrame(findCurrentFrameEntry());
    return m_sync;
}

/// @brief Finds the test data of the current frame.
/// @return The test data of the current frame.
KTestSystem::TestData KTestSystem::findCurrentFrameEntry() {
    EGG::Vector3f pos;
    EGG::Quatf fullRot;
    EGG::Vector3f extVel;
    EGG::Vector3f intVel;
    f32 speed = 0.0f;
    f32 acceleration = 0.0f;
    f32 softSpeedLimit = 0.0f;
    EGG::Quatf mainRot;
    EGG::Vector3f angVel2;
    f32 raceCompletion = 0.0f;
    u16 checkpointId = 0;
    u8 jugemId = 0;

    pos.read(m_stream);
    fullRot.read(m_stream);

    if (m_versionMinor >= Changelog::AddedExtVel) {
        extVel.read(m_stream);
    }

    if (m_versionMinor >= Changelog::AddedIntVel) {
        intVel.read(m_stream);
    }

    if (m_versionMinor >= Changelog::AddedSpeed) {
        speed = m_stream.read_f32();
        acceleration = m_stream.read_f32();
        softSpeedLimit = m_stream.read_f32();
    }

    if (m_versionMinor >= Changelog::AddedRotation) {
        mainRot.read(m_stream);
        angVel2.read(m_stream);
    }

    if (m_versionMinor >= Changelog::AddedCheckpoints) {
        raceCompletion = m_stream.read_f32();
        checkpointId = m_stream.read_u16();
        jugemId = m_stream.read_u8();
        m_stream.skip(1);
    }

    TestData data;
    data.pos = pos;
    data.fullRot = fullRot;
    data.extVel = extVel;
    data.intVel = intVel;
    data.speed = speed;
    data.acceleration = acceleration;
    data.softSpeedLimit = softSpeedLimit;
    data.mainRot = mainRot;
    data.angVel2 = angVel2;
    data.raceCompletion = raceCompletion;
    data.checkpointId = checkpointId;
    data.jugemId = jugemId;
    return data;
}

/// @brief Tests the frame against the provided test data.
/// @param data The test data to compare against.
void KTestSystem::testFrame(const TestData &data) {
    auto *object = Kart::KartObjectManager::Instance()->object(0);
    const auto &pos = object->pos();
    const auto &fullRot = object->fullRot();
    const auto &extVel = object->extVel();
    const auto &intVel = object->intVel();
    f32 speed = object->speed();
    f32 acceleration = object->acceleration();
    f32 softSpeedLimit = object->softSpeedLimit();
    const auto &mainRot = object->mainRot();
    const auto &angVel2 = object->angVel2();

    const auto &player = System::RaceManager::Instance()->player();
    f32 raceCompletion = player.raceCompletion();
    u16 checkpointId = player.checkpointId();
    u8 jugemId = player.jugemId();

    switch (m_versionMinor) {
    case Changelog::AddedCheckpoints:
        checkDesync(data.raceCompletion, raceCompletion, "raceCompletion");
        checkDesync(data.checkpointId, checkpointId, "checkpointId");
        checkDesync(data.jugemId, jugemId, "jugemId");
        [[fallthrough]];
    case Changelog::AddedRotation:
        checkDesync(data.mainRot, mainRot, "mainRot");
        checkDesync(data.angVel2, angVel2, "angVel2");
        [[fallthrough]];
    case Changelog::AddedSpeed:
        checkDesync(data.speed, speed, "speed");
        checkDesync(data.acceleration, acceleration, "acceleration");
        checkDesync(data.softSpeedLimit, softSpeedLimit, "softSpeedLimit");
        [[fallthrough]];
    case Changelog::AddedIntVel:
        checkDesync(data.intVel, intVel, "intVel");
        [[fallthrough]];
    case Changelog::AddedExtVel:
        checkDesync(data.extVel, extVel, "extVel");
        [[fallthrough]];
    default:
        checkDesync(data.pos, pos, "pos");
        checkDesync(data.fullRot, fullRot, "fullRot");
    }
}

/// @brief Runs a single test case, and ends when the test is finished or when a desync is found.
/// @details This will also accumulate results in results.txt.
/// @return Whether the run synchronized or desynchronized.
bool KTestSystem::runTest() {
    while (calcTest()) {
        calc();
    }

    // TODO: Use a system heap! std::string relies on heap allocation
    // The heap is destroyed after this and there is no further allocation, so it's not re-disabled
    m_sceneMgr->currentScene()->heap()->enableAllocation();
    writeTestOutput();
    return m_sync;
}

/// @brief Writes details about the current test to file.
/// @details This is designed to be cumulative across multiple tests.
void KTestSystem::writeTestOutput() const {
    std::string outStr(getCurrentTestCase().name.data());
    outStr += "\n" + std::string(m_sync ? "1" : "0") + "\n";
    outStr += std::to_string(getCurrentTestCase().targetFrame) + "\n";
    outStr += std::to_string(m_frameCount) + "\n";
    Abstract::File::Append("results.txt", outStr.c_str(), outStr.size());
}

/// @brief Gets the current test case.
/// @details In the event that there is no active test case, this gets the next test case.
/// @return The current test case.
const KTestSystem::TestCase &KTestSystem::getCurrentTestCase() const {
    ASSERT(!m_testCases.empty());
    return m_testCases.front();
}

/// @brief Initializes the race configuration as needed for test cases.
/// @param config The race configuration instance.
/// @param arg Unused optional argument.
void KTestSystem::OnInit(System::RaceConfig *config, void * /* arg */) {
    size_t size;
    u8 *rkg = Abstract::File::LoadHost(Instance()->getCurrentTestCase().rkgPath.data(), size);
    config->setGhost(rkg);
    delete[] rkg;

    config->raceScenario().players[0].type = System::RaceConfig::Player::Type::Ghost;
}
