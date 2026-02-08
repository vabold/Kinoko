#include "game/field/obj/ObjectDossun.hh"

namespace Field {

class ObjectDossunTsuibiHolder;

/// @brief Represents one of the two thwomps that oscillate and stomp in the long rBC hallway.
class ObjectDossunTsuibi final : public ObjectDossun {
    friend ObjectDossunTsuibiHolder;

public:
    /// @addr{0x8076393C}
    ObjectDossunTsuibi(const System::MapdataGeoObj &params, ObjectDossunTsuibiHolder *holder)
        : ObjectDossun(params), m_holder(holder) {}

    /// @addr{0x80764C48}
    ~ObjectDossunTsuibi() override = default;

    /// @addr{0x80763A04}
    void calc() override {
        m_touchingGround = false;
    }

    /// @addr{0x80763A10}
    void startStill() override {
        ObjectDossun::startStill();
        m_holder->startStill();
    }

private:
    ObjectDossunTsuibiHolder *m_holder;
};

} // namespace Field
