#pragma once

#include "egg/core/Heap.hh"

#include <abstract/memory/ExpHeap.hh>

namespace EGG {

/// @brief High-level implementation of a memory heap for managing dynamic memory allocation.
/// Allocation may occur anywhere within the memory space the heap is created from.
/// @details Due to requesting chunks from a provided memory block, internal fragmentation is
/// non-existent, but external fragmentation is still possible. Allocating temporary blocks from the
/// tail, and scene-permanent blocks from the head, is recommended. The memory overhead per
/// allocation is `sizeof(Abstract::Memory::MEMiExpBlockHead)`.
class ExpHeap : public Heap {
public:
    /// @brief Memory blocks have group IDs assigned to them (default 0).
    /// This class is a record of the total sum of those blocks' sizes, categorized by group ID.
    /// Used for memory profiling.
    /// @details Interfaces with the expanded heap via `ExpHeap::calcGroupSize`.
    class GroupSizeRecord {
    public:
        GroupSizeRecord();
        void reset();
        [[nodiscard]] size_t getGroupSize(u16 groupID) const;
        void addSize(u16 groupID, size_t size);

        [[nodiscard]] constexpr size_t size() const {
            return m_entries.size();
        }

    private:
        std::array<size_t, 256> m_entries;
    };

    ~ExpHeap() override;
    void destroy() override;

    /// @addr{0x80226EFC}
    [[nodiscard]] Kind getHeapKind() const override {
        return Heap::Kind::Expanded;
    }

    [[nodiscard]] void *alloc(size_t size, s32 align) override;
    void free(void *block) override;
    [[nodiscard]] u32 getAllocatableSize(s32 align = 4) const override;

    static void addGroupSize(void *block, Abstract::Memory::MEMiHeapHead *heap, uintptr_t param);
    void calcGroupSize(GroupSizeRecord *record);

    void setGroupID(u16 groupID);
    [[nodiscard]] u16 getGroupID() const;

    [[nodiscard]] Abstract::Memory::MEMiExpHeapHead *dynamicCastHandleToExp();
    [[nodiscard]] const Abstract::Memory::MEMiExpHeapHead *dynamicCastHandleToExp() const;

    [[nodiscard]] static ExpHeap *create(void *startAddress, size_t size, u16 opt);
    [[nodiscard]] static ExpHeap *create(size_t size, Heap *heap, u16 opt);

private:
    ExpHeap(Abstract::Memory::MEMiHeapHead *handle);
};

} // namespace EGG
