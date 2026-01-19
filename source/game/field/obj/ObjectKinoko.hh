#pragma once

#include "game/field/obj/ObjectKCL.hh"

namespace Field {

enum class KinokoType : s16 {
    Light = 0,
    Dark = 1,
};

/// @brief The base class for a mushroom object with jump pad properties
class ObjectKinoko : public ObjectKCL {
public:
    ObjectKinoko(const System::MapdataGeoObj &params);
    ~ObjectKinoko() override;

    void calc() override;

    /// @addr {0x80807DAC}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x80807D8C}
    [[nodiscard]] const char *getKclName() const override {
        return m_type == KinokoType::Light ? "kinoko_r" : "kinoko_d_r";
    }
    virtual void calcOscillation() = 0;

protected:
    KinokoType m_type;
    const EGG::Vector3f m_objPos; // the initial position of the object
    const EGG::Vector3f m_objRot; // the initial rotation of the object
    s16 m_pulseFrame;
    s16 m_restFrame;
    s16 m_pulseFalloff;
    s16 m_oscFrame;
};

/// @brief Mushrooms which oscillate up and down. The stem does not move.
/// @details This represents the first two mushrooms on MG, even though they don't oscillate up or
/// down.
class ObjectKinokoUd : public ObjectKinoko {
public:
    ObjectKinokoUd(const System::MapdataGeoObj &params);
    ~ObjectKinokoUd() override;

    void calcOscillation() override;

    /// @addr{0x80807DFC}
    /// @details The base game does check for the light type, however since m_type never gets set
    /// it'll always be 0 which means it always returns "kinoko_r"
    [[nodiscard]] const char *getKclName() const override {
        return "kinoko_r";
    }

    /// @addr{0x80807DF8}
    void calcScale(u32) override {}

private:
    u16 m_waitFrame;
    s16 m_period;
    s16 m_waitDuration;
    s16 m_amplitude;
    f32 m_angFreq;
};

/// @brief Mushrooms which bend in a certain direction
/// @details This functionality didn't get used in the base game?
class ObjectKinokoBend : public ObjectKinoko {
public:
    ObjectKinokoBend(const System::MapdataGeoObj &params);
    ~ObjectKinokoBend() override;
    void calcOscillation() override;

    /// @addr{0x80807D88}
    void calcScale(u32) override {}

private:
    s16 m_currentFrame;
    s16 m_period;
    f32 m_amplitude;
    f32 m_angFreq;
};

/// @brief The class for a mushroom object with normal road properties, for the most part
class ObjectKinokoNm : public ObjectKCL {
public:
    ObjectKinokoNm(const System::MapdataGeoObj &params);
    ~ObjectKinokoNm() override;

    /// @addr{0x80827A74}
    [[nodiscard]] const char *getKclName() const override {
        return m_type == KinokoType::Light ? "kinoko_g" : "kinoko_d_g";
    }

private:
    KinokoType m_type;
};

} // namespace Field
