#pragma once

#include "game/field/obj/ObjectWoodbox.hh"

namespace Field {

class ObjectWoodboxWSub final : public ObjectWoodbox {
public:
    ObjectWoodboxWSub(const System::MapdataGeoObj &params);
    ~ObjectWoodboxWSub() override;

    /// @addr{0x8077E3E4}
    void init() override {
        ObjectBreakable::init();
        m_state = 0;
    }

    void calc() override;

    /// @addr{0x8077EDA4}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x8077E444}
    void enableCollision() override {
        ObjectBreakable::enableCollision();
        m_railInterpolator->init(0.0f, 0);
        m_railInterpolator->setPerPointVelocities(true);
    }

private:
    void calcPosition();
};

} // namespace Field
