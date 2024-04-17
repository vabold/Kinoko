#include "List.hh"

namespace Abstract {

Node::Node() : m_data(nullptr), m_prev(nullptr), m_next(nullptr) {}

Node::~Node() = default;

List::List() : m_head(nullptr), m_tail(nullptr) {}

List::~List() {
    for (Node *node = m_head; node;) {
        Node *next = getNext(node);
        // NOTE (vabold): Objects are handled independently
        // Do not expect this to delete Node::m_data!
        delete node;
        node = next;
    }
}

Node *List::head() const {
    return m_head;
}

Node *List::getNext(Node *node) const {
    return node ? node->m_next : m_head;
}

void List::append(void *data) {
    Node *node = new Node();
    node->m_data = data;
    node->m_prev = m_tail;
    node->m_next = nullptr;

    if (m_tail) {
        m_tail->m_next = node;
    } else {
        m_head = node;
    }

    m_tail = node;
}

void List::remove(Node *node) {
    if (m_head == node) {
        m_head = node->m_next;
    }

    if (node->m_prev) {
        node->m_prev->m_next = node->m_next;
    }

    if (node->m_next) {
        node->m_next->m_prev = node->m_prev;
    } else {
        m_tail = node->m_prev;
    }

    delete node;
}

} // namespace Abstract
