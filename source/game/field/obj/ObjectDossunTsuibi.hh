#include "game/field/obj/ObjectDossun.hh"

namespace Kinoko::Field {

class ObjectDossunTsuibiHolder;

/// @brief Represents one of the two thwomps that oscillate and stomp in the long rBC hallway.
/// @details Interfaces with @ref ObjectDossunTsuibiHolder so both Thwomps stop at the same time.
class ObjectDossunTsuibi final : public ObjectDossun {
    /// @brief Grants access to the manager class @ref ObjectDossunTsuibiHolder so that it can
    /// control the Thwomp's behavior.
    friend ObjectDossunTsuibiHolder;

public:
    /// @addr{0x8076393C}
    /// @brief Constructor
    /// @param params The parameters used to initialize the object
    /// @param holder Pointer to the manager class that controls this Thwomp
    ObjectDossunTsuibi(const System::MapdataGeoObj &params, ObjectDossunTsuibiHolder *holder)
        : ObjectDossun(params), m_holder(holder) {}

    /// @addr{0x80764C48}
    /// @brief Default virtual destructor
    ~ObjectDossunTsuibi() override = default;

    /// @addr{0x80763A04}
    /// @copybrief ObjectBase::calc()
    /// @details Simply resets the @ref m_touchingGround flag every frame. The Thwomp's per-frame
    /// logic is instead invoked by @ref ObjectDossunTsuibiHolder::calc() calling into the
    /// appropriate state calc function.
    void calc() override {
        m_touchingGround = false;
    }

private:
    /// @addr{0x80763A10}
    /// @copybrief ObjectDossun::startStill()
    /// @details Calls @ref ObjectDossun::startStill() to initialize the Thwomp at the beginning of
    /// the still state. Also notifies the manager class @ref ObjectDossunTsuibiHolder that this
    /// Thwomp has entered the still state.
    void startStill() override {
        ObjectDossun::startStill();
        m_holder->startStill();
    }

    ObjectDossunTsuibiHolder *const m_holder; ///< Pointer to the manager class
};

} // namespace Kinoko::Field
