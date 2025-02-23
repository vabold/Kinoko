#include "ExpHeap.hh"

#include <limits>

using namespace Abstract::Memory;

namespace EGG {

ExpHeap::ExpHeap(MEMiHeapHead *handle) : Heap(handle) {}

/// @addr{0x802269A8}
ExpHeap::~ExpHeap() {
    dispose();
    dynamicCastHandleToExp()->destroy();
}

/// @addr{0x80226A1C}
ExpHeap *ExpHeap::create(void *startAddress, size_t size, u16 opt) {
    ExpHeap *heap = nullptr;
    void *buffer = startAddress;

    void *endAddress = RoundDown(AddOffset(startAddress, size), 4);
    startAddress = RoundUp(startAddress, 4);

    size_t addrRange = GetAddrNum(endAddress) - GetAddrNum(startAddress);
    if (startAddress > endAddress || addrRange < sizeof(ExpHeap) + 4) {
        return nullptr;
    }

    void *handleStart = AddOffset(startAddress, sizeof(ExpHeap));
    MEMiExpHeapHead *handle =
            MEMiExpHeapHead::create(handleStart, addrRange - sizeof(ExpHeap), opt);
    if (handle) {
        heap = new (startAddress) ExpHeap(handle);
        heap->registerHeapBuffer(buffer);
    }

    return heap;
}

/// @addr{0x80226AC8}
ExpHeap *ExpHeap::create(size_t size, Heap *pHeap, u16 opt) {
    ExpHeap *heap = nullptr;

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

/// @addr{0x80226B94}
void ExpHeap::destroy() {
    Heap *pParent = getParentHeap();
    this->~ExpHeap();
    if (pParent) {
        pParent->free(this);
    }
}

/// @addr{0x80226C04}
void *ExpHeap::alloc(size_t size, s32 align) {
    if (tstDisableAllocation()) {
        PANIC("HEAP ALLOC FAIL (%p, %s): Heap is locked", this, m_name);
    }

    return dynamicCastHandleToExp()->alloc(size, align);
}

/// @addr{0x80226C78}
void ExpHeap::free(void *block) {
    dynamicCastHandleToExp()->free(block);
}

/// @addr{0x80226C90}
u32 ExpHeap::getAllocatableSize(s32 align) const {
    return dynamicCastHandleToExp()->getAllocatableSize(align);
}

/// @addr{0x80226CA0}
void ExpHeap::addGroupSize(void *block, MEMiHeapHead * /* heap */, uintptr_t param) {
    MEMiExpBlockHead *blockHead =
            static_cast<MEMiExpBlockHead *>(SubOffset(block, sizeof(MEMiExpBlockHead)));
    u16 groupID = blockHead->m_attribute.fields.groupId;

    GroupSizeRecord *record = reinterpret_cast<GroupSizeRecord *>(param);
    record->addSize(groupID, blockHead->m_size);
}

/// @addr{0x80226CFC}
void ExpHeap::calcGroupSize(GroupSizeRecord *record) {
    record->reset();
    dynamicCastHandleToExp()->visitAllocated(addGroupSize, GetAddrNum(record));
}

/// @addr{0x80226C98}
void ExpHeap::setGroupID(u16 groupID) {
    return dynamicCastHandleToExp()->setGroupID(groupID);
}

u16 ExpHeap::getGroupID() const {
    return dynamicCastHandleToExp()->getGroupID();
}

MEMiExpHeapHead *ExpHeap::dynamicCastHandleToExp() {
    return reinterpret_cast<MEMiExpHeapHead *>(m_handle);
}

const MEMiExpHeapHead *ExpHeap::dynamicCastHandleToExp() const {
    return reinterpret_cast<MEMiExpHeapHead *>(m_handle);
}

/// @addr{0x80226DD0}
ExpHeap::GroupSizeRecord::GroupSizeRecord() {
    reset();
}

/// @addr{0x80226E00}
void ExpHeap::GroupSizeRecord::reset() {
    for (auto &entry : m_entries) {
        entry = 0;
    }
}

size_t ExpHeap::GroupSizeRecord::getGroupSize(u16 groupID) const {
    return m_entries[groupID];
}

/// @addr{0x80226E98}
void ExpHeap::GroupSizeRecord::addSize(u16 groupID, size_t size) {
    m_entries[groupID] += size;
}

} // namespace EGG
