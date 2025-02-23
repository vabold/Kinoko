#pragma once

#include "abstract/memory/HeapCommon.hh"

#include <functional>

namespace Abstract::Memory {

/// @brief Represents a region of memory.
struct Region {
    Region(void *start, void *end);
    [[nodiscard]] uintptr_t getRange() const;

    void *start;
    void *end;
};

struct MEMiExpBlockHead;

/// @brief Link of a non-intrusive doubly-linked list.
struct MEMiExpBlockLink {
    MEMiExpBlockHead *m_prev;
    MEMiExpBlockHead *m_next;
};

/// @brief Non-intrusive doubly-linked list.
struct MEMiExpBlockList {
    MEMiExpBlockHead *insert(MEMiExpBlockHead *block, MEMiExpBlockHead *prev);
    MEMiExpBlockHead *append(MEMiExpBlockHead *block);
    MEMiExpBlockHead *remove(MEMiExpBlockHead *block);

    MEMiExpBlockHead *m_head;
    MEMiExpBlockHead *m_tail;
};

/// @brief Head of the memory block. Contains information about the block and a link in the
/// corresponding used/free linked lists.
struct MEMiExpBlockHead {
private:
    MEMiExpBlockHead(const Region &region, u16 signature);

public:
    [[nodiscard]] static MEMiExpBlockHead *createFree(const Region &region);
    [[nodiscard]] static MEMiExpBlockHead *createUsed(const Region &region);

    [[nodiscard]] Region getRegion() const;
    [[nodiscard]] void *getMemoryStart() const;
    [[nodiscard]] void *getMemoryEnd() const;

    u16 m_signature;
    union {
        u16 val;
        struct {
            u16 direction : 1;
            u16 alignment : 7;
            u16 groupId : 8;
        } fields;
    } m_attribute;
    u32 m_size;
#ifdef BUILD_DEBUG
    u32 m_tag;
#endif // BUILD_DEBUG
    MEMiExpBlockLink m_link;
};

/// @brief Low-level implementation of a memory heap for managing dynamic memory allocation.
/// Allocation may occur anywhere within the memory space the heap is created from.
/// @details Due to requesting chunks from a provided memory block, internal fragmentation is
/// non-existent, but external fragmentation is still possible. Allocating temporary blocks from the
/// tail, and scene-permanent blocks from the head, is recommended. The memory overhead per
/// allocation is `sizeof(MEMiExpBlockHead)`.
class MEMiExpHeapHead : public MEMiHeapHead {
private:
    MEMiExpHeapHead(void *end, u16 opt);
    ~MEMiExpHeapHead();

public:
    typedef std::function<void(void *, MEMiHeapHead *, uintptr_t)> Visitor;

    static MEMiExpHeapHead *create(void *startAddress, size_t size, u16 flag);
    void destroy();

    void *alloc(size_t size, s32 align);
    void free(void *block);
    [[nodiscard]] u32 getAllocatableSize(s32 align) const;
    void visitAllocated(Visitor visitor, uintptr_t param);

    [[nodiscard]] u16 getGroupID() const;
    void setGroupID(u16 groupID);

private:
    enum class eAttribute {
        BestFitAlloc = 0,
    };
    typedef EGG::TBitFlag<u16, eAttribute> Attribute;

    [[nodiscard]] void *allocFromHead(size_t size, s32 alignment);
    [[nodiscard]] void *allocFromTail(size_t size, s32 alignment);
    [[nodiscard]] void *allocUsedBlockFromFreeBlock(MEMiExpBlockHead *block, void *address,
            u32 size, s32 direction);
    bool recycleRegion(const Region &initialRegion);

    MEMiExpBlockList m_freeBlocks;
    MEMiExpBlockList m_usedBlocks;
    u16 m_groupId;
#ifdef BUILD_DEBUG
    u32 m_tag;
#endif // BUILD_DEBUG
    Attribute m_attribute;

    static constexpr u32 EXP_HEAP_SIGNATURE = 0x45585048; // EXPH
};

} // namespace Abstract::Memory
