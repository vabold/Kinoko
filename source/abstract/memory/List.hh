#pragma once

#include "abstract/memory/Memory.hh"

namespace Abstract::Memory {

struct MEMLink {
    void *m_prevObject;
    void *m_nextObject;
};

struct MEMList {
    MEMList(u16 offset);

    void append(void *object);
    void remove(void *object);
    [[nodiscard]] void *getFirst();
    [[nodiscard]] void *getNext(void *object);

    void *m_headObject;
    void *m_tailObject;
    u16 m_numObjects;
    u16 m_offset;

private:
    void setFirstObject(void *object);
    [[nodiscard]] MEMLink *getLink(void *object);
};

} // namespace Abstract::Memory
