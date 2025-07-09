#pragma once

#include "game/field/KCollisionTypes.hh"

#include "game/system/KPadController.hh"

#include <egg/math/Matrix.hh>

#include <list>
#include <vector>

namespace Field {

class BoxColUnit;
class ObjectCollisionKart;

} // namespace Field

namespace Render {

class KartModel;

} // namespace Render

namespace Kart {

class CollisionGroup;
struct CollisionData;
class KartAction;
class KartBody;
class KartCollide;
class KartDynamics;
class KartHalfPipe;
class KartJump;
class KartMove;
class KartParam;
struct BSP;
class KartPhysics;
class KartState;
class KartSub;
class KartSuspension;
class KartSuspensionPhysics;
class KartTire;
class WheelPhysics;

/// @brief Shared between classes who inherit KartObjectProxy so they can access one another.
struct KartAccessor {
    KartParam *param;
    KartBody *body;
    Render::KartModel *model;
    KartSub *sub;
    KartMove *move;
    KartAction *action;
    KartCollide *collide;
    Field::ObjectCollisionKart *objectCollisionKart;
    KartState *state;

    std::vector<KartSuspension *> suspensions;
    std::vector<KartTire *> tires;

    Field::BoxColUnit *boxColUnit;
};

/// @brief Base class for most kart-related objects.
/// @nosubgrouping
class KartObjectProxy {
    friend class KartObject;

public:
    KartObjectProxy();
    ~KartObjectProxy();

    /// @beginSetters
    void setPos(const EGG::Vector3f &pos);
    void setRot(const EGG::Quatf &q);
    void setInertiaScale(const EGG::Vector3f &scale);
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] KartAction *action();
    [[nodiscard]] const KartAction *action() const;
    [[nodiscard]] KartBody *body();
    [[nodiscard]] const KartBody *body() const;
    [[nodiscard]] KartCollide *collide();
    [[nodiscard]] const KartCollide *collide() const;
    [[nodiscard]] CollisionGroup *collisionGroup();
    [[nodiscard]] const CollisionGroup *collisionGroup() const;
    [[nodiscard]] KartMove *move();
    [[nodiscard]] const KartMove *move() const;
    [[nodiscard]] KartHalfPipe *halfPipe();
    [[nodiscard]] const KartHalfPipe *halfPipe() const;
    [[nodiscard]] KartJump *jump();
    [[nodiscard]] const KartJump *jump() const;
    [[nodiscard]] KartParam *param();
    [[nodiscard]] const KartParam *param() const;
    [[nodiscard]] const BSP &bsp() const;
    [[nodiscard]] KartPhysics *physics();
    [[nodiscard]] const KartPhysics *physics() const;
    [[nodiscard]] KartDynamics *dynamics();
    [[nodiscard]] const KartDynamics *dynamics() const;
    [[nodiscard]] KartState *state();
    [[nodiscard]] const KartState *state() const;
    [[nodiscard]] KartSub *sub();
    [[nodiscard]] const KartSub *sub() const;
    [[nodiscard]] KartSuspension *suspension(u16 suspIdx);
    [[nodiscard]] const KartSuspension *suspension(u16 suspIdx) const;
    [[nodiscard]] KartSuspensionPhysics *suspensionPhysics(u16 suspIdx);
    [[nodiscard]] const KartSuspensionPhysics *suspensionPhysics(u16 suspIdx) const;
    [[nodiscard]] KartTire *tire(u16 tireIdx);
    [[nodiscard]] const KartTire *tire(u16 tireIdx) const;
    [[nodiscard]] WheelPhysics *tirePhysics(u16 tireIdx);
    [[nodiscard]] const WheelPhysics *tirePhysics(u16 tireIdx) const;
    [[nodiscard]] CollisionData &collisionData();
    [[nodiscard]] const CollisionData &collisionData() const;
    [[nodiscard]] CollisionData &collisionData(u16 tireIdx);
    [[nodiscard]] const CollisionData &collisionData(u16 tireIdx) const;
    [[nodiscard]] const System::KPad *inputs() const;
    [[nodiscard]] Render::KartModel *model();
    [[nodiscard]] const Render::KartModel *model() const;
    [[nodiscard]] Field::ObjectCollisionKart *objectCollisionKart();
    [[nodiscard]] const Field::ObjectCollisionKart *objectCollisionKart() const;
    [[nodiscard]] Field::BoxColUnit *boxColUnit();
    [[nodiscard]] const Field::BoxColUnit *boxColUnit() const;

    [[nodiscard]] const EGG::Vector3f &scale() const;
    [[nodiscard]] const EGG::Matrix34f &pose() const;
    [[nodiscard]] EGG::Vector3f bodyFront() const;
    [[nodiscard]] EGG::Vector3f bodyForward() const;
    [[nodiscard]] EGG::Vector3f bodyUp() const;

    [[nodiscard]] const EGG::Vector3f &componentXAxis() const;
    [[nodiscard]] const EGG::Vector3f &componentYAxis() const;
    [[nodiscard]] const EGG::Vector3f &componentZAxis() const;

    [[nodiscard]] const EGG::Vector3f &pos() const;
    [[nodiscard]] const EGG::Vector3f &prevPos() const;
    [[nodiscard]] const EGG::Quatf &fullRot() const;
    [[nodiscard]] const EGG::Vector3f &extVel() const;
    [[nodiscard]] const EGG::Vector3f &intVel() const;
    [[nodiscard]] const EGG::Vector3f &velocity() const;
    [[nodiscard]] f32 speed() const;
    [[nodiscard]] f32 acceleration() const;
    [[nodiscard]] f32 softSpeedLimit() const;
    [[nodiscard]] const EGG::Quatf &mainRot() const;
    [[nodiscard]] const EGG::Vector3f &angVel2() const;
    [[nodiscard]] bool isBike() const;
    [[nodiscard]] u16 suspCount() const;
    [[nodiscard]] u16 tireCount() const;
    [[nodiscard]] bool hasFloorCollision(const WheelPhysics *wheelPhysics) const;
    [[nodiscard]] std::pair<EGG::Vector3f, EGG::Vector3f> getCannonPosRot();
    [[nodiscard]] f32 speedRatio() const;
    [[nodiscard]] f32 speedRatioCapped() const;
    [[nodiscard]] bool isInRespawn() const;
    [[nodiscard]] Field::KCLTypeMask wallKclType() const;
    [[nodiscard]] u32 wallKclVariant() const;

    [[nodiscard]] static std::list<KartObjectProxy *> &proxyList() {
        return s_proxyList;
    }
    /// @endGetters

protected:
    void apply(size_t idx);

private:
    static void ApplyAll(const KartAccessor *pointers);

    const KartAccessor *m_accessor;

    static std::list<KartObjectProxy *> s_proxyList; ///< List of all KartObjectProxy children.
};

} // namespace Kart
