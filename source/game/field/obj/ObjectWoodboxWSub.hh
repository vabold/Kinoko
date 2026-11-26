#pragma once

#include "game/field/RailInterpolator.hh"
#include "game/field/obj/ObjectWoodbox.hh"

namespace Kinoko::Field {

/// @brief Represents a single wooden box spawned by an @ref ObjectWoodboxW
/// @details The @ref ObjectWoodboxW spawner is responsible for using @ref enableCollision() to make
/// the box tangible again and reset its position along the rail.
class ObjectWoodboxWSub final : public ObjectWoodbox {
public:
    /// @addr{0x8077E34C}
    /// @copydoc ObjectWoodbox::ObjectWoodbox(const System::MapdataGeoObj &)
    ObjectWoodboxWSub(const System::MapdataGeoObj &params) : ObjectWoodbox(params) {}

    /// @addr{0x8077E388}
    /// @brief Default virtual destructor
    ~ObjectWoodboxWSub() override = default;

    /// @addr{0x8077E3E4}
    /// @copybrief ObjectBase::init()
    /// @details Calls @ref ObjectBreakable::init() and sets @ref m_state to @ref State::Inactive.
    void init() override {
        ObjectBreakable::init();
        m_state = State::Inactive;
    }

    /// @addr{0x8077E49C}
    /// @copybrief ObjectBase::calc()
    /// @details Does nothing if the box is inactive. Otherwise, calls @ref calcPosition() to
    /// update the box's position along the rail.
    void calc() override {
        if (m_state == State::Inactive) {
            return;
        }

        calcPosition();
    }

    /// @addr{0x8077EDA4}
    /// @copybrief ObjectBase::loadFlags()
    /// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame
    [[nodiscard]] LoadFlags loadFlags() const override {
        return LoadFlags(eLoadFlags::Calc);
    }

    /// @addr{0x8077E444}
    /// @copybrief ObjectBreakable::enableCollision()
    /// @details Calls @ref ObjectBreakable::enableCollision() to set @ref m_state to @ref
    /// State::Active and resets the box's rail interpolator to the beginning of the rail.
    void enableCollision() override {
        ObjectBreakable::enableCollision();
        m_railInterpolator->init(0.0f, 0);
        m_railInterpolator->setPerPointVelocities(true);
    }

private:
    /// @addr{0x8077E56C}
    /// @brief Updates the rail interpolator and the box's position along the rail
    /// @details If the box has reached the end of the rail, then it becomes intangible by setting
    /// @ref m_state to @ref State::Inactive.
    void calcPosition() {
        auto status = m_railInterpolator->calc();

        if (status == RailInterpolator::Status::RailEnd) {
            m_state = State::Inactive;
        }

        setPos(m_railInterpolator->curPos());
    }
};

} // namespace Kinoko::Field
