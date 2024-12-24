#pragma once

#include <abstract/memory/List.hh>

namespace EGG {

class Heap;

/// @brief An interface for ensuring certain structures and classes are destroyed with the heap.
/// @details Singletons are expected to inherit Disposer.
class Disposer {
    friend class Heap;

public:
    [[nodiscard]] static u16 getLinkOffset() {
        return reinterpret_cast<uintptr_t>(&reinterpret_cast<Disposer *>(NULL)->m_link);
    }

protected:
    Disposer();
    virtual ~Disposer();

private:
    Heap *m_heap;
    Abstract::Memory::MEMLink m_link;
};

} // namespace EGG
