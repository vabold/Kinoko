#include "List.hh"

namespace Abstract::Memory {

MEMList::MEMList() : MEMList(0) {}

/// @addr{0x80199BF0}
MEMList::MEMList(u16 offset) {
    m_headObject = nullptr;
    m_tailObject = nullptr;
    m_numObjects = 0;
    m_offset = offset;
}

/// @addr{0x80199C08}
void MEMList::append(void *object) {
    ASSERT(object);

    if (!m_headObject) {
        setFirstObject(object);
    } else {
        MEMLink *link = getLink(object);

        link->m_prevObject = m_tailObject;
        link->m_nextObject = nullptr;

        getLink(m_tailObject)->m_nextObject = object;
        m_tailObject = object;
        ++m_numObjects;
    }
}

/// @addr{0x80199C78}
void MEMList::remove(void *object) {
    ASSERT(object);

    MEMLink *link = getLink(object);

    if (!link->m_prevObject) {
        m_headObject = link->m_nextObject;
    } else {
        getLink(link->m_prevObject)->m_nextObject = link->m_nextObject;
    }

    if (!link->m_nextObject) {
        m_tailObject = link->m_prevObject;
    } else {
        getLink(link->m_nextObject)->m_prevObject = link->m_prevObject;
    }

    link->m_prevObject = nullptr;
    link->m_nextObject = nullptr;
    --m_numObjects;
}

void *MEMList::getFirst() {
    return getNext(nullptr);
}

/// @addr{0x80199CE4}
void *MEMList::getNext(void *object) {
    return object ? getLink(object)->m_nextObject : m_headObject;
}

void MEMList::setFirstObject(void *object) {
    ASSERT(object);

    MEMLink *link = getLink(object);

    link->m_nextObject = nullptr;
    link->m_prevObject = nullptr;

    m_headObject = object;
    m_tailObject = object;
    ++m_numObjects;
}

MEMLink *MEMList::getLink(void *object) {
    return reinterpret_cast<MEMLink *>(GetAddrNum(object) + m_offset);
}

} // namespace Abstract::Memory
