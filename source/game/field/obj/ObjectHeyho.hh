#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectHeyho;

/// @brief Shy guys on DK Summit.
class ObjectHeyho : public ObjectCollidable, public StateManager<ObjectHeyho> {
    friend StateManager<ObjectHeyho>;

public:
    ObjectHeyho(const System::MapdataGeoObj &params);
    ~ObjectHeyho() override;

    void init() override;
    void calc() override;
    void loadAnims() override;
    void calcCollisionTransform() override;

    /// @addr{0x806D02B4}
    [[nodiscard]] u32 loadFlags() const override {
        return 3;
    }

private:
    enum class Animation {
        Move = 1,
        Jump = 2,
        Jumped = 3,
    };

    void changeAnimation(Animation anim) {
        m_drawMdl->anmMgr()->playAnim(0.0f, 1.0f, static_cast<size_t>(anim));
        m_currentAnim = anim;
    }

    // State methods

    /// @addr{0x806CF4CC}
    void enterMove() {}

    /// @addr{0x806CF714}
    void enterJump() {
        m_spinFrame = 0;
    }

    void calcMove();
    void calcJump();

    void calcStateTransition();
    void calcMotion();
    void calcInterp();

    const s32 m_color;
    f32 m_apex;
    EGG::Vector3f m_midpoint;
    EGG::Vector3f m_transformOffset;
    f32 m_currentVel;
    f32 m_accel;
    f32 m_maxVelSq;
    EGG::Vector3f m_up;
    EGG::Vector3f m_forward;
    EGG::Vector3f m_floorNrm;
    bool m_floorCollision;
    Animation m_currentAnim;
    bool m_freeFall;
    f32 m_launchVel;
    s16 m_spinFrame;

    static constexpr std::array<StateManagerEntry<ObjectHeyho>, 2> STATE_ENTRIES = {{
            {0, &ObjectHeyho::enterMove, &ObjectHeyho::calcMove},
            {1, &ObjectHeyho::enterJump, &ObjectHeyho::calcJump},
    }};

    static constexpr EGG::Vector3f COLLISION_OFFSET = EGG::Vector3f(0.0f, 10.0f, 0.0f);
    static constexpr f32 COLLISION_RADIUS = 100.0f;
};

} // namespace Field
