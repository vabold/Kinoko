#pragma once

#include <Types.hh>

#include <algorithm>

namespace Kinoko::Field {

/// @brief Represents a single entry in the StateManager's state table
/// @details Each entry contains an ID, a function pointer invoked when the state is entered, and a
/// function pointer invoked every frame while the state is active
struct StateManagerEntry {
    u16 id;                  ///< The ID of the state, always maps to the index in the entry table
    void (*onEnter)(void *); ///< Function pointer invoked when the state is entered
    void (*onCalc)(void *);  ///< Function pointer invoked every frame while the state is active
};

/// @brief Templated helper function to create a StateManagerEntry for an object of type T
/// @tparam T The type of the object that will use this state entry
/// @tparam Enter Pointer to the member function of T, called when the state is entered, or
/// `nullptr` if no work is required on entry
/// @tparam Calc Pointer to the member function of T, called every frame while the state is active,
/// or `nullptr` if no work is required
/// @param id The ID of the state, always maps to the index in the entry table
/// @return A StateManagerEntry struct initialized with the provided ID and function pointers
/// @note `Enter` and `Calc` are checked with `if` rather than `if constexpr`. Each object's state
/// table is built inside its own class body, where `T` is still incomplete, and GCC cannot
/// constant-evaluate a member function pointer of an incomplete class against `nullptr`. The check
/// is still optimized away since both values are known at compile time.
/// @relates StateManagerEntry
template <typename T, void (T::*Enter)(), void (T::*Calc)()>
[[nodiscard]] constexpr StateManagerEntry StateEntry(u16 id) {
    auto enter = [](void *obj) {
        if (Enter) {
            (reinterpret_cast<T *>(obj)->*Enter)();
        }
    };
    auto calc = [](void *obj) {
        if (Calc) {
            (reinterpret_cast<T *>(obj)->*Calc)();
        }
    };
    return {id, enter, calc};
}

/// @brief Base class that manages different "states" for an object
/// @details Objects can inherit from this class in order to define unique behavior depending on
/// what the current "state" of the object is. For example, @ref ObjectFireSnake may be falling from
/// the sun, resting, or jumping. Each state has an @ref StateManagerEntry::onEnter "onEnter" and a
/// @ref StateManagerEntry::onCalc "onCalc" function. The object that inherits this class is
/// responsible for defining how many states there are and must specify the enter and calc functions
/// for each of those states, using the @ref StateEntry() helper function. The object is also
/// responsible for defining the transition points (i.e. the criteria to move from one state to
/// another) and updating the next state ID accordingly.
/// @note This class was originally implemented using the [curiously recurring template pattern
/// (CRTP)](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern). However, we ran
/// into inheritance issues where a subclass of an object needed to define its own member function
/// pointers for @ref StateManagerEntry::onEnter "onEnter" and a @ref StateManagerEntry::onCalc
/// "onCalc", but the template type `T` in the StateManager no longer matched the subclass type
/// (since the parent class inherits from @ref StateManager). For example, @ref ObjectFireSnakeV
/// inherits from @ref ObjectFireSnake, and the template type `T` would no longer match @ref
/// ObjectFireSnakeV. Rather than try to leverage virtual inheritance with wrapper functions to call
/// the parent member functions, we instead refactored to use @ref StateManagerEntry structs each
/// having lambdas which invoke the @ref StateManagerEntry::onEnter "onEnter" and @ref
/// StateManagerEntry::onCalc "onCalc" function.
class StateManager {
protected:
    /// @brief Constructor that initializes the @ref StateManager with the object that owns it and
    /// the state entries
    /// @param obj The object that owns this @ref StateManager instance
    /// @param entries A span of @ref StateManagerEntry structs that define the set of enter and
    /// calc functions for each state Id
    StateManager(void *obj, const std::span<const StateManagerEntry> &entries)
        : m_currentStateId(0),
          m_nextStateId(-1),
          m_currentFrame(0),
          m_entries(entries),
          m_entryIds(initEntryIds()),
          m_obj(obj) {}

    /// @brief Default virtual destructor
    virtual ~StateManager() = default;

    /// @brief Evaluates the current state and invokes the appropriate enter and calc functions
    /// @details If the inheriting object has set @ref m_nextStateId to a valid value, transitions
    /// to that state by invoking the @ref StateManagerEntry::onEnter "onEnter" function and
    /// resetting the current frame counter and next state ID. Otherwise, increments the current
    /// frame counter. In both scenarios, invokes the @ref StateManagerEntry::onCalc "onCalc"
    /// function for the current state.
    void calc() {
        if (m_nextStateId >= 0) {
            m_currentStateId = m_nextStateId;
            m_nextStateId = -1;
            m_currentFrame = 0;
            m_entries[m_entryIds[m_currentStateId]].onEnter(m_obj);
        } else {
            ++m_currentFrame;
        }

        m_entries[m_entryIds[m_currentStateId]].onCalc(m_obj);
    }

    u16 m_currentStateId; ///< ID of the current state
    s32 m_nextStateId;    ///< ID of the state to transition to, or -1 if no transition is requested
    u32 m_currentFrame;   ///< Number of frames elapsed since the current state was entered

private:
    /// @brief Builds the table that maps each state ID to its index in @ref m_entries
    /// @return The state ID to entry index table to initialize @ref m_entryIds with
    /// @details Called from the constructor's initializer list so that @ref m_entryIds can be
    /// `const`. Relies on @ref m_entries being declared, and thus initialized, before
    /// @ref m_entryIds.
    [[nodiscard]] owning_span<u16> initEntryIds() const {
        owning_span<u16> entryIds(m_entries.size());

        // The base game initializes all entries to 0xffff, possibly to avoid an uninitialized value
        std::fill(entryIds.begin(), entryIds.end(), 0xffff);

        for (size_t i = 0; i < entryIds.size(); ++i) {
            entryIds[m_entries[i].id] = i;
        }

        return entryIds;
    }

    /// @brief Span of state entries defining the enter and calc functions for each state
    const std::span<const StateManagerEntry> m_entries;

    const owning_span<u16> m_entryIds; ///< Maps state IDs to indices in the m_entries span
    void *const m_obj; ///< Pointer to the object that owns this @ref StateManager instance
};

} // namespace Kinoko::Field
