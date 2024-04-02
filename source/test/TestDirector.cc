#include "TestDirector.hh"

#include <game/kart/KartObjectManager.hh>

#include <abstract/File.hh>

#include <egg/util/Stream.hh>

#include <cstddef>
#include <cstring>
#include <format>

namespace Test {

TestDirector::TestDirector(std::span<u8> &suiteData) : m_curTestIdx(0) {
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
    m_file = krkg;
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
        return false;
    }

    // Test the current frame
    TestData data = findNextEntry();
    m_sync = test(data);
    return m_sync;
}

bool TestDirector::test(const TestData &data) {
    auto f2pf = [](f32 f) -> std::string { return std::format("{}", f); };

    auto logVectorDesync = [this, &f2pf](const EGG::Vector3f &v0, const EGG::Vector3f &v1,
                                   const char *name) {
        K_LOG("DESYNC! Frame: %d; Name: %s", m_currentFrame, name);
        K_LOG("Expected: [0x%08X, 0x%08X, 0x%08X] | [%s, %s, %s]", f2u(v0.x), f2u(v0.y), f2u(v0.z),
                f2pf(v0.x).c_str(), f2pf(v0.y).c_str(), f2pf(v0.z).c_str());
        K_LOG("Observed: [0x%08X, 0x%08X, 0x%08X] | [%s, %s, %s]", f2u(v1.x), f2u(v1.y), f2u(v1.z),
                f2pf(v1.x).c_str(), f2pf(v1.y).c_str(), f2pf(v1.z).c_str());
    };

    auto logQuatDesync = [this, &f2pf](const EGG::Quatf &q0, const EGG::Quatf &q1,
                                 const char *name) {
        K_LOG("DESYNC! Frame: %d; Name: %s", m_currentFrame, name);
        K_LOG("Expected [0x%08X, 0x%08X, 0x%08X, 0x%08X] | [%s, %s, %s, %s]", f2u(q0.v.x),
                f2u(q0.v.y), f2u(q0.v.z), f2u(q0.w), f2pf(q0.v.x).c_str(), f2pf(q0.v.y).c_str(),
                f2pf(q0.v.z).c_str(), f2pf(q0.w).c_str());
        K_LOG("Observed [0x%08X, 0x%08X, 0x%08X, 0x%08X] | [%s, %s, %s, %s]", f2u(q1.v.x),
                f2u(q1.v.y), f2u(q1.v.z), f2u(q1.w), f2pf(q1.v.x).c_str(), f2pf(q1.v.y).c_str(),
                f2pf(q1.v.z).c_str(), f2pf(q1.w).c_str());
    };

    auto *object = Kart::KartObjectManager::Instance()->object(0);
    const auto &pos = object->pos();
    const auto &fullRot = object->fullRot();

    if (data.pos != pos) {
        logVectorDesync(data.pos, pos, "pos");
        return false;
    }
    if (data.fullRot != fullRot) {
        logQuatDesync(data.fullRot, fullRot, "fullRot");
        return false;
    }

    return true;
}

void TestDirector::writeTestOutput() const {
    std::string outStr(testCase().name.data());
    outStr += "\n" + std::string(m_sync ? "1" : "0") + "\n";
    outStr += std::to_string(testCase().targetFrame) + "\n";
    outStr += std::to_string(m_frameCount) + "\n";
    Abstract::File::Append("results.txt", outStr.c_str(), outStr.size());
}

TestData TestDirector::findNextEntry() {
    EGG::Vector3f pos;
    EGG::Quatf fullRot;
    pos.read(m_stream);
    fullRot.read(m_stream);

    TestData data;
    data.pos = pos;
    data.fullRot = fullRot;
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
