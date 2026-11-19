#include "ObjectHeyhoBall.hh"

#include "game/field/ObjectDirector.hh"

#include "game/kart/Reaction.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x806D02C4}
/// @copydoc ObjectProjectile::ObjectProjectile(const System::MapdataGeoObj &)
/// @details Sets @ref m_airtime from param setting 2 and caches the cannonball's initial
/// position to @ref m_initPos. Finally, registers this object to the vector of managed objects
/// in @ref ObjectDirector.
ObjectHeyhoBall::ObjectHeyhoBall(const System::MapdataGeoObj &params)
    : ObjectProjectile(params),
      StateManager(this, STATE_ENTRIES),
      m_airtime(static_cast<f32>(params.setting(1))),
      m_initPos(params.pos()) {
    registerManagedObject();
}

/// @addr{0x806D1820}
/// @brief Default virtual destructor
ObjectHeyhoBall::~ObjectHeyhoBall() = default;

/// @addr{0x806D05F0}
/// @copybrief ObjectBase::init()
/// @details Starts the cannonball in the intangible state. Initializes some internal state
/// variables, including resizing the cannonball's @ref BoxColUnit to a radius of @ref
/// INIT_BLAST_RADIUS. Computes the ratio between the blast radius and the cannonball's radius. With
/// this ratio, computes @ref m_scaleChangeRate to control the rate at which the blast radius
/// changes.
/// @note @ref m_scaleChangeRate is normally computed in 0x806D0D84 which runs when the cannonball
/// begins exploding, however this value never changes. For Kinoko, we take creative liberty to move
/// the calculation here to avoid frequent re-computation.
void ObjectHeyhoBall::init() {
    m_nextStateId = 0;
    m_shipPos.setZero();
    m_workingPos = pos();
    m_intensity = ExplosionIntensity::ExplosionLoseItem;

    resize(INIT_BLAST_RADIUS, 0.0f);

    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    m_blastRadiusRatio = INIT_BLAST_RADIUS /
            static_cast<f32>(parse<s16>(flowTable.set(flowTable.slot(id()))->params.sphere.radius));
    m_scaleChangeRate =
            (1.2f * m_blastRadiusRatio - 1.0f) / EXPLOSION_VERTEX_FRAME / EXPLOSION_VERTEX_FRAME;
}

/// @addr{0x806D0780}
/// @copybrief ObjectBase::calc()
/// @details Evaluates the cannonball's state machine and applies new position of the
/// cannonball.
void ObjectHeyhoBall::calc() {
    StateManager::calc();
    setPos(m_workingPos);
}

/// @addr{0x806D18EC}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectHeyhoBall::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806D0880}
/// @copybrief ObjectCollidable::onCollision()
/// @param hitDepth The depth of the collision between the kart and the object
/// @return @ref Kart::Reaction::ExplosionLoseItem if the explosion recently occurred, @ref
/// Kart::Reaction::SpinSomeSpeed if the explosion has dissipated a bit, otherwise @ref
/// Kart::Reaction::WallAllSpeed if the explosion has not yet occurred.
/// @details References the explosion intensity to determine what reaction to apply. If the
/// explosion dissipated a bit, then this function also resets the hit depth.
Kart::Reaction ObjectHeyhoBall::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction /*reactionOnKart*/, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f &hitDepth) {
    if (m_currentStateId == 3) {
        if (m_intensity == ExplosionIntensity::ExplosionLoseItem) {
            return Kart::Reaction::ExplosionLoseItem;
        }

        hitDepth.setZero();
        return Kart::Reaction::SpinSomeSpeed;
    }

    return Kart::Reaction::WallAllSpeed;
}

/// @addr{0x806D10A4}
/// @brief Init callback function called by @ref ObjectHeyhoShipManager::init()
/// @details Caches the ship's position to @ref m_shipPos. Computes the XZ-plane direction from the
/// ship to the cannonball's target landing position. Derives the cannonball's @ref m_xzSpeed and
/// @ref m_initYSpeed such that it takes @ref m_airtime frames to land after being launched.
void ObjectHeyhoBall::initProjectile(const EGG::Vector3f &pos) {
    m_shipPos = pos;
    m_xzDir = m_initPos - m_shipPos;
    m_xzDir.y = 0.0f;
    m_xzDir.normalise2();

    EGG::Vector2f xzDist = EGG::Vector2f(m_shipPos.x - m_initPos.x, m_shipPos.z - m_initPos.z);
    m_xzSpeed = EGG::Mathf::sqrt(xzDist.dot()) / m_airtime;
    f32 yDist = m_initPos.y + -BALL_RADIUS - m_shipPos.y;
    m_initYSpeed = yDist / m_airtime + 0.5f * 4.0f * m_airtime;
}

/// @addr{0x806D188C}
/// @brief Callback function used by the @ref ObjectHeyhoShipManager to throw this cannonball
/// @details Transitions the cannonball to the falling state.
void ObjectHeyhoBall::onLaunch() {
    m_nextStateId = 1;
}

/// @addr{0x806D0A3C}
/// @brief Runs once when the cannonball is fired
/// @details Initializes the cannonball's @ref BoxColUnit if not already created. Resizes
/// the collision unit to a radius of @ref INIT_BLAST_RADIUS.
void ObjectHeyhoBall::enterFalling() {
    if (!unit()) {
        loadAABB(0.0f);
        resize(INIT_BLAST_RADIUS, 0.0f);
    }
}

/// @addr{0x806D0C0C}
/// @brief Runs once when the cannonball has landed and is blinking before exploding
/// @details Snaps the cannonball to @ref m_initPos but offset down by its @ref BALL_RADIUS so that
/// it is halfway clipped through the floor.
void ObjectHeyhoBall::enterBlinking() {
    m_workingPos = m_initPos + EGG::Vector3f::ey * -BALL_RADIUS;
}

/// @addr{0x806D0A14}
/// @brief Runs every frame that the cannonball is not visible
/// @details Snaps the cannonball's position to the @ref ObjectHeyhoShip
void ObjectHeyhoBall::calcIntangible() {
    m_workingPos = m_shipPos;
}

/// @addr{0x806D0AD8}
/// @brief Runs every frame that the cannonball is falling
/// @details Applies a gravitational force of `4.0f` and compares the resulting height with the
/// cannonball's target landing height. If the resulting height is above the target landing height,
/// sets the Y-component of @ref m_workingPos to that height and applies @ref m_xzSpeed to the X and
/// Z components of @ref m_workingPos. If the resulting height is below the target landing height,
/// then transitions the cannonball to the blinking state. To dampen the cannonball's position
/// snapping into the ground, updates @ref m_workingPos to the average between the current @ref
/// m_workingPos and the cannonball's target landing position in the floor.
void ObjectHeyhoBall::calcFalling() {
    constexpr f32 GRAVITY = 4.0f;

    f32 currentHeight =
            m_workingPos.y + (m_initYSpeed - GRAVITY * static_cast<f32>(m_currentFrame));
    if (currentHeight > m_initPos.y + -BALL_RADIUS) {
        m_workingPos.y = currentHeight;
        m_workingPos.x += m_xzDir.x * m_xzSpeed;
        m_workingPos.z += m_xzDir.z * m_xzSpeed;
    } else {
        m_nextStateId = 2;
        m_workingPos = (m_workingPos + m_initPos + EGG::Vector3f::ey * -BALL_RADIUS) * 0.5f;
    }
}

/// @addr{0x806D0CD8}
/// @brief Runs every frame that the cannonball is blinking before exploding
/// @details If the cannonball has been in the blinking state for `180` frames, transitions to
/// the exploding state.
void ObjectHeyhoBall::calcBlinking() {
    constexpr u32 REST_FRAMES = 180;

    if (m_currentFrame >= REST_FRAMES) {
        m_nextStateId = 3;
    }
}

/// @addr{0x806D0F24}
/// @brief Runs every frame the cannonball is exploding
/// @details Scales the explosion sphere over `46` frames using a capped downward parabola.
/// Determines the @ref m_intensity of the explosion based off of the time elapsed, so that @ref
/// onCollision returns the appropriate reaction. If the explosion state has elapsed `46` frames,
/// then resets the object's scale, unregisters the object's @ref BoxColUnit to disable collisions,
/// and transitions the cannonball to the intangible state.
void ObjectHeyhoBall::calcExploding() {
    constexpr u32 EXPLODE_FRAMES = 46;

    /// Colliding after this frame does not cause player to launch upwards and lose item
    constexpr u32 SPIN_FRAME = 32;

    if (m_currentFrame >= EXPLODE_FRAMES) {
        calcFinishedExplodingScale();

        if (unit()) {
            unregisterCollision();
        }

        m_nextStateId = 0;
        m_intensity = ExplosionIntensity::ExplosionLoseItem;
    } else {
        calcExplodingScale();
        m_intensity = m_currentFrame >= SPIN_FRAME ? ExplosionIntensity::SpinSomeSpeed :
                                                     ExplosionIntensity::ExplosionLoseItem;
    }
}

/// @addr{0x806D1570}
/// @brief Runs after the explosion has finished to set the final scale of the cannonball
/// @details Sets the cannonball's XYZ scale to `{1.001f, 1.001f, 1.001f}`.
void ObjectHeyhoBall::calcFinishedExplodingScale() {
    constexpr EGG::Vector3f BALL_SCALE = EGG::Vector3f(1.001f, 1.001f, 1.001f);

    setScale(BALL_SCALE);
}

/// @addr{0x806D14D8}
/// @brief Calculates the cannonball scale on the first 46 frames of the explosion
/// @details Let \f$R\f$ be @ref m_blastRadiusRatio, \f$k\f$ be @ref m_scaleChangeRate, and \f$f\f$
/// be @ref m_currentFrame. The scale follows a downward parabola capped at \f$R\f$:
/// \f[s(f) = \min\left(R,\ 1.2R - k\,(f - 40)^2\right), \quad k = \frac{1.2R - 1}{40^2}\f]
/// The parabola's vertex is at frame \f$40\f$ with an uncapped peak of \f$1.2R\f$. Since the
/// explosion ends at frame \f$46\f$, the descending half of the parabola is never observed.
void ObjectHeyhoBall::calcExplodingScale() {
    f32 shrinkFrames = static_cast<f32>(m_currentFrame) - EXPLOSION_VERTEX_FRAME;
    f32 scale = 1.2f * m_blastRadiusRatio + -m_scaleChangeRate * shrinkFrames * shrinkFrames;

    scale = std::min(m_blastRadiusRatio, scale);
    setScale(scale);
}

} // namespace Kinoko::Field
