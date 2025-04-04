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
class StateManagerBase {
public:
    StateManagerBase() : m_currentStateId(0), m_nextStateId(-1), m_currentFrame(0), m_obj(nullptr) {
        // Concepts don't work here due to CRTP causing incomplete class type use.
        STATIC_ASSERT((std::is_base_of_v<ObjectBase, T>));
    }

    virtual ~StateManagerBase() {}

protected:
    u16 m_currentStateId;
    s32 m_nextStateId;
    u32 m_currentFrame;
    std::span<u16> m_entryIds;
    std::span<const StateManagerEntry<T>> m_entries;
    const T *m_obj;
};

// Keep the constructor from above
template <typename T>
class StateManager : public StateManagerBase<T> {
public:
    StateManager() = default;
    ~StateManager() = default;
};

} // namespace Field
