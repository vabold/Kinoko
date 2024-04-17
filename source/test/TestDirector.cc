#include "TestDirector.hh"

#include <game/kart/KartObjectManager.hh>

#include <abstract/File.hh>

#include <egg/util/Stream.hh>

#include <format>

namespace Test {

// We use an unscoped enum to avoid static_casting in all usecases
// This is defined in the source due to its lack of scoping
enum Changelog {
    Initial = 1,
    AddedExtVel = 2,
    AddedIntVel = 3,
    AddedSpeed = 4,
    AddedRotation = 5,
};

TestDirector::TestDirector(const std::span<u8> &suiteData) {
    Abstract::File::Remove("results.txt");
    EGG::RamStream stream = EGG::RamStream(suiteData.data(), suiteData.size());
    stream.setEndian(std::endian::big);
    parseSuite(stream);
    init();
}

TestDirector::~TestDirector() = default;

void TestDirector::parseSuite(EGG::RamStream &stream) {
    constexpr u32 TEST_HEADER_SIGNATURE = 0x54535448; // TSTH
    constexpr u32 TEST_FOOTER_SIGNATURE = 0x54535446; // TSTF
    constexpr u16 SUITE_MAJOR_VER = 1;
    constexpr u16 SUITE_MAX_MINOR_VER = 0;

    u16 numTestCases = stream.read_u16();
    u16 testMajorVer = stream.read_u16();
    u16 testMinorVer = stream.read_u16();

    if (testMajorVer != SUITE_MAJOR_VER || testMinorVer > SUITE_MAX_MINOR_VER) {
        K_PANIC("Version not supported! Provided file is %d.%d while Kinoko supports up to %d.%d",
                testMajorVer, testMinorVer, SUITE_MAJOR_VER, SUITE_MAX_MINOR_VER);
    }

    for (u16 i = 0; i < numTestCases; ++i) {
        // Validate alignment
        if (stream.read_u32() != TEST_HEADER_SIGNATURE) {
            K_PANIC("Invalid binary data for test case!");
        }

        u16 totalSize = stream.read_u16();
        TestCase testCase;

        u16 nameLen = stream.read_u16();
        testCase.name = stream.read_string();
        if (nameLen != testCase.name.size() + 1) {
            K_PANIC("Test case name length mismatch!");
        }

        u16 rkgPathLen = stream.read_u16();
        testCase.rkgPath = stream.read_string();
        if (rkgPathLen != testCase.rkgPath.size() + 1) {
            K_PANIC("Test case RKG Path length mismatch!");
        }

        u16 krkgPathLen = stream.read_u16();
        testCase.krkgPath = stream.read_string();
        if (krkgPathLen != testCase.krkgPath.size() + 1) {
            K_PANIC("Test case KRKG Path length mismatch!");
        }

        testCase.targetFrame = stream.read_u16();

        // Validate alignment
        if (stream.read_u32() != TEST_FOOTER_SIGNATURE) {
            K_PANIC("Invalid binary data for test case!");
        }

        if (totalSize != sizeof(u16) * 4 + nameLen + rkgPathLen + krkgPathLen) {
            K_PANIC("Unexpected bytes in test case");
        }

        m_testCases.push(testCase);
    }
}

void TestDirector::init() {
    size_t size;
    u8 *krkg = Abstract::File::Load(testCase().krkgPath.data(), size);
    m_stream = EGG::RamStream(krkg, static_cast<u32>(size));
    m_currentFrame = -1;
    m_sync = true;

    // Initialize endianness for the RAM stream
    u16 mark = *reinterpret_cast<u16 *>(krkg + offsetof(TestHeader, byteOrderMark));
    std::endian endian = parse<u16>(mark) == 0xfeff ? std::endian::big : std::endian::little;
    m_stream.setEndian(endian);

    readHeader();

    assert(m_stream.read_u32() == m_stream.index());
}

bool TestDirector::calc() {
    // Check if we're out of frames
    u16 targetFrame = testCase().targetFrame;
    assert(targetFrame <= m_frameCount);
    if (++m_currentFrame > targetFrame) {
        K_LOG("Test Case Passed: %s [%d / %d]", testCase().name.c_str(), targetFrame, m_frameCount);
        return false;
    }

    // Test the current frame
    TestData data = findNextEntry();
    test(data);
    return m_sync;
}

void TestDirector::test(const TestData &data) {
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

    switch (m_versionMinor) {
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

void TestDirector::writeTestOutput() const {
    std::string outStr(testCase().name.data());
    outStr += "\n" + std::string(m_sync ? "1" : "0") + "\n";
    outStr += std::to_string(testCase().targetFrame) + "\n";
    outStr += std::to_string(m_frameCount) + "\n";
    Abstract::File::Append("results.txt", outStr.c_str(), outStr.size());
}

// Pops the first test case in the queue.
// Also free the KRKG buffer for that test case.
// Returns whether or not there are remaining test cases.
bool TestDirector::popTestCase() {
    assert(m_testCases.size() > 0);
    m_testCases.pop();
    delete[] m_stream.data();

    return !m_testCases.empty();
}

TestData TestDirector::findNextEntry() {
    EGG::Vector3f pos;
    EGG::Quatf fullRot;
    EGG::Vector3f extVel;
    EGG::Vector3f intVel;
    f32 speed = 0.0f;
    f32 acceleration = 0.0f;
    f32 softSpeedLimit = 0.0f;
    EGG::Quatf mainRot;
    EGG::Vector3f angVel2;
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
    return data;
}

const TestCase &TestDirector::testCase() const {
    assert(m_testCases.size() > 0);
    return m_testCases.front();
}

bool TestDirector::sync() const {
    return m_sync;
}

void TestDirector::readHeader() {
    constexpr u32 KRKG_SIGNATURE = 0x4b524b47; // KRKG

    assert(m_stream.read_u32() == KRKG_SIGNATURE);
    m_stream.skip(2);
    m_frameCount = m_stream.read_u16();
    m_versionMajor = m_stream.read_u16();
    m_versionMinor = m_stream.read_u16();
}

} // namespace Test
