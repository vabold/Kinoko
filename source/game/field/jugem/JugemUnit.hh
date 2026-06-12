#pragma once

#include "game/field/jugem/JugemMove.hh"
#include "game/field/jugem/JugemSwitch.hh"

#include "game/field/StateManager.hh"

#include "game/kart/KartObject.hh"

namespace Kinoko::Field {

/// @brief Manages interpolation keyframes for the Lakitu's position when ascending and descending.
class JugemInterp {
public:
    JugemInterp(u32 count) {
        m_keyframes = owning_span<InterpKeyframe>(count);
        m_time = 0.0f;
        m_rate = 0.0f;
    }

    /// @addr{0x80721E64}
    ~JugemInterp() = default;

    /// @addr{0x8072370C}
    void initKeyframes(f32 startRate, f32 endRate, f32 tDelta, u32 param4) {
        m_numKeyframes = 0;

        setStartKeyframe(startRate);
        addKeyframe(endRate, tDelta, param4);

        m_rate = startRate;
        m_time = 0.0f;
    }

    /// @addr{0x80723AF8}
    bool calc(f32 tDelta) {
        m_rate = calcInterpRate(m_time);
        m_time += tDelta;

        bool endOfKeyframe = m_time > m_keyframes[1].m_time;
        if (endOfKeyframe) {
            m_time = m_keyframes[1].m_time;
        }

        return endOfKeyframe;
    }

    /// @addr{0x8074C048}
    void setStartKeyframe(f32 param1) {
        m_keyframes[m_numKeyframes].m_interpRate = param1;
        m_keyframes[m_numKeyframes].m_time = 0.0f;
        ++m_numKeyframes;
    }

    /// @addr{0x8074C0B4}
    void addKeyframe(f32 rate, f32 tDelta, u32 /*param4*/) {
        m_keyframes[m_numKeyframes].m_interpRate = rate;
        m_keyframes[m_numKeyframes].m_time = tDelta + m_keyframes[m_numKeyframes - 1].m_time;
        ++m_numKeyframes;
    }

    /// @addr{0x8074C1E0}
    [[nodiscard]] f32 calcInterpRate(f32 time) const {
        const InterpKeyframe *currKeyframe = nullptr;
        const InterpKeyframe *nextKeyframe = nullptr;
        u32 iVar5 = 0;

        for (u32 idx = 0; idx < m_numKeyframes - 1; ++idx) {
            currKeyframe = &m_keyframes[iVar5];
            nextKeyframe = &m_keyframes[iVar5 + 1];
            if (currKeyframe->m_time <= time && time < nextKeyframe->m_time) {
                break;
            }
        }

        f32 timePastKeyframe = time - currKeyframe->m_time;
        f32 keyframeDuration = nextKeyframe->m_time - currKeyframe->m_time;

        timePastKeyframe = std::clamp(timePastKeyframe, 0.0f, keyframeDuration);

        return Lerp(currKeyframe->m_interpRate, nextKeyframe->m_interpRate,
                timePastKeyframe / keyframeDuration);
    }

    [[nodiscard]] f32 rate() const {
        return m_rate;
    }

private:
    struct InterpKeyframe {
        InterpKeyframe() : m_interpRate(0.0f), m_time(0.0f) {}

        f32 m_interpRate;
        f32 m_time;
    };

    /// @addr{0x8074C3F0}
    [[nodiscard]] static f32 Lerp(f32 a, f32 b, f32 t) {
        f32 sin = EGG::Mathf::SinFIdx(RAD2FIDX * (-HALF_PI + t * (HALF_PI - -HALF_PI)));
        f32 interpFactor = std::clamp(0.5f * (1.0f + sin), 0.0f, 1.0f);

        return a + interpFactor * (b - a);
    }

    owning_span<InterpKeyframe> m_keyframes;
    u32 m_numKeyframes;
    f32 m_time;
    f32 m_rate;
};

/// @brief Represents a single Lakitu and houses some state management members.
class JugemUnit : public StateManager {
public:
    JugemUnit(const Kart::KartObject *kartObj);
    ~JugemUnit();

    /// @addr{0x80721EC0}
    void createSwitchRace() {
        m_switchReverse = new JugemSwitchReverse;
    }

    /// @addr{0x80722100}
    void init() {
        m_pos.setZero();
        m_move->init();
    }

    void calc();

private:
    enum class State {
        Away = 0,
        Descending = 1,
        Stay = 2,
        Ascending = 3,
    };

    /// @addr{0x80723458}
    void enterIdle() {
        m_state = State::Away;
        m_ascendTimer = 0;
    }

    void enterReverse();

    /// @addr{0x807234A4}
    void calcIdle() {
        if (m_switchReverse && m_switchReverse->isOn()) {
            m_nextStateId = 1;
        }
    }

    void calcReverse();

    /// @addr{0x80722ED8}
    void calcSwitches() {
        if (m_switchReverse) {
            m_switchReverse->calc();
        }
    }

    /// @addr{0x80722F6C}
    void setPosFromTransform(const EGG::Matrix34f &mat) {
        m_pos = mat.base(3);
        m_pos.y = m_kartObj->pos().y;
    }

    [[nodiscard]] EGG::Vector3f transformLocalToWorldUpright(const EGG::Vector3f &v) const;
    void calcCollision();

    const Kart::KartObject *m_kartObj;
    EGG::Vector3f m_pos;
    State m_state;
    u32 m_ascendTimer; ///< How long Lakitu has been disappearing/ascending for
    JugemSwitch *m_switchReverse;
    JugemMove *m_move;
    JugemInterp *m_interp;

    [[nodiscard]] static EGG::Vector3f Interpolate(f32 t, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1) {
        return v0 + (v1 - v0) * t;
    }

    static constexpr std::array<StateManagerEntry, 2> STATE_ENTRIES = {{
            StateEntry<JugemUnit, &JugemUnit::enterIdle, &JugemUnit::calcIdle>(0),
            StateEntry<JugemUnit, &JugemUnit::enterReverse, &JugemUnit::calcReverse>(1),
    }};
};

} // namespace Kinoko::Field
