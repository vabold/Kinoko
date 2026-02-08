#pragma once

#include "game/field/obj/ObjectCollidable.hh"

#include "game/render/DrawMdl.hh"

namespace Field {

class ObjectKoopaBall final : public ObjectCollidable {
public:
    ObjectKoopaBall(const System::MapdataGeoObj &params);
    ~ObjectKoopaBall() override;

    void init() override;
    void calc() override;

    /// @addr{0x80771F68}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    /// @addr{0x80771F60}
    const EGG::Vector3f &getCollisionTranslation() const override {
        return m_vel;
    }

private:
    enum class State {
        Tangible = 0,
        Intangible = 1,
        Exploding = 2,
    };

    void calcTangible();
    void calcExploding();
    void calcIntangible();

    void checkSphereFull();

    State m_state;
    f32 m_initPosY; ///< Starting height
    EGG::Vector3f m_vel;
    f32 m_angSpeed;
    s32 m_cooldownTimer; ///< Frames until koopa will start to shoot the fireball
    Render::DrawMdl *m_bombCoreDrawMdl;
    s32 m_explodeTimer;
    u32 m_animFramecount;
    f32 m_angleRad;
    f32 m_curScale;

    static constexpr f32 RADIUS_AABB = 870.0f * 2.0f;
    static constexpr f32 SCALE_INITIAL = RADIUS_AABB / 940.0f * 0.5f;
    static constexpr f32 SCALE_DELTA = SCALE_INITIAL / 25.0f;
    static constexpr f32 INITIAL_VELOCITY = 400.0f;
    static constexpr f32 INITIAL_ANGULAR_SPEED = 3.0f;
    static constexpr f32 INITIAL_Y_VEL = -30.0f;
};

} // namespace Field
