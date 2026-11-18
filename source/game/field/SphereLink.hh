#pragma once

#include <egg/math/Vector.hh>

namespace Kinoko::Field {

/// @brief Represents one link in a chain where each link's position is limited by the other links
/// @details Used by @ref HanachanChainManager to ensure that each body segment stays connected to
/// neighboring segments.
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

    /// @beginSetters

    /// @brief Sets the previous link that this link is connected to
    /// @param prev The previous link that this link should be connected to
    void setPrev(SphereLink *prev) {
        m_prev = prev;
    }

    /// @brief Sets the next link that this link is connected to
    /// @param next The next link that this link should be connected to
    void setNext(SphereLink *next) {
        m_next = next;
    }

    /// @brief Sets the position of the link in world space
    /// @param pos The new position of the link
    void setPos(const EGG::Vector3f &pos) {
        m_pos = pos;
    }

    /// @brief Sets the velocity of the link
    /// @param vel The new velocity of the link
    void setVel(const EGG::Vector3f &vel) {
        m_vel = vel;
    }

    /// @brief Adds a force to the link's spring force
    /// @param force The force to add to the spring force
    void addSpringForce(const EGG::Vector3f &force) {
        m_springForce += force;
    }

    /// @endSetters

    /// @beginGetters

    /// @brief Determines whether this link is at the front of the chain
    /// @return `true` if this link has no previous link, `false` otherwise
    [[nodiscard]] bool isLeader() const {
        return !m_prev;
    }

    /// @brief Gets the length of the link
    /// @return The maximum distance between this link and the previous (forward) link, @ref
    /// m_linkLen
    [[nodiscard]] f32 linkLen() const {
        return m_linkLen;
    }

    /// @brief Gets the current position of the link in world space
    /// @return The current position of the link, @ref m_pos
    [[nodiscard]] const EGG::Vector3f &pos() const {
        return m_pos;
    }

    /// @brief Gets the smoothed up vector of the link
    /// @return The smoothed up vector of the link, @ref m_smoothedUp
    [[nodiscard]] const EGG::Vector3f &up() const {
        return m_smoothedUp;
    }

    /// @endGetters

    /// @brief The gravity vector applied to the link's velocity every frame
    static constexpr EGG::Vector3f GRAVITY = EGG::Vector3f(0.0f, 2.5f, 0.0f);

private:
    void calcSpring();

    SphereLink *m_prev;  ///< The previous link in the chain, or nullptr if this is the first link
    SphereLink *m_next;  ///< The next link in the chain, or nullptr if this is the last link
    f32 m_linkLen;       ///< The maximum distance between this link and the previous (forward) link
    EGG::Vector3f m_pos; ///< The current position of the link in world space
    EGG::Vector3f m_vel; ///< The current velocity of the link
    EGG::Vector3f m_springForce; ///< Prevents links from stretching past their m_linkLen
    EGG::Vector3f m_smoothedUp;  ///< Smoothed up vector for the link
    bool m_touchingGround;       ///< Whether the link is currently touching the ground

    /// @brief Magnitude of the spring force applied to the link when it is stretched beyond its
    /// link length
    static constexpr f32 SPRING_FORCE_MAGNITUDE = 2.0f;
};

} // namespace Kinoko::Field
