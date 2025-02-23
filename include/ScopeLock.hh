#pragma once

#include <Common.hh>

#include <egg/core/ExpHeap.hh>

template <typename T>
class ScopeLock;

template <>
class ScopeLock<GroupID> {
public:
    /// @brief Temporarily changes the group ID of a given heap to better track memory allocation.
    /// @details Typical pattern is "ScopeLock<GroupID> lock(groupID);" to initialize a lock.
    ScopeLock(GroupID newID) {
        EGG::ExpHeap *heap = EGG::Heap::dynamicCastToExp(EGG::Heap::getCurrentHeap());
        ASSERT(heap);
        GroupID prevID = static_cast<GroupID>(heap->getGroupID());

        if (prevID != GroupID::None) {
            WARN("Overwriting non-default group ID! Replacing %d with %d", prevID, newID);
        }

        heap->setGroupID(static_cast<u16>(newID));
    }

    ~ScopeLock() {
        EGG::ExpHeap *heap = EGG::Heap::dynamicCastToExp(EGG::Heap::getCurrentHeap());
        ASSERT(heap);
        heap->setGroupID(static_cast<u16>(GroupID::None));
    }
};
