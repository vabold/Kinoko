#include "game/field/obj/ObjectKinoko.hh"

namespace Field {
ObjectKinoko::ObjectKinoko(const System::MapdataGeoObj &params) : ObjectKCL(params),
m_obj_pos(m_pos),
m_obj_scale(m_scale),
m_obj_rot(m_rot)
{
    m_type = (KinokoType)params.setting(0);

    _12e = 0;
    _144 = 6;
    _130 = 40;
    _132 = 10;
    _12c = 1 % 40;
    _134 = 0.1f;
    m_period = _144 * F_PI / _130;
    _13c = 0.0008f;
    _146 = 0;

    m_stem_obj = new ObjectKinokoStem(params, m_type);
    load();
}

void ObjectKinoko::init() {}

// 8080782c
void ObjectKinoko::calc() {
    if (_12e == 0) {
        _12c++;
    }
    if (_12c == _130) {
        _12e++;
    }
    if (_132 < _12e) {
        _12e = 0;
    }
    if (_130 < _12c) {
        _12c = 0;
    }

    _134 = _13c * (f32)(_130 - _12c);
    m_flags |= 8; // SCALE_DIRTY
    m_scale.set(_134 * EGG::Mathf::sin(m_period * (f32)_12c) + 1.0f);

    calcOscillation();
}

ObjectKinokoUd::ObjectKinokoUd(const System::MapdataGeoObj &params) : ObjectKinoko(params) {
    _14c = 0;
    _146 = params.setting(3);
    _14e = params.setting(2);
    _150 = params.setting(4);
    _152 = params.setting(1);
    m_ud_period = F_TAU / (f32)_14e;
}

ObjectKinoko::~ObjectKinoko() = default;

void ObjectKinokoUd::calcOscillation() {
    m_flags |= 1; // |= POSITION_DIRTY;
    m_obj_pos.y = m_pos.y + (f32)_152 * (EGG::Mathf::cos(m_ud_period * (f32)_146) + 1.0f) * 0.5f;
    
    if (_14c == 0) {
        _146++;
    }
    if (_146 == (_14e / 2)) {
        _14c++;
    }
    if (_150 < _14c) {
        _14c = 0;
    }
    if (_14e < _146) {
        _146 = 0;
    }
}

ObjectKinokoBend::ObjectKinokoBend(const System::MapdataGeoObj &params) : ObjectKinoko(params) {
    const s16 setting2 = params.setting(2);
    const s16 period_denom = setting2 >= 2 ? setting2 : 2;

    _158 = params.setting(3);
    _15c = (f32)params.setting(1) * DEG2RAD;
    _160 = params.setting(4);

    m_period_denom = period_denom;

    m_pos_x = m_pos.x;
    m_scale_x = m_pos.z;
    m_rot_x = m_pos.y - (f32)_160;
    m_bend_period = F_TAU / (f32)period_denom;
}

void ObjectKinokoBend::calcOscillation() {
    const f32 s = EGG::Mathf::sin(m_bend_period * (f32)_158);
    EGG::Vector3f v1(EGG::Vector3f::ez * s);
    EGG::Vector3f v2(v1 * (f32)_15c);
    EGG::Vector3f v3(m_obj_rot + v2);

    calcTransform();

    m_flags |= 2; // |= ROTATION_DIRTY;
    m_rot = m_transform.multVector33(v3);

    if (++_158 >= m_period_denom) {
        _158 = 0;
    }
}

const char *ObjectKinoko::getKclName() const {
    return m_type == KinokoType::Light ? "kinoko_r" : "kinoko_d_r";
}

ObjectKinokoBend::~ObjectKinokoBend() = default;

const char *ObjectKinokoUd::getKclName() const {
    return "kinoko_r";
}

ObjectKinokoUd::~ObjectKinokoUd() = default;

const char *ObjectKinokoStem::getKclName() const { 
    return m_type == KinokoType::Light ? "kinoko_kuki" : "kinoko_d_kuki";
}

ObjectKinokoStem::ObjectKinokoStem(const System::MapdataGeoObj &params, KinokoType type) : ObjectCollidable(params) {
    m_type = type;
}

ObjectKinokoStem::~ObjectKinokoStem() = default;

const char *ObjectKinokoNm::getKclName() const { 
    return m_type == KinokoType::Light ? "kinoko_g" : "kinoko_d_g";
}

ObjectKinokoNm::ObjectKinokoNm(const System::MapdataGeoObj &params) : ObjectKCL(params) {
    m_type = (KinokoType)params.setting(0);
    m_stem_obj = new ObjectKinokoStem(params, m_type);
}

ObjectKinokoNm::~ObjectKinokoNm() = default;

}