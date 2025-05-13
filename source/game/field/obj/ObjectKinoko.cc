#include "game/field/obj/ObjectKinoko.hh"

namespace Field {

ObjectKinokoStem::ObjectKinokoStem(const System::MapdataGeoObj &params, KinokoType type)
    : ObjectCollidable(params) {
    m_type = type;
    load();
}

/// @addr{0x80807E90}
ObjectKinokoStem::~ObjectKinokoStem() = default;

ObjectKinokoNm::ObjectKinokoNm(const System::MapdataGeoObj &params) : ObjectKCL(params) {
    m_type = static_cast<KinokoType>(params.setting(0));
    m_stemObj = new ObjectKinokoStem(params, m_type);
}

/// @addr{0x80827A9C}
ObjectKinokoNm::~ObjectKinokoNm() = default;

/// @addr{0x8080761C}
ObjectKinoko::ObjectKinoko(const System::MapdataGeoObj &params)
    : ObjectKCL(params), m_objPos(m_pos), m_objRot(m_rot) {
    m_type = static_cast<KinokoType>(params.setting(0));

    _12e = 0;
    _144 = 6;
    _130 = 40;
    _132 = 10;
    _12c = 1 % 40;
    _134 = 0.1f;
    m_period = 6.0f * F_PI / 40.0f;
    _13c = 0.0008f;
    _146 = 0;

    m_stemObj = new ObjectKinokoStem(params, m_type);
}

/// @adrr{0x80807A54}
ObjectKinoko::~ObjectKinoko() = default;

/// @addr{0x8080782C}
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

    _134 = _13c * static_cast<f32>(_130 - _12c);
    m_flags |= 8; // |= SCALE_DIRTY
    m_scale.set(_134 * EGG::Mathf::sin(m_period * static_cast<f32>(_12c)) + 1.0f);

    calcOscillation();
}

/// @addr{0x80807950}
ObjectKinokoUd::ObjectKinokoUd(const System::MapdataGeoObj &params) : ObjectKinoko(params) {
    _14c = 0;
    _146 = params.setting(3);
    _150 = params.setting(4);
    _152 = params.setting(1);
    m_periodDenom = std::max<u16>(params.setting(2), 2);
    m_udPeriod = F_TAU / static_cast<f32>(m_periodDenom);
}

/// @addr{0x80807E1C}
ObjectKinokoUd::~ObjectKinokoUd() = default;

/// @addr{0x80807A54}
void ObjectKinokoUd::calcOscillation() {
    m_flags |= 1; // |= POSITION_DIRTY;
    m_pos.y = m_objPos.y +
            static_cast<f32>(_152) * (EGG::Mathf::cos(m_udPeriod * static_cast<f32>(_146)) + 1.0f) *
                    0.5f;

    if (_14c == 0) {
        _146++;
    }
    if (_146 == (m_periodDenom / 2)) {
        _14c++;
    }
    if (_14c > _150) {
        _14c = 0;
    }
    if (_146 > m_periodDenom) {
        _146 = 0;
    }
}

/// @addr{0x80807B7C}
ObjectKinokoBend::ObjectKinokoBend(const System::MapdataGeoObj &params) : ObjectKinoko(params) {
    _158 = params.setting(3);
    _15c = static_cast<f32>(params.setting(1)) * DEG2RAD;

    m_periodDenom = std::max<u16>(params.setting(2), 2);
    m_bendPeriod = F_TAU / static_cast<f32>(m_periodDenom);
}

/// @addr{0x80807DB4}
ObjectKinokoBend::~ObjectKinokoBend() = default;

/// @addr{0x80807C98}
void ObjectKinokoBend::calcOscillation() {
    const f32 s = EGG::Mathf::sin(m_bendPeriod * static_cast<f32>(_158));
    EGG::Vector3f v1(EGG::Vector3f::ez * s);
    EGG::Vector3f v2(v1 * static_cast<f32>(_15c));
    EGG::Vector3f v3(m_objRot + v2);

    calcTransform();

    m_flags |= 2; // |= ROTATION_DIRTY;
    m_rot = m_transform.multVector33(v3);

    if (++_158 >= m_periodDenom) {
        _158 = 0;
    }
}

} // namespace Field
