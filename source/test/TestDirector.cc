#include "TestDirector.hh"

#include <game/kart/KartObjectManager.hh>

#include <abstract/File.hh>

#include <cstddef>

namespace Test {

TestDirector::TestDirector() {
    size_t size;
    u8 *file = Abstract::File::Load("Tests/rmc3-rta-1-17-843.krkg", size);
    m_file = file;
    m_stream = EGG::RamStream(file, static_cast<u32>(size));
    m_currentFrame = -1;
    m_sync = true;

    // Initialize endianness for the RAM stream
    u16 mark = *reinterpret_cast<u16 *>(file + offsetof(TestHeader, byteOrderMark));
    std::endian endian = parse<u16>(mark) == 0xfeff ? std::endian::big : std::endian::little;
    m_stream.setEndian(endian);

    readHeader();

    assert(m_stream.read_u32() == m_stream.index());
}

TestDirector::~TestDirector() = default;

bool TestDirector::calc() {
    // Check if we're out of frames
    constexpr u16 TARGET_FRAME = 2;
    assert(TARGET_FRAME <= m_frameCount);
    if (++m_currentFrame > TARGET_FRAME) {
        return false;
    }

    // Test the current frame
    TestData data = findNextEntry();
    m_sync = test(data);
    return m_sync;
}

bool TestDirector::test(const TestData &data) {
    auto logVectorDesync = [this](const EGG::Vector3f &v0, const EGG::Vector3f v1,
                                   const char *name) {
        K_LOG("DESYNC! Frame: %d; Name: %s", m_currentFrame, name);
        K_LOG("Expected [%f, %f, %f], got [%f, %f, %f]", v0.x, v0.y, v0.z, v1.x, v1.y, v1.z);
    };

    auto logQuatDesync = [this](const EGG::Quatf &q0, const EGG::Quatf q1, const char *name) {
        K_LOG("DESYNC! Frame: %d; Name: %s", m_currentFrame, name);
        K_LOG("Expected [%f, %f, %f, %f], got [%f, %f, %f, %f]", q0.v.x, q0.v.y, q0.v.z, q0.w,
                q1.v.x, q1.v.y, q1.v.z, q1.w);
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

bool TestDirector::sync() const {
    return m_sync;
}

void TestDirector::readHeader() {
    assert(m_stream.read_u32() == 0x4b524b47); // 'KRKG'
    m_stream.skip(2);
    m_frameCount = m_stream.read_u16();
    m_versionMajor = m_stream.read_u16();
    m_versionMinor = m_stream.read_u16();
}

} // namespace Test
