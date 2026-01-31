#pragma once

#include "game/field/ObjectCollisionSphere.hh"
#include "game/field/SphereLink.hh"
#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief Class that interfaces with the chain links corresponding to wiggler body parts.
class HanachanChainManager {
public:
    HanachanChainManager(const std::span<const f32> &linkDistances);
    ~HanachanChainManager();

    /// @addr{0x806F3370}
    void init() {
        for (auto &link : m_links) {
            link.init();
        }
    }

    void calc();

    /// @addr{0x806F43E8}
    void setPos(size_t idx, const EGG::Vector3f &pos) {
        ASSERT(idx < m_links.size());
        m_links[idx].setPos(pos);
    }

    /// @addr{0x806F451C}
    void setVel(size_t idx, const EGG::Vector3f &v) {
        ASSERT(idx < m_links.size());
        m_links[idx].setVel(v);
    }

    /// @addr{0x806F47B0}
    [[nodiscard]] const EGG::Vector3f &pos(size_t idx) const {
        ASSERT(idx < m_links.size());
        return m_links[idx].pos();
    }

    /// @addr{0x806F481C}
    [[nodiscard]] const EGG::Vector3f &up(size_t idx) const {
        ASSERT(idx < m_links.size());
        return m_links[idx].up();
    }

    /// @addr{0x806F45A4}
    void addSpringForce(size_t idx, const EGG::Vector3f &v) {
        ASSERT(idx < m_links.size());
        m_links[idx].addSpringForce(v);
    }

private:
    /// @addr{0x806F5290}
    void calcConstraints() {
        for (size_t i = 1; i < m_links.size(); ++i) {
            m_links[i].calcConstraints(1.0f);
        }
    }

    std::span<SphereLink> m_links;
};

class ObjectHanachanPart : public ObjectCollidable {
    friend class ObjectHanachan;

public:
    ObjectHanachanPart(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

    ObjectHanachanPart(const char *name, const EGG::Vector3f &pos, const EGG::Vector3f &rot,
            const EGG::Vector3f &scale)
        : ObjectCollidable(name, pos, rot, scale) {}

    /// @addr{0x806C7E68}
    ~ObjectHanachanPart() = default;

    /// @addr{0x806CCAD0}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806CCB88}
    [[nodiscard]] const char *getKclName() const override {
        return "hanachan";
    }

    /// @addr{0x806CCACC}
    void loadRail() override {}

private:
    void calcTransformFromUpAndTangent(const EGG::Vector3f &pos, const EGG::Vector3f &up,
            const EGG::Vector3f &tangent);
};

class ObjectHanachanHead final : public ObjectHanachanPart {
public:
    ObjectHanachanHead(const char *name, const EGG::Vector3f &pos, const EGG::Vector3f &rot,
            const EGG::Vector3f &scale);
    ~ObjectHanachanHead() override;

    /// @addr{0x806C818C}
    void createCollision() override {
        m_collision = new ObjectCollisionSphere(150.0f, collisionCenter());
    }

    void calcCollisionTransform() override;

private:
    EGG::Vector3f m_lastPos;
};

class ObjectHanachanBody final : public ObjectHanachanPart {
    friend class ObjectHanachan;

public:
    ObjectHanachanBody(const System::MapdataGeoObj &params, const char *mdlName);
    ObjectHanachanBody(const char *name, const EGG::Vector3f &pos, const EGG::Vector3f &rot,
            const EGG::Vector3f &scale, const char *mdlName);
    ~ObjectHanachanBody() override;

    /// @addr{0x806CCB80}
    [[nodiscard]] const char *getKclName() const override {
        return m_mdlName;
    }

    void calcCollisionTransform() override;

protected:
    const char *m_mdlName;
    bool m_lastSegment;
    EGG::Vector3f m_lastPos;

private:
    static constexpr std::array<const char *, 4> MDL_NAMES = {
            "hanachan_body1",
            "hanachan_body2",
            "hanachan_body3",
            "hanachan_body4",
    };
};

class ObjectHanachan final : public ObjectCollidable, public StateManager {
public:
    ObjectHanachan(const System::MapdataGeoObj &params);
    ~ObjectHanachan() override;

    void init() override;

    /// @addr{0x806C9860}
    void calc() override {
        calcRail();
        StateManager::calc();
        calcBody();
    }

    /// @addr{0x806CC9FC}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806CC9F8}
    void loadGraphics() override {}

    /// @addr{0x806CC9F4}
    void createCollision() override {}

private:
    enum class RailAlignment {
        Aligned = -1,
        Unknown = 0,
        MisalignedLeft = 1,
        MisalignedRight = 2,
    };

    void initWalk();

    /// @addr{0x806C9EC8}
    void initWait() {}

    void calcWalk();
    void calcWait();

    void onSegmentEnd();
    void calcRail();
    void calcBody();
    void initBody();
    void initChain();
    void clearChain();
    void calcRailAlignmentMotion();

    /// @addr{0x806CAC74}
    void calcSlowMotion() {
        constexpr f32 PERIOD = 50.0f;
        constexpr f32 WAVELENGTH = 4000.0f;

        calcLateralMotion(m_stillAngVel, PERIOD, WAVELENGTH, m_currentFrame);
    }

    /// @addr{0x806CAC94}
    void calcFastMotion(s32 frame) {
        constexpr f32 AMPLITUDE = 25.0f;
        constexpr f32 PERIOD = 300.0f;
        constexpr f32 WAVELENGTH = 5500.0f;

        calcLateralMotion(AMPLITUDE, PERIOD, WAVELENGTH, static_cast<s16>(frame));
    }

    void calcLateralMotion(f32 amplitude, f32 period, f32 wavelength, s16 frame);
    [[nodiscard]] RailAlignment calcRailAlignment() const;

    /// @addr{0x806CAF9C}
    [[nodiscard]] bool shouldStartMoving() const {
        return m_currentFrame > m_stillDuration;
    }

    /// @addr{0x806CBE2C}
    void setMovingVel() {
        m_railInterpolator->setCurrVel(m_movingVel);
    }

    [[nodiscard]] ObjectHanachanHead *&headPart() {
        return reinterpret_cast<ObjectHanachanHead *&>(m_parts[0]);
    }

    [[nodiscard]] std::span<ObjectHanachanPart *> bodyParts() {
        return std::span(m_parts.begin() + 1, m_parts.size() - 1);
    }

    std::array<ObjectHanachanPart *, 7> m_parts;
    HanachanChainManager m_chain;
    const f32 m_movingVel;
    std::array<float, 7> m_partDisplacement; ///< Total distance between a given part and the head
    u16 m_stillDuration;
    f32 m_stillAngVel; ///< Affects body part swaying when coming to a standstill
    bool m_still;
    u16 m_leftMisalignFrame; ///< Frame at which the wiggler became left-misaligned from the rail
    EGG::Vector3f m_right;
    RailAlignment m_railAlignment; ///< Captures when the wiggle takes sharp turns
    RailAlignment m_prevRailAlignment;

    /// @brief The distance between each body part link in the chain
    static constexpr std::array<f32, 6> BODY_PART_DISTANCES = {{
            360.0f,
            510.0f,
            510.0f,
            480.0f,
            510.0f,
            510.0f,
    }};

    static constexpr std::array<StateManagerEntry, 2> STATE_ENTRIES = {{
            {StateEntry<ObjectHanachan, &ObjectHanachan::initWalk, &ObjectHanachan::calcWalk>(0)},
            {StateEntry<ObjectHanachan, &ObjectHanachan::initWait, &ObjectHanachan::calcWait>(1)},
    }};
};

} // namespace Field
