#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief The falling rocks on GCN DK Mountain
class ObjectRock : public ObjectCollidable {
public:
    ObjectRock(const System::MapdataGeoObj &params);
    ~ObjectRock() override;

    void init() override;
    void calc() override;

    /// @addr{0x8077037C}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x8077036C}
    [[nodiscard]] const EGG::Vector3f &getCollisionTranslation() const override {
        return m_colTranslate;
    }

    [[nodiscard]] Kart::Reaction onCollision(Kart::KartObject *kartObj,
            Kart::Reaction reactionOnKart, Kart::Reaction reactionOnObj,
            EGG::Vector3f &hitDepth) override;

private:
    enum class State {
        Tangible = 0,
        Intangible = 1,
    };

    void calcTangible();
    void calcIntangible();
    void calcTangibleSub();

    void checkSphereFull();
    void breakRock();

    State m_state;
    f32 m_startYPos;
    EGG::Vector3f m_colTranslate;
    f32 m_angSpd;
    int m_cooldownTimer;
    f32 m_angRad;

    const s32 m_cooldownDuration;
    const f32 m_vel;
    const f32 m_intangibleHeight;

    static constexpr f32 INITIAL_ANGULAR_SPEED = 3.0f;
};

} // namespace Field
