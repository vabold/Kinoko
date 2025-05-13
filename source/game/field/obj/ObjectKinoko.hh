#pragma once

#include "game/field/obj/ObjectCollidable.hh"
#include "game/field/obj/ObjectKCL.hh"

namespace Field {

enum class KinokoType : u16 {
    Light = 0,
    Dark = 1,
};

/// @brief The class that represents the mushroom's stem
class ObjectKinokoStem : public ObjectCollidable {
public:
    ObjectKinokoStem(const System::MapdataGeoObj &params, KinokoType type);
    ~ObjectKinokoStem() override;

    /// @addr{0x80807E64}
    [[nodiscard]] const char *getKclName() const override {
        return m_type == KinokoType::Light ? "kinoko_kuki" : "kinoko_d_kuki";
    }

private:
    KinokoType m_type;
};

/// @brief The base class for a mushroom object with normal road properties, for the most part
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
    ObjectKinokoStem *m_stemObj;
};

/// @brief The base class for a mushroom object with jump pad properties
class ObjectKinoko : public ObjectKCL {
public:
    ObjectKinoko(const System::MapdataGeoObj &params);
    ~ObjectKinoko() override;

    [[nodiscard]] void calc() override;

    /// @addr {0x80807DAC}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x80807D8C}
    [[nodiscard]] const char *getKclName() const {
        return m_type == KinokoType::Light ? "kinoko_r" : "kinoko_d_r";
    }
    virtual void calcOscillation() = 0;

protected:
    KinokoType m_type;
    EGG::Vector3f m_objPos;
    EGG::Vector3f m_objRot;
    ObjectKinokoStem *m_stemObj;
    s16 _12c;
    s16 _12e;
    s16 _130;
    s16 _132;
    s16 _134;
    f32 _13c;
    f32 m_period;
    u16 _144;
    u16 _146;
};

/// @brief Mushrooms which oscillate up and down. The stem does not move.
/// @details This represents the first two mushrooms on MG, even though they don't oscillate up or
/// down.
class ObjectKinokoUd : public ObjectKinoko {
public:
    ObjectKinokoUd(const System::MapdataGeoObj &params);
    ~ObjectKinokoUd() override;

    [[nodiscard]] void calcOscillation() override;

    /// @addr{0x80807DFC}
    /// @details The base game does check for the light type, however since m_type never gets set
    /// it'll always be 0 which means it always returns "kinoko_r"
    [[nodiscard]] const char *getKclName() const {
        return "kinoko_r";
    }

    /// @addr{0x80807DF8}
    [[nodiscard]] void calcScale(u32 timeOffset) override {}

private:
    u16 _14c;
    s16 m_periodDenom;
    s16 _150;
    s16 _152;
    f32 m_udPeriod;
};

/// @brief Mushrooms which bend in a certain direction
/// @details This functionality didn't get used in the base game?
class ObjectKinokoBend : public ObjectKinoko {
public:
    ObjectKinokoBend(const System::MapdataGeoObj &params);
    ~ObjectKinokoBend() override;
    [[nodiscard]] void calcOscillation() override;

    /// @addr{0x80807D88}
    [[nodiscard]] void calcScale(u32 timeOffset) override {}

private:
    s16 _158;
    s16 m_periodDenom;
    f32 _15c;
    f32 m_bendPeriod;
};

} // namespace Field
