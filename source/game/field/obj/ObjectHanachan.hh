#pragma once

#include "game/field/SphereLink.hh"
#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Class that interfaces with the chain links corresponding to Wiggler body parts.
/// @details Responsible for updating chain link objects and enforcing constraints when the chain is
/// taut. @ref ObjectHanachan uses this class to retrieve chain link positions so it can update the
/// Wiggler body parts accordingly.
class HanachanChainManager {
public:
    HanachanChainManager(const std::span<const f32> &linkDistances);
    ~HanachanChainManager();

    void init();
    void calc();

    /// @beginSetters

    /// @addr{0x806F43E8}
    /// @brief Sets the position of the specified Wiggler body segment
    /// @param idx The index of the body segment
    /// @param pos The new position of the body segment
    void setPos(size_t idx, const EGG::Vector3f &pos) {
        ASSERT(idx < m_links.size());
        m_links[idx].setPos(pos);
    }

    /// @addr{0x806F451C}
    /// @brief Sets the velocity of the specified Wiggler body segment
    /// @param idx The index of the body segment
    /// @param v The new velocity of the body segment
    void setVel(size_t idx, const EGG::Vector3f &v) {
        ASSERT(idx < m_links.size());
        m_links[idx].setVel(v);
    }

    /// @addr{0x806F45A4}
    /// @brief Adds a spring force to the specified Wiggler body segment
    /// @param idx The index of the body segment
    /// @param v The spring force to add to the body segment
    void addSpringForce(size_t idx, const EGG::Vector3f &v) {
        ASSERT(idx < m_links.size());
        m_links[idx].addSpringForce(v);
    }

    /// @endSetters

    /// @beginGetters

    /// @addr{0x806F47B0}
    /// @brief Retrieves the position of the specified Wiggler body segment
    /// @param idx The index of the body segment
    /// @return The position of the body segment
    /// @pre `idx` must be less than the number of links in the chain
    [[nodiscard]] const EGG::Vector3f &pos(size_t idx) const {
        ASSERT(idx < m_links.size());
        return m_links[idx].pos();
    }

    /// @addr{0x806F481C}
    /// @brief Retrieves the up vector of the specified Wiggler body segment
    /// @param idx The index of the body segment
    /// @return The up vector of the body segment
    /// @pre `idx` must be less than the number of links in the chain
    [[nodiscard]] const EGG::Vector3f &up(size_t idx) const {
        ASSERT(idx < m_links.size());
        return m_links[idx].up();
    }

    /// @endGetters

private:
    void calcConstraints();

    owning_span<SphereLink> m_links; ///< Array of links representing the Wiggler's body segments
};

/// @brief Base class for one of the spherical body segments of a Wiggler
class ObjectHanachanPart : public ObjectCollidable {
    /// @brief Grants the main ObjectHanachan class access to the part's state
    friend class ObjectHanachan;

public:
    ObjectHanachanPart(const System::MapdataGeoObj &params);
    ObjectHanachanPart(const char *name, const EGG::Vector3f &pos, const EGG::Vector3f &rot,
            const EGG::Vector3f &scale);
    ~ObjectHanachanPart() override;

    void loadRail() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getKclName() const override;

private:
    void calcTransformFromUpAndTangent(const EGG::Vector3f &pos, const EGG::Vector3f &up,
            const EGG::Vector3f &tangent);
};

/// @brief Represents the head of a Wiggler, which is the leading segment of the body chain
/// @details The head segment has a larger collision sphere than the body segments
class ObjectHanachanHead final : public ObjectHanachanPart {
public:
    ObjectHanachanHead(const char *name, const EGG::Vector3f &pos, const EGG::Vector3f &rot,
            const EGG::Vector3f &scale);
    ~ObjectHanachanHead() override;

    void createCollision() override;
    void calcCollisionTransform() override;

private:
    EGG::Vector3f m_lastPos; ///< Position from last frame, used to calculate speed
};

/// @brief Represents one of the body segments of a Wiggler that trails behind the head segment
class ObjectHanachanBody final : public ObjectHanachanPart {
    /// @brief Grants the main ObjectHanachan class access to the body segment's state
    friend class ObjectHanachan;

public:
    ObjectHanachanBody(const System::MapdataGeoObj &params, const char *mdlName);
    ObjectHanachanBody(const char *name, const EGG::Vector3f &pos, const EGG::Vector3f &rot,
            const EGG::Vector3f &scale, const char *mdlName);
    ~ObjectHanachanBody() override;

    void calcCollisionTransform() override;
    [[nodiscard]] const char *getKclName() const override;

protected:
    const char *m_mdlName;   ///< KCL name that corresponds to this body segment
    bool m_lastSegment;      ///< True if this is the last segment in the Wiggler's body chain
    EGG::Vector3f m_lastPos; ///< Position from last frame, used to calculate speed

private:
    /// @brief Static array of model names for the Wiggler's body segments, used to determine which
    /// model to load for each segment
    static constexpr std::array<const char *, 4> MDL_NAMES = {
            "hanachan_body1",
            "hanachan_body2",
            "hanachan_body3",
            "hanachan_body4",
    };
};

/// @brief Represents a Wiggler, which is comprised of multiple @ref ObjectHanachanPart segments
class ObjectHanachan final : public ObjectCollidable, private StateManager {
public:
    ObjectHanachan(const System::MapdataGeoObj &params);
    ~ObjectHanachan() override;

    void init() override;
    void calc() override;
    void loadGraphics() override;
    void createCollision() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    /// @brief Tracks whether the Wiggler is following its rail correctly or has deviated
    enum class RailAlignment {
        Aligned = -1,        ///< The Wiggler is closely following the rail
        Unknown = 0,         ///< Uninitialized
        MisalignedLeft = 1,  ///< The rail is deviating to the left from the Wiggler
        MisalignedRight = 2, ///< The rail is deviating to the right from the Wiggler
    };

    void enterWalk();
    void calcWalk();
    void calcWait();

    void onSegmentEnd();
    void initRail();
    void calcRail();
    void initBody();
    void calcBody();
    void initChain();
    void clearChain();
    void calcRailAlignmentMotion();
    void calcSway();
    void calcDefaultLateralMotion();
    void calcFastLateralMotion(s32 frame);
    void calcLateralMotion(f32 amplitude, f32 period, f32 wavelength, s16 frame);
    void setRailVel();
    [[nodiscard]] RailAlignment calcRailAlignment() const;
    [[nodiscard]] bool shouldStartMoving() const;

    /// @beginGetters

    /// @brief Gets the head part of the Wiggler via a `reinterpret_cast`
    /// @return A pointer to the head part of the Wiggler
    [[nodiscard]] ObjectHanachanHead *&headPart() {
        return reinterpret_cast<ObjectHanachanHead *&>(m_parts[0]);
    }

    /// @brief Gets a span of all body parts except the head of the Wiggler
    /// @return A span containing pointers to all body parts except the head
    [[nodiscard]] std::span<ObjectHanachanPart *> bodyParts() {
        return std::span(m_parts.begin() + 1, m_parts.size() - 1);
    }

    /// @endGetters

    /// @brief The number of body parts that make up a Wiggler, including the head and body segments
    static constexpr size_t PART_COUNT = 7;

    std::array<ObjectHanachanPart *, PART_COUNT> m_parts; ///< Array of body segment pointers
    HanachanChainManager m_chain; ///< Manager that enforces stretch constraints of the chain link
    const f32 m_walkSpeed;        ///< The speed at which the Wiggler moves along its rail
    std::array<float, PART_COUNT> m_partDisplacement; ///< Distance between a part and the head
    u16 m_stillDuration;     ///< How long the Wiggler waits at the current rail point
    f32 m_swayAmplitude;     ///< Amplitude of lateral sway motion when walking or standing still
    bool m_still;            ///< True if the Wiggler is standing still, false if it is walking
    u16 m_leftMisalignFrame; ///< Frame at which the wiggler became left-misaligned from the rail
    EGG::Vector3f m_prevRailTangent; ///< Previous frame's rail tangent, used to detect sharp turns
    RailAlignment m_railAlignment;   ///< Describes if the Wiggler has deviated from its rail
    RailAlignment m_prevRailAlignment; ///< Used to detect when the Wiggler becomes misaligned

    /// @brief The distance between each body part link in the chain
    static constexpr std::array<f32, 6> BODY_PART_DISTANCES = {{
            360.0f,
            510.0f,
            510.0f,
            480.0f,
            510.0f,
            510.0f,
    }};

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 2> STATE_ENTRIES = {{
            {StateEntry<ObjectHanachan, &ObjectHanachan::enterWalk, &ObjectHanachan::calcWalk>(0)},
            {StateEntry<ObjectHanachan, nullptr, &ObjectHanachan::calcWait>(1)},
    }};

    /// @brief Initial amplitude of lateral sway motion when walking or standing still
    static constexpr f32 INIT_SWAY_AMPLITUDE = 15.0f;
};

} // namespace Kinoko::Field
