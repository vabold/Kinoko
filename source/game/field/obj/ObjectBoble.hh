#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectBoble : public ObjectCollidable {
public:
    ObjectBoble(const System::MapdataGeoObj &params);
    ~ObjectBoble() override;

    void init() override;
    void calc() override;

    /// @addr{0x8075E744}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x8075E1F4}
    Kart::Reaction onCollision(Kart::KartObject * /*kartObj*/, Kart::Reaction reactionOnKart,
            Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) override {
        return reactionOnKart;
    }

private:
    void calcTangent();

    EGG::Vector3f m_curTangentDir;
};

} // namespace Field
