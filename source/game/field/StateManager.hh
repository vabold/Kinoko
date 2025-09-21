#pragma once

#include "game/field/obj/ObjectBase.hh"

#include <type_traits>

namespace Field {

template <typename T>
struct StateManagerEntry {
    using StateEnterFunc = void (T::*)();
    using StateCalcFunc = void (T::*)();

    u16 id;
    StateEnterFunc onEnter;
    StateCalcFunc onCalc;
};

template <typename T>
class StateManager {
public:
    StateManager(T *obj);

    virtual ~StateManager() {
        delete[] m_entryIds.data();
    }

protected:
    void calc() {
        if (m_nextStateId >= 0) {
            m_currentStateId = m_nextStateId;
            m_nextStateId = -1;
            m_currentFrame = 0;

            auto enterFunc = m_entries[m_entryIds[m_currentStateId]].onEnter;
            (m_obj->*enterFunc)();
        } else {
            ++m_currentFrame;
        }

        auto calcFunc = m_entries[m_entryIds[m_currentStateId]].onCalc;
        (m_obj->*calcFunc)();
    }

    u16 m_currentStateId;
    s32 m_nextStateId;
    u32 m_currentFrame;
    std::span<u16> m_entryIds;
    std::span<const StateManagerEntry<T>> m_entries;
    T *m_obj;
};

/// @brief Defined outside of the class declaration so that typename T will be a complete type.
template <typename T>
StateManager<T>::StateManager(T *obj)
    : m_currentStateId(0), m_nextStateId(-1), m_currentFrame(0), m_obj(obj) {
    // Concepts don't work here due to CRTP causing incomplete class type use.
    STATIC_ASSERT((std::is_base_of_v<ObjectBase, T>));
    STATIC_ASSERT((std::is_same_v<decltype(T::STATE_ENTRIES),
            const std::array<StateManagerEntry<T>, T::STATE_ENTRIES.size()>>));

    m_entryIds = std::span(new u16[T::STATE_ENTRIES.size()], T::STATE_ENTRIES.size());

    // The base game initializes all entries to 0xffff, possibly to avoid an uninitialized value
    for (auto &id : m_entryIds) {
        id = 0xffff;
    }

    for (size_t i = 0; i < m_entryIds.size(); ++i) {
        m_entryIds[T::STATE_ENTRIES[i].id] = i;
    }

    m_entries = std::span<const StateManagerEntry<T>>(T::STATE_ENTRIES);
}

} // namespace Field
