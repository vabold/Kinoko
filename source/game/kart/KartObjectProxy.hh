#pragma once

#include "game/system/KPadController.hh"

#include <egg/math/Matrix.hh>

#include <abstract/List.hh>

#include <vector>

namespace Render {

class KartModel;

} // namespace Render

namespace Kart {

class CollisionGroup;
struct CollisionData;
class KartBody;
class KartCollide;
class KartDynamics;
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

struct KartAccessor {
    KartParam *param;
    KartBody *body;
    Render::KartModel *model;
    KartSub *sub;
    KartMove *move;
    KartCollide *collide;
    KartState *state;

    std::vector<KartSuspension *> suspensions;
    std::vector<KartTire *> tires;
};

class KartObjectProxy {
    friend class KartObject;

public:
    KartObjectProxy();
    ~KartObjectProxy();

    KartBody *body();
    const KartBody *body() const;
    KartCollide *collide();
    const KartCollide *collide() const;
    CollisionGroup *collisionGroup();
    const CollisionGroup *collisionGroup() const;
    KartMove *move();
    const KartMove *move() const;
    KartJump *jump();
    const KartJump *jump() const;
    KartParam *param();
    const KartParam *param() const;
    const BSP &bsp() const;
    KartPhysics *physics();
    const KartPhysics *physics() const;
    KartDynamics *dynamics();
    const KartDynamics *dynamics() const;
    KartState *state();
    const KartState *state() const;
    KartSub *sub();
    const KartSub *sub() const;
    KartSuspension *suspension(u16 suspIdx);
    const KartSuspension *suspension(u16 suspIdx) const;
    KartSuspensionPhysics *suspensionPhysics(u16 suspIdx);
    const KartSuspensionPhysics *suspensionPhysics(u16 suspIdx) const;
    KartTire *tire(u16 tireIdx);
    const KartTire *tire(u16 tireIdx) const;
    WheelPhysics *tirePhysics(u16 tireIdx);
    const WheelPhysics *tirePhysics(u16 tireIdx) const;
    CollisionData &collisionData();
    const CollisionData &collisionData() const;
    CollisionData &collisionData(u16 tireIdx);
    const CollisionData &collisionData(u16 tireIdx) const;
    const System::KPad *inputs() const;
    Render::KartModel *model();
    const Render::KartModel *model() const;

    const EGG::Vector3f &scale() const;
    const EGG::Matrix34f &pose() const;
    EGG::Vector3f bodyFront() const;
    EGG::Vector3f bodyForward() const;

    const EGG::Vector3f &componentXAxis() const;
    const EGG::Vector3f &componentYAxis() const;
    const EGG::Vector3f &componentZAxis() const;

    void setPos(const EGG::Vector3f &pos);
    void setRot(const EGG::Quatf &q);

    const EGG::Vector3f &pos() const;
    const EGG::Quatf &fullRot() const;
    const EGG::Vector3f &extVel() const;
    const EGG::Vector3f &intVel() const;
    f32 speed() const;
    f32 acceleration() const;
    f32 softSpeedLimit() const;
    const EGG::Quatf &mainRot() const;
    const EGG::Vector3f &angVel2() const;
    bool isBike() const;
    u16 suspCount() const;
    u16 tireCount() const;
    bool hasFloorCollision(const WheelPhysics *wheelPhysics) const;

    Abstract::List *list() const;

protected:
    void apply(size_t idx);

    const KartAccessor *m_accessor;

private:
    static void ApplyAll(const KartAccessor *pointers);

    // Used to initialize multiple KartObjectProxy instances at once
    // Lists are created on the stack in KartObject::Create
    static Abstract::List *s_list;
};

} // namespace Kart
