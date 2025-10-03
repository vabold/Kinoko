#pragma once

#include "game/field/obj/ObjectBase.hh"

#include <type_traits>

namespace Field {

struct StateManagerEntry {
    u16 id;
    void (*onEnter)(void *);
    void (*onCalc)(void *);
};

template <typename T, void (T::*Enter)(), void (T::*Calc)()>
constexpr StateManagerEntry StateEntry(u16 id) {
    auto enter = [](void *obj) { (reinterpret_cast<T *>(obj)->*Enter)(); };
    auto calc = [](void *obj) { (reinterpret_cast<T *>(obj)->*Calc)(); };
    return {id, enter, calc};
}

/// @brief Base class that represents different "states" for an object.
/// @details Some objects inherit from this class in order to define unique behavior depending on
/// what the current "state" of the object is. For example, FireSnakes may be falling from the sun,
/// resting, or jumping. Each state has an "enter" and a "calc" function. The object that inherits
/// this class is responsible for defining how many states there are and must specify the enter and
/// calc functions for each of those states. The object is also responsible for defining the
/// transition points (i.e. the criteria to move from one state to another).
class StateManager {
protected:
    StateManager(void *obj, const std::span<const StateManagerEntry> &entries)
        : m_currentStateId(0), m_nextStateId(-1), m_currentFrame(0), m_entries(entries),
          m_obj(obj) {
        m_entryIds = std::span(new u16[m_entries.size()], m_entries.size());

        // The base game initializes all entries to 0xffff, possibly to avoid an uninitialized value
        for (auto &id : m_entryIds) {
            id = 0xffff;
        }

        for (size_t i = 0; i < m_entryIds.size(); ++i) {
            m_entryIds[m_entries[i].id] = i;
        }
    }

    virtual ~StateManager() {
        delete[] m_entryIds.data();
    }

    void calc() {
        if (m_nextStateId >= 0) {
            m_currentStateId = m_nextStateId;
            m_nextStateId = -1;
            m_currentFrame = 0;

            auto enterFunc = m_entries[m_entryIds[m_currentStateId]].onEnter;
            enterFunc(m_obj);
        } else {
            ++m_currentFrame;
        }

        auto calcFunc = m_entries[m_entryIds[m_currentStateId]].onCalc;
        calcFunc(m_obj);
    }

    u16 m_currentStateId;
    s32 m_nextStateId;
    u32 m_currentFrame;
    std::span<u16> m_entryIds;
    std::span<const StateManagerEntry> m_entries;
    void *m_obj;
};

} // namespace Field
