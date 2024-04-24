#include "KartObjectProxy.hh"

#include "game/kart/CollisionGroup.hh"
#include "game/kart/KartObject.hh"
#include "game/kart/KartObjectManager.hh"
#include "game/kart/KartSub.hh"
#include "game/kart/KartSuspension.hh"
#include "game/kart/KartTire.hh"

#include "game/system/RaceManager.hh"

namespace Kart {

KartObjectProxy::KartObjectProxy() : m_accessor(nullptr) {
    if (s_list) {
        s_list->append(this);
    }
}

KartObjectProxy::~KartObjectProxy() = default;

KartBody *KartObjectProxy::body() {
    return m_accessor->body;
}

const KartBody *KartObjectProxy::body() const {
    return m_accessor->body;
}

KartCollide *KartObjectProxy::collide() {
    return m_accessor->collide;
}

const KartCollide *KartObjectProxy::collide() const {
    return m_accessor->collide;
}

CollisionGroup *KartObjectProxy::collisionGroup() {
    return m_accessor->body->physics()->hitboxGroup();
}

const CollisionGroup *KartObjectProxy::collisionGroup() const {
    return m_accessor->body->physics()->hitboxGroup();
}

KartMove *KartObjectProxy::move() {
    return m_accessor->move;
}

const KartMove *KartObjectProxy::move() const {
    return m_accessor->move;
}

KartJump *KartObjectProxy::jump() {
    return m_accessor->move->jump();
}

const KartJump *KartObjectProxy::jump() const {
    return m_accessor->move->jump();
}

KartParam *KartObjectProxy::param() {
    return m_accessor->param;
}

const KartParam *KartObjectProxy::param() const {
    return m_accessor->param;
}

const BSP &KartObjectProxy::bsp() const {
    return param()->bsp();
}

KartPhysics *KartObjectProxy::physics() {
    return body()->physics();
}

const KartPhysics *KartObjectProxy::physics() const {
    return body()->physics();
}

KartDynamics *KartObjectProxy::dynamics() {
    return physics()->dynamics();
}

const KartDynamics *KartObjectProxy::dynamics() const {
    return physics()->dynamics();
}

KartState *KartObjectProxy::state() {
    return m_accessor->state;
}

const KartState *KartObjectProxy::state() const {
    return m_accessor->state;
}

KartSub *KartObjectProxy::sub() {
    return m_accessor->sub;
}

const KartSub *KartObjectProxy::sub() const {
    return m_accessor->sub;
}

KartSuspension *KartObjectProxy::suspension(u16 suspIdx) {
    return m_accessor->suspensions[suspIdx];
}

const KartSuspension *KartObjectProxy::suspension(u16 suspIdx) const {
    return m_accessor->suspensions[suspIdx];
}

KartSuspensionPhysics *KartObjectProxy::suspensionPhysics(u16 suspIdx) {
    return m_accessor->suspensions[suspIdx]->suspPhysics();
}

const KartSuspensionPhysics *KartObjectProxy::suspensionPhysics(u16 suspIdx) const {
    return m_accessor->suspensions[suspIdx]->suspPhysics();
}

KartTire *KartObjectProxy::tire(u16 tireIdx) {
    return m_accessor->tires[tireIdx];
}

const KartTire *KartObjectProxy::tire(u16 tireIdx) const {
    return m_accessor->tires[tireIdx];
}

WheelPhysics *KartObjectProxy::tirePhysics(u16 tireIdx) {
    return tire(tireIdx)->wheelPhysics();
}

const WheelPhysics *KartObjectProxy::tirePhysics(u16 tireIdx) const {
    return m_accessor->tires[tireIdx]->wheelPhysics();
}

CollisionData &KartObjectProxy::collisionData() {
    return physics()->hitboxGroup()->collisionData();
}

const CollisionData &KartObjectProxy::collisionData() const {
    return m_accessor->body->physics()->hitboxGroup()->collisionData();
}

const System::KPad *KartObjectProxy::inputs() const {
    return System::RaceManager::Instance()->player().inputs();
}

Render::KartModel *KartObjectProxy::model() {
    return m_accessor->model;
}

const Render::KartModel *KartObjectProxy::model() const {
    return m_accessor->model;
}

CollisionData &KartObjectProxy::collisionData(u16 tireIdx) {
    return tirePhysics(tireIdx)->hitboxGroup()->collisionData();
}

const CollisionData &KartObjectProxy::collisionData(u16 tireIdx) const {
    return m_accessor->tires[tireIdx]->wheelPhysics()->hitboxGroup()->collisionData();
}

const EGG::Vector3f &KartObjectProxy::scale() const {
    return move()->scale();
}

const EGG::Matrix34f &KartObjectProxy::pose() const {
    return physics()->pose();
}

EGG::Vector3f KartObjectProxy::bodyFront() const {
    const EGG::Matrix34f &mtx = pose();
    return EGG::Vector3f(mtx[0, 2], mtx[1, 2], mtx[2, 2]);
}

EGG::Vector3f KartObjectProxy::bodyForward() const {
    const EGG::Matrix34f &mtx = pose();
    return EGG::Vector3f(mtx[0, 0], mtx[1, 0], mtx[2, 0]);
}

const EGG::Vector3f &KartObjectProxy::componentXAxis() const {
    return physics()->xAxis();
}

const EGG::Vector3f &KartObjectProxy::componentYAxis() const {
    return physics()->yAxis();
}

const EGG::Vector3f &KartObjectProxy::componentZAxis() const {
    return physics()->zAxis();
}

void KartObjectProxy::setPos(const EGG::Vector3f &pos) {
    dynamics()->setPos(pos);
}

void KartObjectProxy::setRot(const EGG::Quatf &q) {
    dynamics()->setFullRot(q);
    dynamics()->setMainRot(q);
}

const EGG::Vector3f &KartObjectProxy::pos() const {
    return dynamics()->pos();
}

const EGG::Quatf &KartObjectProxy::fullRot() const {
    return dynamics()->fullRot();
}

const EGG::Vector3f &KartObjectProxy::extVel() const {
    return dynamics()->extVel();
}

const EGG::Vector3f &KartObjectProxy::intVel() const {
    return dynamics()->intVel();
}

f32 KartObjectProxy::speed() const {
    return move()->speed();
}

f32 KartObjectProxy::acceleration() const {
    return move()->acceleration();
}

f32 KartObjectProxy::softSpeedLimit() const {
    return move()->softSpeedLimit();
}

const EGG::Quatf &KartObjectProxy::mainRot() const {
    return dynamics()->mainRot();
}

const EGG::Vector3f &KartObjectProxy::angVel2() const {
    return dynamics()->angVel2();
}

bool KartObjectProxy::isBike() const {
    return param()->isBike();
}

u16 KartObjectProxy::suspCount() const {
    return param()->suspCount();
}

u16 KartObjectProxy::tireCount() const {
    return param()->tireCount();
}

bool KartObjectProxy::hasFloorCollision(const WheelPhysics *wheelPhysics) const {
    return wheelPhysics->hitboxGroup()->collisionData().bFloor;
}

Abstract::List *KartObjectProxy::list() const {
    return s_list;
}

void KartObjectProxy::apply(size_t idx) {
    m_accessor = KartObjectManager::Instance()->object(idx)->accessor();
}

void KartObjectProxy::ApplyAll(const KartAccessor *pointers) {
    for (Abstract::Node *node = s_list->head(); node; node = s_list->getNext(node)) {
        node->data<KartObjectProxy>()->m_accessor = pointers;
    }
}

Abstract::List *KartObjectProxy::s_list = nullptr;

} // namespace Kart
