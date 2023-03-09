#include "KartParam.hh"

#include "source/game/system/ResourceManager.hh"

namespace Kart {

void KartParam::clear() {
    m_kartParam.clear();
    m_driverParam.clear();
}

void KartParam::init() {
    m_kartParam.load("kartParam.bin");
    m_driverParam.load("driverParam.bin");
    if (!validate()) {
        K_PANIC("Parameter files could not be validated!");
    }
}

KartParam *KartParam::CreateInstance() {
    assert(!s_instance);
    s_instance = new KartParam;
    return s_instance;
}

void KartParam::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

KartParam *KartParam::Instance() {
    return s_instance;
}

KartParam::KartParam() = default;

KartParam::~KartParam() = default;

bool KartParam::validate() {
    // Validate kartParam.bin
    if (!m_kartParam.m_file || m_kartParam.m_size == 0) {
        return false;
    }

    u32 vehicleCount = parse<u32>(*(reinterpret_cast<u32 *>(m_kartParam.m_file)), std::endian::big);
    if (m_kartParam.m_size != vehicleCount * sizeof(Stats) + 4) {
        return false;
    }

    // Validate driverParam.bin
    if (!m_driverParam.m_file || m_driverParam.m_size == 0) {
        return false;
    }

    u32 driverCount = parse<u32>(*(reinterpret_cast<u32 *>(m_driverParam.m_file)), std::endian::big);
    if (m_driverParam.m_size != driverCount * sizeof(Stats) + 4) {
        return false;
    }

    return true;
}

void KartParam::FileInfo::clear() {
    m_file = nullptr;
    m_size = 0;
}

void KartParam::FileInfo::load(const char *filename) {
    auto *resourceManager = System::ResourceManager::Instance();
    m_file = resourceManager->getFile(filename, &m_size, 0);
}

KartParam *KartParam::s_instance = nullptr;

} // namespace Kart
