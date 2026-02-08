#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectPressSenko;

/// @brief Responsible for stomper state management.
/// @details Actually inherits from ObjectItembox, but we don't need to implement.
class ObjectItemboxPress final : public ObjectCollidable {
public:
    ObjectItemboxPress(const System::MapdataGeoObj &params);
    ~ObjectItemboxPress() override;

    /// @addr{0x8076DA88}
    void init() override {
        m_state = 0;
    }

    void calc() override;

    /// @addr{0x8076E9E4}
    [[nodiscard]] ObjectId id() const override {
        return ObjectId::Itembox;
    }

    /// @addr{0x8076E9C4}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x8076E9CC}
    [[nodiscard]] const char *getResources() const override {
        return "itembox";
    }

    /// @addr{0x8076E9D8}
    [[nodiscard]] const char *getKclName() const override {
        return "itembox";
    }

    void startPress();

    void setState(u32 state) {
        m_state = state;
    }

    void setSenko(ObjectPressSenko *senko) {
        m_senko = senko;
    }

private:
    void calcPosition();

    u32 m_state;
    ObjectPressSenko *m_senko;
};

} // namespace Field
