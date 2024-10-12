#include "egg/core/ExpHeap.hh"

using namespace Abstract::Memory;

namespace EGG {

/// @addr{0x802296E8}
Heap::Heap(MEMiHeapHead *handle) : m_handle(handle), m_children() {
    if (!s_isHeapInitialized) {
        PANIC("Cannot create a heap before calling Heap::initalize");
    }

    m_block = nullptr;
    m_parentHeap = nullptr;
    m_name = "NoName";
    m_flags.makeAllZero();

    s_heapList.append(this);
}

/// @addr{0x80229780}
Heap::~Heap() {
    s_heapList.remove(this);
}

/// @addr{0x80229C5C}
void Heap::dispose() {
    for (auto *&node : m_children) {
        node->~Disposer();
        node = nullptr;
    }

    m_children.clear();
}

void Heap::disableAllocation() {
    m_flags.setBit(eFlags::Lock);
}

void Heap::enableAllocation() {
    m_flags.resetBit(eFlags::Lock);
}

bool Heap::tstDisableAllocation() const {
    return m_flags.onBit(eFlags::Lock);
}

void Heap::appendDisposer(Disposer *disposer) {
    m_children.push_back(disposer);
}

void Heap::removeDisposer(Disposer *disposer) {
    m_children.remove(disposer);
}

Heap *Heap::becomeAllocatableHeap() {
    Heap *oldHeap = s_allocatableHeap;
    s_allocatableHeap = this;
    return oldHeap;
}

/// @addr{0x80229D74}
Heap *Heap::becomeCurrentHeap() {
    Heap *oldHeap = s_currentHeap;
    s_currentHeap = this;
    return oldHeap;
}

void Heap::registerHeapBuffer(void *buffer) {
    m_block = buffer;
}

void *Heap::getStartAddress() {
    return this;
}

void *Heap::getEndAddress() {
    return m_handle->getHeapEnd();
}

const char *Heap::getName() const {
    return m_name;
}

/// @addr{0x80229AD4}
Heap *Heap::getParentHeap() const {
    return m_parentHeap;
}

void Heap::setName(const char *name) {
    m_name = name;
}

void Heap::setParentHeap(Heap *heap) {
    m_parentHeap = heap;
}

/// @addr{0x802296A8}
void Heap::initialize() {
    constexpr size_t MEMORY_SPACE_SIZE = 0x1000000;

    s_isHeapInitialized = true;

    s_memorySpace = malloc(MEMORY_SPACE_SIZE);
    ExpHeap::initRootHeap(s_memorySpace, MEMORY_SPACE_SIZE);
}

/// @addr{0x80229814}
void *Heap::alloc(size_t size, int align, Heap *pHeap) {
    Heap *currentHeap = s_currentHeap;

    if (s_allocatableHeap) {
        if (currentHeap && !pHeap) {
            pHeap = currentHeap;
        }

        if (pHeap != s_allocatableHeap) {
            WARN("HEAP ALLOC FAIL (%p, %s): Allocatable heap is %p (%s)", pHeap, pHeap->getName(),
                    s_allocatableHeap, s_allocatableHeap->getName());

            return nullptr;
        }
    }

    if (pHeap) {
        return pHeap->alloc(size, align);
    }

    if (currentHeap) {
        void *block = currentHeap->alloc(size, align);

        if (!block) {
            u32 heapFreeSize = currentHeap->getAllocatableSize(0x4);
            s32 heapSize = GetAddrNum(currentHeap->getEndAddress()) -
                    GetAddrNum(currentHeap->getStartAddress());

            constexpr f32 BYTES_TO_MBYTES = 1024.0f * 1024.0f;
            f32 heapSizeMB = static_cast<f32>(heapSize) / BYTES_TO_MBYTES;
            f32 heapFreeSizeMB = static_cast<f32>(heapFreeSize) / BYTES_TO_MBYTES;
            f32 sizeMB = static_cast<f32>(size) / BYTES_TO_MBYTES;

            WARN("HEAP ALLOC FAIL (%p, %s):\nTotal bytes: %d (%.1fMBytes)\nFree bytes: %d "
                 "(%.1fMBytes)\nAlloc bytes: %d "
                 "(%.1fMBytes)\nAlign: %d",
                    currentHeap, currentHeap->getName(), heapSize, heapSizeMB, heapFreeSize,
                    heapFreeSizeMB, size, sizeMB, align);
        }

        return block;
    }

    WARN("HEAP ALLOC FAIL: Cannot allocate %d from heap %p", size, pHeap);
    return nullptr;
}

/// @addr{0x80229B84}
void Heap::free(void *block, Heap *pHeap) {
    if (!pHeap) {
        MEMiHeapHead *handle = MEMiHeapHead::findContainHeap(block);
        if (!handle) {
            return;
        }

        pHeap = findHeap(handle);
        if (!pHeap) {
            return;
        }
    }

    pHeap->free(block);
}

Heap *Heap::findHeap(MEMiHeapHead *handle) {
    Heap *node = nullptr;
    while ((node = reinterpret_cast<Heap *>(s_heapList.getNext(node)))) {
        if (node->m_handle == handle) {
            return node;
        }
    }

    return nullptr;
}

/// @addr{0x80229ADC}
Heap *Heap::findContainHeap(const void *block) {
    MEMiHeapHead *handle = MEMiHeapHead::findContainHeap(block);
    return handle ? findHeap(handle) : nullptr;
}

ExpHeap *Heap::dynamicCastToExp(Heap *heap) {
    return heap->getHeapKind() == Kind::Expanded ? reinterpret_cast<ExpHeap *>(heap) : nullptr;
}

Heap *Heap::getCurrentHeap() {
    return s_currentHeap;
}

void *Heap::getMemorySpace() {
    return s_memorySpace;
}

} // namespace EGG

/// @addr{0x80229DCC}
void *operator new(size_t size) noexcept {
    return EGG::Heap::alloc(size, 4, nullptr);
}

/// @addr{0x80229DD8}
void *operator new(size_t size, int align) noexcept {
    return EGG::Heap::alloc(size, align, nullptr);
}

/// @addr{0x80229DE0}
void *operator new(size_t size, EGG::Heap *heap, int align) noexcept {
    return EGG::Heap::alloc(size, align, heap);
}

/// @addr{0x80229DF0}
void *operator new[](size_t size) noexcept {
    return EGG::Heap::alloc(size, 4, nullptr);
}

/// @addr{0x80229DFC}
void *operator new[](size_t size, int align) noexcept {
    return EGG::Heap::alloc(size, align, nullptr);
}

/// @addr{0x80229E04}
void *operator new[](size_t size, EGG::Heap *heap, int align) noexcept {
    return EGG::Heap::alloc(size, align, heap);
}

/// @addr{0x80229E14}
void operator delete(void *block) noexcept {
    EGG::Heap::free(block, nullptr);
}

void operator delete(void *block, size_t /* size */) noexcept {
    EGG::Heap::free(block, nullptr);
}

/// @addr{0x80229EE0}
void operator delete[](void *block) noexcept {
    EGG::Heap::free(block, nullptr);
}

void operator delete[](void *block, size_t /* size */) noexcept {
    EGG::Heap::free(block, nullptr);
}

MEMList EGG::Heap::s_heapList = MEMList(EGG::Heap::getOffset()); ///< @addr{0x80384320}

EGG::Heap *EGG::Heap::s_currentHeap = nullptr;     ///< @addr{0x80386EA0}
bool EGG::Heap::s_isHeapInitialized = false;       ///< @addr{0x80386EA4}
EGG::Heap *EGG::Heap::s_allocatableHeap = nullptr; ///< @addr{0x80386EA8}
void *EGG::Heap::s_memorySpace = nullptr;
