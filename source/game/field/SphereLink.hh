#pragma once

#include <egg/math/Vector.hh>

namespace Field {

/// @brief One link in a chain where each link's position is limited by the other links.
class SphereLink {
public:
    SphereLink();
    ~SphereLink();

    void initLinkLen(f32 length);
    void init();
    void calcStiffness();
    void calc();
    void calcConstraints(f32 scale);
    void checkCollision();
    void calcPos();

    [[nodiscard]] bool isLeader() const {
        return !m_prev;
    }

    /// @beginSetters
    void setPrev(SphereLink *prev) {
        m_prev = prev;
    }

    void setNext(SphereLink *next) {
        m_next = next;
    }

    void setPos(const EGG::Vector3f &pos) {
        m_pos = pos;
    }

    void setVel(const EGG::Vector3f &vel) {
        m_vel = vel;
    }

    void addSpringForce(const EGG::Vector3f &force) {
        m_springForce += force;
    }
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] f32 linkLen() const {
        return m_linkLen;
    }

    [[nodiscard]] const EGG::Vector3f &pos() const {
        return m_pos;
    }

    [[nodiscard]] const EGG::Vector3f &up() const {
        return m_up;
    }

    [[nodiscard]] static constexpr const EGG::Vector3f &Gravity() {
        return GRAVITY;
    }
    /// @endGetters

private:
    void calcSpring();

    SphereLink *m_prev;
    SphereLink *m_next;
    f32 m_linkLen; ///< The maximum distance between this link and the previous (forward) link
    EGG::Vector3f m_pos;
    EGG::Vector3f m_vel;
    EGG::Vector3f m_springForce; ///< Prevents links from stretching past their m_linkLen
    EGG::Vector3f m_up;
    bool m_touchingGround;

    static constexpr EGG::Vector3f GRAVITY = EGG::Vector3f(0.0f, 2.5f, 0.0f);
};

} // namespace Field
