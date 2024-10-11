#include "Disposer.hh"

#include "egg/core/Heap.hh"

namespace EGG {

/// @addr{0x8021A0F0}
Disposer::Disposer() {
    m_heap = Heap::findContainHeap(this);
    if (m_heap) {
        m_heap->appendDisposer(this);
    }
}

/// @addr{0x8021A144}
Disposer::~Disposer() {
    if (m_heap) {
        m_heap->removeDisposer(this);
    }
}

} // namespace EGG
