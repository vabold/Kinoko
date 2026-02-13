#include "CameraManager.hh"

namespace System {

/// @addr{0x805A83E4}
CameraManager *CameraManager::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new CameraManager;
    return s_instance;
}

/// @addr{0x805A8434}
void CameraManager::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

/// @addr{0x805A8468}
CameraManager::CameraManager() {
    Render::KartCamera::CreateInstance();
}

/// @addr{0x805A8CDC}
CameraManager::~CameraManager() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("CameraManager instance not explicitly handled!");
    }

    Render::KartCamera::DestroyInstance();
}

CameraManager *CameraManager::s_instance = nullptr; ///< @addr{0x809C19B8}

} // namespace System
