#pragma once

#include <source/abstract/List.hh>

namespace Kart {

struct KartAccessor {};

class KartObjectProxy {
    friend class KartObject;

public:
    KartObjectProxy();
    ~KartObjectProxy();

    Abstract::List *list() const;

protected:
    KartAccessor *m_accessor;

private:
    static void ApplyAll(KartAccessor &pointers);

    // Used to initialize multiple KartObjectProxy instances at once
    // Lists are created on the stack in KartObject::Create
    static Abstract::List *s_list;
};

} // namespace Kart
