#include "ObjectFirebar.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x807678F4}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Constructs and loads the required number of @ref ObjectFireball objects based on
/// the number of spokes (param setting 4) and fireballs per spoke (param setting 1). Sets @ref
/// m_angSpeed based on param setting 2. For each @ref ObjectFireball, computes and caches the
/// distance from the firebar and angle about the firebar's rotation axis. Finally, Computes the
/// axis of rotation for the firebar and its initial tangent direction.
ObjectFirebar::ObjectFirebar(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      m_spokes(std::max<u32>(1, params.setting(3))),
      m_angSpeed(static_cast<f32>(static_cast<s16>(params.setting(1)))),
      m_fireballs(std::max<u32>(1, params.setting(0) * m_spokes)) {
    const f32 ringSpacing = 100.0f * static_cast<f32>(static_cast<s16>(params.setting(2)));

    for (size_t i = 0; i < m_fireballs.size(); ++i) {
        m_fireballs[i] = EGG::egg_new<ObjectFireball>(params);
        m_fireballs[i]->load();

        f32 ring = 1.0f + static_cast<f32>(i / m_spokes);
        m_fireballs[i]->setDistance(ring * ringSpacing);
        m_fireballs[i]->setAngle((360.0f / m_spokes) * (i % m_spokes));
    }

    EGG::Matrix34f mat;
    mat.makeR(rot());
    m_axis = mat.base(2);
    m_axis.normalise();
    m_initDir = m_axis.cross(RotateAxisAngle(HALF_PI, EGG::Vector3f::ex, m_axis));
    m_initDir.normalise();
}

/// @addr{0x807688AC}
/// @brief Default virtual destructor
ObjectFirebar::~ObjectFirebar() = default;

/// @addr{0x80767DEC}
/// @copybrief ObjectBase::init()
/// @details Resets the firebar's angle to zero.
void ObjectFirebar::init() {
    m_currAngle = 0.0f;
}

/// @addr{0x80767E04}
/// @copybrief ObjectBase::calc()
/// @details Rotates the firebar based on @ref m_angSpeed. Updates the positions of all
/// underlying fireballs accordingly.
void ObjectFirebar::calc() {
    m_currAngle += m_angSpeed / 60.0f;

    if (m_currAngle > 360.0f) {
        m_currAngle -= 360.0f;
    } else if (m_currAngle < 0.0f) {
        m_currAngle += 360.0f;
    }

    // OPT: Every fireball on the same spoke shares the same angle. For Kinoko, we can leverage this
    // fact to introduce a small performance improvement by building each spoke's rotation matrix
    // once instead of calling RotateAxisAngle() once per fireball.
    size_t spokeCount = std::min<size_t>(m_spokes, m_fireballs.size());
    for (size_t spoke = 0; spoke < spokeCount; ++spoke) {
        EGG::Matrix34f mat;
        mat.setBase(3, EGG::Vector3f::zero);
        mat.setAxisRotation((m_currAngle + m_fireballs[spoke]->angle()) * DEG2RAD, m_axis);

        for (size_t i = spoke; i < m_fireballs.size(); i += m_spokes) {
            auto *fireball = m_fireballs[i];
            fireball->setPos(pos() + mat.ps_multVector(m_initDir * fireball->distance()));
        }
    }
}

/// @addr{0x807687D8}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectFirebar::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x808CE358}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the firebar, `WLfirebarGC` or `koopaFirebar`
const char *ObjectFirebar::getKclName() const {
    return id() == ObjectId::WLFirebarGC ? "WLfirebarGC" : "koopaFirebar";
}

} // namespace Kinoko::Field
