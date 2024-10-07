#include "HeapCommon.hh"

#include <cstring>

namespace Abstract::Memory {

MEMList &MEMiHeapHead::getChildList() {
    return m_childList;
}

void *MEMiHeapHead::getHeapStart() {
    return m_heapStart;
}

void *MEMiHeapHead::getHeapEnd() {
    return m_heapEnd;
}

MEMList &MEMiHeapHead::getRootList() {
    return s_rootList;
}

u32 MEMiHeapHead::getFillVal(FillType type) {
    size_t t = static_cast<std::underlying_type_t<FillType>>(type);
    ASSERT(t < 3);
    return s_fillVals[t];
}

/// @addr{0x80198658}
MEMiHeapHead *MEMiHeapHead::findContainHeap(const void *block) {
    return findContainHeap(&s_rootList, block);
}

/// @addr{0x8019832C}
MEMiHeapHead::MEMiHeapHead(u32 signature, void *heapStart, void *heapEnd, const OptFlag &opt)
    : m_childList(MEMList(getLinkOffset())) {
    m_signature = signature;
    m_heapStart = heapStart;
    m_heapEnd = heapEnd;
    m_optFlag = opt;

    fillNoUseMemory(heapStart, GetAddrNum(heapEnd) - GetAddrNum(heapStart));

    findListContainHeap().append(this);
}

/// @addr{0x801984EC}
MEMiHeapHead::~MEMiHeapHead() {
    findListContainHeap().remove(this);
    m_signature = 0;
}

void MEMiHeapHead::fillNoUseMemory(void *address, u32 size) {
    if (m_optFlag.onBit(eOptFlag::DebugFillAlloc)) {
        memset(address, getFillVal(FillType::NoUse), size);
    }
}

void MEMiHeapHead::fillAllocMemory(void *address, u32 size) {
    if (m_optFlag.onBit(eOptFlag::DebugFillAlloc)) {
        memset(address, getFillVal(FillType::Alloc), size);
    } else if (m_optFlag.onBit(eOptFlag::ZeroFillAlloc)) {
        memset(address, 0, size);
    }
}

void MEMiHeapHead::fillFreeMemory(void *address, u32 size) {
    if (m_optFlag.onBit(eOptFlag::DebugFillAlloc)) {
        memset(address, getFillVal(FillType::Free), size);
    }
}

/// @addr{0x801981EC}
MEMiHeapHead *MEMiHeapHead::findContainHeap(MEMList *list, const void *block) {
    MEMiHeapHead *heap = nullptr;

    while ((heap = reinterpret_cast<MEMiHeapHead *>(list->getNext(heap)))) {
        if (GetAddrNum(heap->m_heapStart) > GetAddrNum(block) ||
                GetAddrNum(block) >= GetAddrNum(heap->m_heapEnd)) {
            continue;
        }

        MEMiHeapHead *search = findContainHeap(&heap->m_childList, block);
        if (search) {
            return search;
        }

        return heap;
    }

    return nullptr;
}

MEMList &MEMiHeapHead::findListContainHeap() const {
    MEMiHeapHead *containHeap = findContainHeap(this);
    return containHeap ? containHeap->getChildList() : getRootList();
}

MEMList MEMiHeapHead::s_rootList = MEMList(MEMiHeapHead::getLinkOffset());

} // namespace Abstract::Memory
