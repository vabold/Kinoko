#pragma once

#include "source/game/kart/KartObject.hh"

namespace Kart {

class KartObjectManager {
public:
    static KartObjectManager *CreateInstance();
    static void DestroyInstance();
    static KartObjectManager *Instance();

private:
    KartObjectManager();
    ~KartObjectManager();

    size_t m_count;
    KartObject **m_objects;

    static KartObjectManager *s_instance;
};

} // namespace Kart
