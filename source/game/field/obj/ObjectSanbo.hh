#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectSanbo final : public ObjectCollidable {
public:
    ObjectSanbo(const System::MapdataGeoObj &params);
    ~ObjectSanbo() override;

    void init() override;
    void calc() override;

    /// @addr{0x8077BD38}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

private:
    void calcMove();
    void checkSphere();

    bool m_standstill;   ///< Whether or not the pokey is currently walking
    u32 m_stillDuration; ///< Frames before the pokey will start to walk
    f32 m_yVel;          ///< Falling speed
    EGG::Vector3f m_up;
    EGG::Vector3f m_tangent;
};

} // namespace Field
