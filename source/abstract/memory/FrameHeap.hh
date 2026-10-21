#pragma once

#include "abstract/memory/HeapCommon.hh"

namespace Kinoko::Abstract::Memory {

class MEMiFrmHeapHead : public MEMiHeapHead {
private:
    MEMiFrmHeapHead(void *end, u16 opt);
    ~MEMiFrmHeapHead();

public:
    static MEMiFrmHeapHead *create(void *startAddress, size_t size, u16 flag);
    void destroy();

    void *alloc(size_t size, s32 align);
    // Freeing by block is invalid
    // Freeing by flag is not implemented in Kinoko
    // void free(u32 flag);
    [[nodiscard]] u32 getAllocatableSize(s32 align) const;

    void *calcUsedEnd();

private:
    [[nodiscard]] void *allocFromHead(size_t size, s32 alignment);
    [[nodiscard]] void *allocFromTail(size_t size, s32 alignment);

    void *m_headAllocator;
    void *m_tailAllocator;

    static constexpr u32 FRM_HEAP_SIGNATURE = 0x46524d48; // FRMH
};

} // namespace Kinoko::Abstract::Memory
