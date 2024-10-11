#pragma once

#include <abstract/memory/List.hh>

namespace EGG {

class Heap;

/// @brief An interface for ensuring certain structures and classes are destroyed with the heap.
/// @details Singletons are expected to inherit Disposer.
class Disposer {
    friend class Heap;

protected:
    Disposer();
    virtual ~Disposer();

private:
    Heap *m_heap;
};

} // namespace EGG
