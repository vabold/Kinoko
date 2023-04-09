#pragma once

#include <Common.hh>

namespace Abstract {

class Node {
    friend class List;

public:
    Node();
    ~Node();

    template <typename T>
    T *data() {
        return reinterpret_cast<T *>(m_data);
    }

private:
    void *m_data;
    Node *m_prev;
    Node *m_next;
};

class List {
public:
    List();
    ~List();

    Node *head() const;

    Node *getNext(Node *node) const;

    void append(void *data);

private:
    Node *m_head;
    Node *m_tail;
};

} // namespace Abstract
