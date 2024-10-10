#pragma once

#include "egg/core/Heap.hh"

#include "abstract/memory/ExpHeap.hh"

namespace EGG {

class ExpHeap : public Heap {
public:
    class GroupSizeRecord {
    public:
        GroupSizeRecord();
        void reset();
        [[nodiscard]] size_t getGroupSize(u16 groupID) const;
        void addSize(u16 groupID, size_t size);

    private:
        std::array<size_t, 256> m_entries;
    };

    ~ExpHeap() override;
    void destroy() override;
    [[nodiscard]] Kind getHeapKind() const override;
    [[nodiscard]] void *alloc(size_t size, s32 align) override;
    void free(void *block) override;
    [[nodiscard]] u32 getAllocatableSize(s32 align = 4) const override;

    static void addGroupSize(void *block, Abstract::Memory::MEMiHeapHead *heap, uintptr_t param);
    void calcGroupSize(GroupSizeRecord *record);

    void setGroupID(u16 groupID);

    [[nodiscard]] Abstract::Memory::MEMiExpHeapHead *dynamicCastHandleToExp();
    [[nodiscard]] const Abstract::Memory::MEMiExpHeapHead *dynamicCastHandleToExp() const;

    [[nodiscard]] static ExpHeap *create(void *startAddress, size_t size, u16 opt);
    [[nodiscard]] static ExpHeap *create(size_t size, Heap *heap, u16 opt);

    static void initRootHeap(void *startAddress, size_t size);

    [[nodiscard]] static ExpHeap *getRootHeap();

private:
    ExpHeap(Abstract::Memory::MEMiHeapHead *handle);

    static ExpHeap *s_rootHeap;
};

} // namespace EGG
