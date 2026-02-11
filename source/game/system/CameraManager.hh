#pragma once

#include "game/render/KartCamera.hh"

#include <egg/core/Disposer.hh>

namespace System {

class CameraManager : EGG::Disposer {
public:
    static CameraManager *CreateInstance();
    static void DestroyInstance();

    [[nodiscard]] static CameraManager *Instance() {
        return s_instance;
    }

private:
    CameraManager();
    ~CameraManager() override;

    static CameraManager *s_instance;
};

} // namespace System
