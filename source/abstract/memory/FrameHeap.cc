#include "FrameHeap.hh"

namespace Kinoko::Abstract::Memory {

MEMiFrmHeapHead::MEMiFrmHeapHead(void *end, u16 opt)
    : MEMiHeapHead(FRM_HEAP_SIGNATURE, AddOffset(this, sizeof(MEMiFrmHeapHead)), end, opt) {
    m_headAllocator = getHeapStart();
    m_tailAllocator = getHeapEnd();
}

/// @addr{0x801994B4}
MEMiFrmHeapHead::~MEMiFrmHeapHead() = default;

/// @addr{0x80199430}
MEMiFrmHeapHead *MEMiFrmHeapHead::create(void *startAddress, size_t size, u16 flag) {
    void *endAddress = AddOffset(startAddress, size);

    startAddress = RoundUp(startAddress, 4);
    endAddress = RoundDown(endAddress, 4);

    uintptr_t startAddrNum = GetAddrNum(startAddress);
    uintptr_t endAddrNum = GetAddrNum(endAddress);

    if (startAddrNum > endAddrNum) {
        return nullptr;
    }

    if (endAddrNum - startAddrNum < sizeof(MEMiFrmHeapHead) + 4) {
        return nullptr;
    }

    return new (startAddress) MEMiFrmHeapHead(endAddress, flag);
}

void MEMiFrmHeapHead::destroy() {
    this->~MEMiFrmHeapHead();
}

/// @addr{0x801994E4}
void *MEMiFrmHeapHead::alloc(size_t size, s32 align) {
    if (size == 0) {
        size = 1;
    }
    size = RoundUp(size, 4);

    void *block = nullptr;
    if (align >= 0) {
        block = allocFromHead(size, align);
    } else {
        block = allocFromTail(size, -align);
    }

    return block;
}

/// @addr{0x801996A4}
u32 MEMiFrmHeapHead::getAllocatableSize(s32 align) const {
    // Doesn't matter which direction it can be allocated from, take absolute value
    align = std::abs(align);
    void *block = RoundUp(m_headAllocator, align);
    return block > m_tailAllocator ? 0 : GetAddrNum(SubOffset(m_tailAllocator, GetAddrNum(block)));
}

void *MEMiFrmHeapHead::calcUsedEnd() {
    return m_tailAllocator == getHeapEnd() ? m_headAllocator : getHeapEnd();
}

void *MEMiFrmHeapHead::allocFromHead(size_t size, s32 alignment) {
    void *start = RoundUp(m_headAllocator, alignment);
    void *end = AddOffset(start, size);

    if (end > m_tailAllocator) {
        return nullptr;
    }

    fillAllocMemory(start, size);
    m_headAllocator = end;
    return start;
}

void *MEMiFrmHeapHead::allocFromTail(size_t size, s32 alignment) {
    void *start = RoundDown(SubOffset(m_tailAllocator, size), alignment);

    if (start < m_headAllocator) {
        return nullptr;
    }

    fillAllocMemory(start, size);
    m_tailAllocator = start;
    return start;
}

} // namespace Kinoko::Abstract::Memory
