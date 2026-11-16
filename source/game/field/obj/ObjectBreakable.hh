#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Represents objects that can be broken by kart collision, such as the DS Delfino Square /
/// Toad's Factory wooden boxes
/// @details This class effectively does nothing currently (except maintaining an "active" state)
/// and is implemented only to maintain the inheritance hierarchy used by @ref ObjectWoodbox.
class ObjectBreakable : public ObjectCollidable {
public:
    /// @brief Represents the current state of the breakable object
    enum class State {
        Inactive = 0, ///< The object is intangible
        Active = 1,   ///< The object is tangible and can be collided with
        Broken = 2,   ///< The object has been broken and is no longer tangible (unused in Kinoko)
    };

    /// @addr{0x8076EBE0}
    /// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
    /// @param params The parameters used to initialize the object
    /// @details Initializes @ref m_state to @ref State::Inactive.
    ObjectBreakable(const System::MapdataGeoObj &params)
        : ObjectCollidable(params), m_state(State::Inactive) {}

    /// @addr{0x8076EC28}
    /// @brief Default virtual destructor
    ~ObjectBreakable() override = default;

    /// @addr{0x807677E4}
    /// @copybrief ObjectBase::loadFlags()
    /// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
    [[nodiscard]] LoadFlags loadFlags() const override {
        return LoadFlags(eLoadFlags::Calc);
    }

    /// @addr{0x8076ED70}
    /// @brief Enables a spawner to make the object tangible again
    /// @details Sets @ref m_state to @ref State::Active.
    virtual void enableCollision() {
        m_state = State::Active;
    }

protected:
    State m_state; ///< The current state of the breakable object
};

} // namespace Kinoko::Field
