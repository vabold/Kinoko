#include "FrmHeap.hh"

namespace Kinoko::EGG {

using namespace Abstract::Memory;

FrmHeap::FrmHeap(MEMiHeapHead *handle) : Heap(handle) {}

FrmHeap::~FrmHeap() {
    dispose();
    dynamicCastHandleToFrm()->destroy();
}

FrmHeap *FrmHeap::create(void *startAddress, size_t size, u16 opt) {
    FrmHeap *heap = nullptr;
    void *buffer = startAddress;

    void *endAddress = RoundDown(AddOffset(startAddress, size), 4);
    startAddress = RoundUp(startAddress, 4);

    size_t addrRange = GetAddrNum(endAddress) - GetAddrNum(startAddress);
    if (startAddress > endAddress || addrRange < sizeof(FrmHeap) + 4) {
        return nullptr;
    }

    void *handleStart = AddOffset(startAddress, sizeof(FrmHeap));
    MEMiFrmHeapHead *handle =
            MEMiFrmHeapHead::create(handleStart, addrRange - sizeof(FrmHeap), opt);
    if (handle) {
        heap = new (startAddress) FrmHeap(handle);
        heap->registerHeapBuffer(buffer);
    }

    return heap;
}

FrmHeap *FrmHeap::create(size_t size, Heap *pHeap, u16 opt) {
    FrmHeap *heap = nullptr;

    if (!pHeap) {
        pHeap = Heap::getCurrentHeap();
    }

    if (size == std::numeric_limits<size_t>::max()) {
        size = pHeap->getAllocatableSize();
    }

    void *block = pHeap->alloc(size, 4);
    if (block) {
        heap = create(block, size, opt);
        if (heap) {
            heap->setParentHeap(pHeap);
        } else {
            pHeap->free(block);
        }
    }

    return heap;
}

void FrmHeap::destroy() {
    Heap *pParent = getParentHeap();
    this->~FrmHeap();
    if (pParent) {
        pParent->free(this);
    }
}

void *FrmHeap::alloc(size_t size, s32 align) {
    return dynamicCastHandleToFrm()->alloc(size, align);
}

// Frame heaps can only free to the heap start or heap end depending on flags
// Freeing by block is invalid because we don't recover the free memory, so this is no-op
void FrmHeap::free(void *) {}

u32 FrmHeap::getAllocatableSize(s32 align) const {
    return dynamicCastHandleToFrm()->getAllocatableSize(align);
}

MEMiFrmHeapHead *FrmHeap::dynamicCastHandleToFrm() {
    return reinterpret_cast<MEMiFrmHeapHead *>(m_handle);
}

const MEMiFrmHeapHead *FrmHeap::dynamicCastHandleToFrm() const {
    return reinterpret_cast<MEMiFrmHeapHead *>(m_handle);
}

} // namespace Kinoko::EGG
