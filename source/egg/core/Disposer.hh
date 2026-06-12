#pragma once

#include <abstract/memory/List.hh>

namespace Kinoko::EGG {

class Heap;

/// @brief An interface for ensuring certain structures and classes are destroyed with the heap.
/// @details Singletons are expected to inherit Disposer.
class Disposer {
    friend class Heap;

public:
    #if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Winvalid-constexpr"
    #endif
    [[nodiscard]] static constexpr u16 getLinkOffset() {
        return reinterpret_cast<uintptr_t>(&reinterpret_cast<Disposer *>(0)->m_link);
    }
    #if defined(__clang__)
    #pragma clang diagnostic pop
    #endif

protected:
    Disposer();
    virtual ~Disposer();

private:
    Heap *m_heap;
    Abstract::Memory::MEMLink m_link;
};

} // namespace Kinoko::EGG
