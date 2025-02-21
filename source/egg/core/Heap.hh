#pragma once

#include "egg/core/Disposer.hh"

#include <abstract/memory/HeapCommon.hh>

#include <list>
#include <new>

namespace EGG {

class ExpHeap;

/// @brief A high-level representation of a memory heap for managing dynamic memory allocation.
/// Interface for allocating and freeing memory blocks.
class Heap : Disposer {
public:
    enum class Kind {
        None,
        Expanded,
        Frame,
        Unit,
        Assert,
    };

    Heap(Abstract::Memory::MEMiHeapHead *handle);
    ~Heap() override;

    virtual void destroy() = 0;
    virtual Kind getHeapKind() const = 0;
    virtual void *alloc(size_t size, s32 align) = 0;
    virtual void free(void *block) = 0;
    virtual u32 getAllocatableSize(s32 align = 4) const = 0;

    void dispose();

    void disableAllocation() {
        m_flags.setBit(eFlags::Lock);
    }

    void enableAllocation() {
        m_flags.resetBit(eFlags::Lock);
    }

    [[nodiscard]] bool tstDisableAllocation() const {
        return m_flags.onBit(eFlags::Lock);
    }

    void appendDisposer(Disposer *disposer) {
        m_children.append(disposer);
    }

    void removeDisposer(Disposer *disposer) {
        m_children.remove(disposer);
    }

    Heap *becomeAllocatableHeap();
    Heap *becomeCurrentHeap();

    void registerHeapBuffer(void *buffer) {
        m_block = buffer;
    }

    [[nodiscard]] void *getStartAddress() {
        return this;
    }

    [[nodiscard]] void *getEndAddress() {
        return m_handle->getHeapEnd();
    }

    [[nodiscard]] const char *getName() const {
        return m_name;
    }

    /// @addr{0x80229AD4}
    [[nodiscard]] Heap *getParentHeap() const {
        return m_parentHeap;
    }

    void setName(const char *name) {
        m_name = name;
    }

    void setParentHeap(Heap *heap) {
        m_parentHeap = heap;
    }

    static void initialize();
    [[nodiscard]] static void *alloc(size_t size, int align, Heap *pHeap);
    static void free(void *block, Heap *pHeap);

    [[nodiscard]] static Heap *findHeap(Abstract::Memory::MEMiHeapHead *handle);
    [[nodiscard]] static Heap *findContainHeap(const void *block);

    [[nodiscard]] static ExpHeap *dynamicCastToExp(Heap *heap) {
        return heap->getHeapKind() == Kind::Expanded ? reinterpret_cast<ExpHeap *>(heap) : nullptr;
    }

    [[nodiscard]] static Heap *getCurrentHeap() {
        return s_currentHeap;
    }

    [[nodiscard]] static constexpr uintptr_t getOffset() {
        // offsetof doesn't work, so instead of hardcoding an offset, we derive it ourselves
        return reinterpret_cast<uintptr_t>(&reinterpret_cast<Heap *>(NULL)->m_link);
    }

protected:
    enum class eFlags {
        Lock = 0,
    };
    typedef TBitFlag<u16, eFlags> Flags;

    Abstract::Memory::MEMiHeapHead *m_handle;
    void *m_block;
    Heap *m_parentHeap;
    Flags m_flags;
    Abstract::Memory::MEMLink m_link;
    Abstract::Memory::MEMList m_children;
    const char *m_name;

    static Abstract::Memory::MEMList s_heapList;

    static Heap *s_currentHeap;
    static Heap *s_allocatableHeap;
};

} // namespace EGG

[[nodiscard]] void *operator new(size_t size) noexcept;
[[nodiscard]] void *operator new(size_t size, int align) noexcept;
[[nodiscard]] void *operator new(size_t size, EGG::Heap *heap, int align) noexcept;
[[nodiscard]] void *operator new[](size_t size) noexcept;
[[nodiscard]] void *operator new[](size_t size, int align) noexcept;
[[nodiscard]] void *operator new[](size_t size, EGG::Heap *heap, int align) noexcept;
void operator delete(void *block) noexcept;
void operator delete[](void *block) noexcept;
