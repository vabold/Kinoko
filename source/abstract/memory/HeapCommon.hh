#pragma once

#include "abstract/memory/List.hh"

#include <egg/core/BitFlag.hh>

#include <array>

class SavestateManager;

namespace Abstract::Memory {

/// @brief A low-level representation of a memory heap for managing dynamic memory allocation.
/// Interface for allocating and freeing memory blocks.
class MEMiHeapHead {
    friend class ::SavestateManager;

public:
    enum class FillType {
        NoUse = 0,
        Alloc = 1,
        Free = 2,
    };

    enum class eOptFlag {
        ZeroFillAlloc = 0,
        DebugFillAlloc = 1,
    };
    typedef EGG::TBitFlag<u16, eOptFlag> OptFlag;

    [[nodiscard]] MEMList &getChildList();
    [[nodiscard]] void *getHeapStart();
    [[nodiscard]] void *getHeapEnd();

    [[nodiscard]] static MEMList &getRootList();
    [[nodiscard]] static u32 getFillVal(FillType type);
    [[nodiscard]] static MEMiHeapHead *findContainHeap(const void *block);

    [[nodiscard]] static constexpr u16 getLinkOffset() {
        return offsetof(MEMiHeapHead, m_link);
    }

protected:
    MEMiHeapHead(u32 signature, void *heapStart, void *heapEnd, const OptFlag &opt);
    ~MEMiHeapHead();

    void fillNoUseMemory(void *address, u32 size);
    void fillAllocMemory(void *address, u32 size);
    void fillFreeMemory(void *address, u32 size);

private:
    [[nodiscard]] static MEMiHeapHead *findContainHeap(MEMList *list, const void *block);
    [[nodiscard]] MEMList &findListContainHeap() const;

    u32 m_signature;
    OptFlag m_optFlag;
    MEMLink m_link;
    MEMList m_childList;
    void *m_heapStart;
    void *m_heapEnd;

    static MEMList s_rootList;
    static constexpr std::array<u32, 3> s_fillVals = {{
            0xC3C3C3C3,
            0xF3F3F3F3,
            0xD3D3D3D3,
    }};
};

} // namespace Abstract::Memory
