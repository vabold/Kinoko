#include "ExpHeap.hh"

#include <limits>
#include <new> // placement new

namespace Abstract::Memory {

Region::Region(void *start, void *end) : start(start), end(end) {}

uintptr_t Region::getRange() const {
    return GetAddrNum(end) - GetAddrNum(start);
}

MEMiExpBlockHead *MEMiExpBlockList::insert(MEMiExpBlockHead *block, MEMiExpBlockHead *prev) {
    MEMiExpBlockHead *next;

    block->m_link.m_prev = prev;
    if (prev) {
        next = prev->m_link.m_next;
        prev->m_link.m_next = block;
    } else {
        next = m_head;
        m_head = block;
    }

    block->m_link.m_next = next;
    if (next) {
        next->m_link.m_prev = block;
    } else {
        m_tail = block;
    }

    return block;
}

MEMiExpBlockHead *MEMiExpBlockList::append(MEMiExpBlockHead *block) {
    return insert(block, m_tail);
}

MEMiExpBlockHead *MEMiExpBlockList::remove(MEMiExpBlockHead *block) {
    MEMiExpBlockHead *prev = block->m_link.m_prev;
    MEMiExpBlockHead *next = block->m_link.m_next;

    if (prev) {
        prev->m_link.m_next = next;
    } else {
        m_head = next;
    }

    if (next) {
        next->m_link.m_prev = prev;
    } else {
        m_tail = prev;
    }

    return prev;
}

MEMiExpBlockHead::MEMiExpBlockHead(const Region &region, u16 signature) {
    m_signature = signature;
    m_attribute.val = 0;

    m_size = region.getRange() - sizeof(MEMiExpBlockHead);

    m_link.m_prev = nullptr;
    m_link.m_next = nullptr;
}

MEMiExpBlockHead *MEMiExpBlockHead::createFree(const Region &region) {
    constexpr u16 FREE_BLOCK_SIGNATURE = 0x4652; // FR
    return new (region.start) MEMiExpBlockHead(region, FREE_BLOCK_SIGNATURE);
}

MEMiExpBlockHead *MEMiExpBlockHead::createUsed(const Region &region) {
    constexpr u16 USED_BLOCK_SIGNATURE = 0x5544; // UD
    return new (region.start) MEMiExpBlockHead(region, USED_BLOCK_SIGNATURE);
}

Region MEMiExpBlockHead::getRegion() const {
    return Region(SubOffset(this, m_attribute.fields.alignment), getMemoryEnd());
}

void *MEMiExpBlockHead::getMemoryStart() const {
    return AddOffset(this, sizeof(MEMiExpBlockHead));
}

void *MEMiExpBlockHead::getMemoryEnd() const {
    return AddOffset(getMemoryStart(), m_size);
}

MEMiExpHeapHead::MEMiExpHeapHead(void *end, u16 opt)
    : MEMiHeapHead(EXP_HEAP_SIGNATURE, AddOffset(this, sizeof(MEMiExpHeapHead)), end, opt) {
    m_groupId = 0;
#ifdef BUILD_DEBUG
    m_tag = 0;
#endif // BUILD_DEBUG
    m_attribute.makeAllZero();

    Region region = Region(getHeapStart(), getHeapEnd());
    MEMiExpBlockHead *block = MEMiExpBlockHead::createFree(region);

    m_freeBlocks.m_head = block;
    m_freeBlocks.m_tail = block;
    m_usedBlocks.m_head = nullptr;
    m_usedBlocks.m_tail = nullptr;
}

/// @addr{0x80198D58}
MEMiExpHeapHead::~MEMiExpHeapHead() = default;

/// @addr{0x80198CA8}
MEMiExpHeapHead *MEMiExpHeapHead::create(void *startAddress, size_t size, u16 flag) {
    void *endAddress = AddOffset(startAddress, size);

    startAddress = RoundUp(startAddress, 4);
    endAddress = RoundDown(endAddress, 4);

    uintptr_t startAddrNum = GetAddrNum(startAddress);
    uintptr_t endAddrNum = GetAddrNum(endAddress);

    if (startAddrNum > endAddrNum) {
        return nullptr;
    }

    if (endAddrNum - startAddrNum < sizeof(MEMiExpHeapHead) + sizeof(MEMiExpBlockHead) + 4) {
        return nullptr;
    }

    return new (startAddress) MEMiExpHeapHead(endAddress, flag);
}

void MEMiExpHeapHead::destroy() {
    this->~MEMiExpHeapHead();
}

/// @addr{0x80198D88}
void *MEMiExpHeapHead::alloc(size_t size, s32 align) {
    if (size == 0) {
        size = 1;
    }
    size = RoundUp(size, 4);

    void *block = nullptr;
    if (align >= 0) {
        block = allocFromHead(size, align);
    } else {
        block = allocFromTail(size, -align);
    }

    return block;
}

/// @addr{0x80199038}
void MEMiExpHeapHead::free(void *block) {
    if (!block) {
        return;
    }

    MEMiExpBlockHead *head =
            reinterpret_cast<MEMiExpBlockHead *>(SubOffset(block, sizeof(MEMiExpBlockHead)));

    Region region = head->getRegion();
    m_usedBlocks.remove(head);
    recycleRegion(region);
}

/// @addr{0x80199180}
u32 MEMiExpHeapHead::getAllocatableSize(s32 align) const {
    // Doesn't matter which direction it can be allocated from, take absolute value
    align = std::abs(align);

    u32 maxSize = 0;
    u32 x = std::numeric_limits<u32>::max();

    for (MEMiExpBlockHead *block = m_freeBlocks.m_head; block; block = block->m_link.m_next) {
        void *memptr = block->getMemoryStart();
        void *start = RoundUp(memptr, align);
        void *end = block->getMemoryEnd();

        if (GetAddrNum(start) < GetAddrNum(end)) {
            u32 size = GetAddrNum(end) - GetAddrNum(start);
            u32 offset = GetAddrNum(start) - GetAddrNum(memptr);

            if (maxSize < size || (maxSize == size && x > offset)) {
                maxSize = size;
                x = offset;
            }
        }
    }

    return maxSize;
}

/// @addr{0x801992A8}
void MEMiExpHeapHead::visitAllocated(Visitor visitor, uintptr_t param) {
    for (MEMiExpBlockHead *block = m_usedBlocks.m_head; block;) {
        MEMiExpBlockHead *next = block->m_link.m_next;
        visitor(block->getMemoryStart(), this, param);
        block = next;
    }
}

/// @addr{0x8019934C}
u16 MEMiExpHeapHead::getGroupID() const {
    return m_groupId;
}

/// @addr{0x80199258}
void MEMiExpHeapHead::setGroupID(u16 groupID) {
    m_groupId = groupID;
}

/// @addr{0x8019899C}
void *MEMiExpHeapHead::allocFromHead(size_t size, s32 alignment) {
    MEMiExpBlockHead *found = nullptr;
    u32 blockSize = -1;
    void *bestAddress = nullptr;

    for (MEMiExpBlockHead *block = m_freeBlocks.m_head; block; block = block->m_link.m_next) {
        void *const memptr = block->getMemoryStart();
        void *const address = RoundUp(memptr, alignment);
        size_t offset = GetAddrNum(address) - GetAddrNum(memptr);

        if (block->m_size < size + offset) {
            continue;
        }

        if (blockSize <= block->m_size) {
            continue;
        }

        found = block;
        blockSize = block->m_size;
        bestAddress = address;

        // This is a valid block to allocate in, but is it the best one?
        if (m_attribute.offBit(eAttribute::BestFitAlloc) || blockSize == size) {
            break;
        }
    }

    if (!found) {
        return nullptr;
    }

    return allocUsedBlockFromFreeBlock(found, bestAddress, size, 0);
}

/// @addr{0x80198A78}
void *MEMiExpHeapHead::allocFromTail(size_t size, s32 alignment) {
    MEMiExpBlockHead *found = nullptr;
    u32 blockSize = -1;
    void *bestAddress = nullptr;

    for (MEMiExpBlockHead *block = m_freeBlocks.m_tail; block; block = block->m_link.m_prev) {
        void *const start = block->getMemoryStart();
        void *const endAddr = AddOffset(start, block->m_size);
        void *const end = RoundDown(SubOffset(endAddr, size), alignment);

        if (static_cast<intptr_t>(GetAddrNum(end) - GetAddrNum(start)) < 0) {
            continue;
        }

        if (blockSize <= block->m_size) {
            continue;
        }

        found = block;
        blockSize = block->m_size;
        bestAddress = end;

        // This is a valid block to allocate in, but is it the best one?
        if (m_attribute.offBit(eAttribute::BestFitAlloc) || blockSize == size) {
            break;
        }
    }

    if (!found) {
        return nullptr;
    }

    return allocUsedBlockFromFreeBlock(found, bestAddress, size, 1);
}

/// @addr{0x80198798}
void *MEMiExpHeapHead::allocUsedBlockFromFreeBlock(MEMiExpBlockHead *block, void *address, u32 size,
        s32 direction) {
    // The left region represents the free block created to the left of the new memory block
    // The right region represents the free block created to the right of the new memory block
    // address -> address + size exists entirely between leftRegion.end and rightRegion.start
    Region leftRegion = block->getRegion();
    Region rightRegion = Region(AddOffset(address, size), leftRegion.end);
    leftRegion.end = SubOffset(address, sizeof(MEMiExpBlockHead));

    MEMiExpBlockHead *prev = m_freeBlocks.remove(block);

    if (leftRegion.getRange() < sizeof(MEMiExpBlockHead) + 4) {
        // Not enough room to insert a free memory block
        leftRegion.end = leftRegion.start;
    } else {
        prev = m_freeBlocks.insert(MEMiExpBlockHead::createFree(leftRegion), prev);
    }

    if (rightRegion.getRange() < sizeof(MEMiExpBlockHead) + 4) {
        // Not enough room to insert a free memory block
        rightRegion.end = rightRegion.start;
    } else {
        m_freeBlocks.insert(MEMiExpBlockHead::createFree(rightRegion), prev);
    }

    fillAllocMemory(leftRegion.end, GetAddrNum(rightRegion.start) - GetAddrNum(leftRegion.end));

    // Now that the free blocks are cleared away, create a new used block
    Region region = Region(SubOffset(address, sizeof(MEMiExpBlockHead)), rightRegion.start);
    MEMiExpBlockHead *head = MEMiExpBlockHead::createUsed(region);

    head->m_attribute.fields.direction = direction;
    head->m_attribute.fields.alignment = GetAddrNum(head) - GetAddrNum(leftRegion.end);
    head->m_attribute.fields.groupId = m_groupId;
#ifdef BUILD_DEBUG
    head->m_tag = ++m_tag;
#endif // BUILD_DEBUG

    m_usedBlocks.append(head);

    return address;
}

// @addr{0x80198B40}
bool MEMiExpHeapHead::recycleRegion(const Region &initialRegion) {
    MEMiExpBlockHead *block = nullptr;
    Region region = initialRegion;

    for (MEMiExpBlockHead *search = m_freeBlocks.m_head; search; search = search->m_link.m_next) {
        if (search < initialRegion.start) {
            block = search;
            continue;
        }

        if (search == initialRegion.end) {
            region.end = search->getMemoryEnd();
            m_freeBlocks.remove(search);
            fillNoUseMemory(search, sizeof(MEMiExpBlockHead));
        }

        break;
    }

    if (block && block->getMemoryEnd() == initialRegion.start) {
        region.start = block;
        block = m_freeBlocks.remove(block);
    }

    if (region.getRange() < sizeof(MEMiExpBlockHead)) {
        return false;
    }

    fillFreeMemory(initialRegion.start, initialRegion.getRange());
    m_freeBlocks.insert(MEMiExpBlockHead::createFree(region), block);
    return true;
}

} // namespace Abstract::Memory
