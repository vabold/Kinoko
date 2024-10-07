#pragma once

#include "abstract/memory/HeapCommon.hh"

#include <new>

namespace EGG {

class ExpHeap;

/// @brief A high-level representation of a memory heap for managing dynamic memory allocation.
/// Interface for allocating and freeing memory blocks.
class Heap {
public:
    enum class Kind {
        None,
        Expanded,
        Frame,
        Unit,
        Assert,
    };

    Heap(Abstract::Memory::MEMiHeapHead *handle);
    virtual ~Heap();

    virtual void destroy() = 0;
    virtual Kind getHeapKind() const = 0;
    virtual void *alloc(size_t size, s32 align) = 0;
    virtual void free(void *block) = 0;
    virtual u32 getAllocatableSize(s32 align = 4) const = 0;

    void disableAllocation();
    void enableAllocation();
    [[nodiscard]] bool tstDisableAllocation() const;

    Heap *becomeAllocatableHeap();
    Heap *becomeCurrentHeap();
    void registerHeapBuffer(void *buffer);

    [[nodiscard]] void *getStartAddress();
    [[nodiscard]] void *getEndAddress();

    [[nodiscard]] const char *getName() const;
    [[nodiscard]] Heap *getParentHeap() const;

    void setName(const char *name);
    void setParentHeap(Heap *heap);

    static void initialize();
    [[nodiscard]] static void *alloc(size_t size, int align, Heap *pHeap);
    static void free(void *block, Heap *pHeap);

    [[nodiscard]] static Heap *findHeap(Abstract::Memory::MEMiHeapHead *handle);
    [[nodiscard]] static Heap *findContainHeap(const void *block);

    [[nodiscard]] static ExpHeap *dynamicCastToExp(Heap *heap);
    [[nodiscard]] static Heap *getCurrentHeap();
    [[nodiscard]] static void *getMemorySpace();

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
    const char *m_name;

    static Abstract::Memory::MEMList s_heapList;

    static Heap *s_currentHeap;
    static bool s_isHeapInitialized;
    static Heap *s_allocatableHeap;
    static ExpHeap *s_rootHeap;
    static void *s_memorySpace;
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
