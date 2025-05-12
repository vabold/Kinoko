#pragma once

#include "egg/math/Vector.hh"
#include "game/field/obj/ObjectKCL.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

enum class KinokoType : u16 {
    Light = 0,
    Dark = 1,
};

class ObjectKinokoStem : public ObjectCollidable {
public:
    ObjectKinokoStem(const System::MapdataGeoObj &params, KinokoType type);
    ~ObjectKinokoStem() override;

    [[nodiscard]] const char *getKclName() const override;
private:
    KinokoType m_type;
};

class ObjectKinokoNm : public ObjectKCL {
public:
    ObjectKinokoNm(const System::MapdataGeoObj &params);
    ~ObjectKinokoNm() override;

    [[nodiscard]] const char *getKclName() const override;
private:
    KinokoType m_type;
    ObjectKinokoStem *m_stem_obj;
};

class ObjectKinoko : public ObjectKCL {
public:
    ObjectKinoko(const System::MapdataGeoObj &params);
    ~ObjectKinoko() override;

    [[nodiscard]] void init() override;
    [[nodiscard]] void calc() override;
    [[nodiscard]] u32 loadFlags() const override;
    [[nodiscard]] const char *getKclName() const override;
    virtual void calcOscillation() = 0;

protected:
    KinokoType m_type;
    EGG::Vector3f m_obj_pos;
    EGG::Vector3f m_obj_scale;
    EGG::Vector3f m_obj_rot;
    ObjectKinokoStem *m_stem_obj;
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

class ObjectKinokoUd : public ObjectKinoko {
public:
    ObjectKinokoUd(const System::MapdataGeoObj &params);
    ~ObjectKinokoUd() override;

    [[nodiscard]] const char *getKclName() const override;
    [[nodiscard]] void calcOscillation() override;
    [[nodiscard]] void calcScale(u32 timeOffset) override;

private:
    u16 _14c;
    s16 _14e;
    s16 _150;
    s16 _152;
    //KinokoType m_ud_type;
    f32 m_ud_period;
};

class ObjectKinokoBend : public ObjectKinoko {
public:
    ObjectKinokoBend(const System::MapdataGeoObj &params);
    ~ObjectKinokoBend() override;
    [[nodiscard]] void calcOscillation() override;
    [[nodiscard]] void calcScale(u32 timeOffset) override;

private:
    s16 _158;
    s16 m_period_denom;
    f32 _15c;
    f32 m_bend_period;
};

}