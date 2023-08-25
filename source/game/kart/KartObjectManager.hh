#pragma once

#include "game/kart/KartObject.hh"

namespace Kart {

class KartObjectManager {
public:
    void init();
    void calc();

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
