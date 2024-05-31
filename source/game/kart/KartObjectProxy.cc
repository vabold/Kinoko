#include "KartObjectProxy.hh"

#include "game/kart/CollisionGroup.hh"
#include "game/kart/KartObject.hh"
#include "game/kart/KartObjectManager.hh"
#include "game/kart/KartSub.hh"
#include "game/kart/KartSuspension.hh"
#include "game/kart/KartTire.hh"

#include "game/system/RaceManager.hh"

#include <algorithm>

namespace Kart {

/// @addr{0x8059018C}
KartObjectProxy::KartObjectProxy() : m_accessor(nullptr) {
    s_proxyList.push_back(this);
}

KartObjectProxy::~KartObjectProxy() = default;

/// @addr{0x80590238}
void KartObjectProxy::setPos(const EGG::Vector3f &pos) {
    dynamics()->setPos(pos);
}

/// @addr{0x80590288}
void KartObjectProxy::setRot(const EGG::Quatf &q) {
    dynamics()->setFullRot(q);
    dynamics()->setMainRot(q);
}

/// @addr{0x8059069C}
KartBody *KartObjectProxy::body() {
    return m_accessor->body;
}

/// @addr{0x8059069C}
const KartBody *KartObjectProxy::body() const {
    return m_accessor->body;
}

/// @addr{0x8059084C}
KartCollide *KartObjectProxy::collide() {
    return m_accessor->collide;
}

/// @addr{0x8059084C}
const KartCollide *KartObjectProxy::collide() const {
    return m_accessor->collide;
}

/// @addr{0x805907D8}
CollisionGroup *KartObjectProxy::collisionGroup() {
    return m_accessor->body->physics()->hitboxGroup();
}

/// @addr{0x805907D8}
const CollisionGroup *KartObjectProxy::collisionGroup() const {
    return m_accessor->body->physics()->hitboxGroup();
}

/// @addr{0x8059077C}
KartMove *KartObjectProxy::move() {
    return m_accessor->move;
}

/// @addr{0x8059077C}
const KartMove *KartObjectProxy::move() const {
    return m_accessor->move;
}

/// @addr{0x80591914}
KartJump *KartObjectProxy::jump() {
    return m_accessor->move->jump();
}

/// @addr{0x80591914}
const KartJump *KartObjectProxy::jump() const {
    return m_accessor->move->jump();
}

/// @addr{0x80590864}
KartParam *KartObjectProxy::param() {
    return m_accessor->param;
}

/// @addr{0x80590864}
const KartParam *KartObjectProxy::param() const {
    return m_accessor->param;
}

/// @addr{0x80590888}
const BSP &KartObjectProxy::bsp() const {
    return param()->bsp();
}

/// @addr{0x805903AC}
KartPhysics *KartObjectProxy::physics() {
    return body()->physics();
}

/// @addr{0x805903AC}
const KartPhysics *KartObjectProxy::physics() const {
    return body()->physics();
}

/// @addr{0x805903E0}
KartDynamics *KartObjectProxy::dynamics() {
    return physics()->dynamics();
}

/// @addr{0x805903E0}
const KartDynamics *KartObjectProxy::dynamics() const {
    return physics()->dynamics();
}

KartState *KartObjectProxy::state() {
    return m_accessor->state;
}

const KartState *KartObjectProxy::state() const {
    return m_accessor->state;
}

/// @addr{0x80590764}
KartSub *KartObjectProxy::sub() {
    return m_accessor->sub;
}

/// @addr{0x80590764}
const KartSub *KartObjectProxy::sub() const {
    return m_accessor->sub;
}

/// @addr{0x805906B4}
KartSuspension *KartObjectProxy::suspension(u16 suspIdx) {
    return m_accessor->suspensions[suspIdx];
}

/// @addr{0x805906B4}
const KartSuspension *KartObjectProxy::suspension(u16 suspIdx) const {
    return m_accessor->suspensions[suspIdx];
}

/// @addr{0x80590704}
KartSuspensionPhysics *KartObjectProxy::suspensionPhysics(u16 suspIdx) {
    return m_accessor->suspensions[suspIdx]->suspPhysics();
}

/// @addr{0x80590704}
const KartSuspensionPhysics *KartObjectProxy::suspensionPhysics(u16 suspIdx) const {
    return m_accessor->suspensions[suspIdx]->suspPhysics();
}

/// @addr{0x805906DC}
KartTire *KartObjectProxy::tire(u16 tireIdx) {
    return m_accessor->tires[tireIdx];
}

/// @addr{0x805906DC}
const KartTire *KartObjectProxy::tire(u16 tireIdx) const {
    return m_accessor->tires[tireIdx];
}

/// @addr{0x80590734}
WheelPhysics *KartObjectProxy::tirePhysics(u16 tireIdx) {
    return tire(tireIdx)->wheelPhysics();
}

/// @addr{0x80590734}
const WheelPhysics *KartObjectProxy::tirePhysics(u16 tireIdx) const {
    return m_accessor->tires[tireIdx]->wheelPhysics();
}

/// @addr{0x8059081C}
CollisionData &KartObjectProxy::collisionData() {
    return physics()->hitboxGroup()->collisionData();
}

/// @addr{0x8059081C}
const CollisionData &KartObjectProxy::collisionData() const {
    return m_accessor->body->physics()->hitboxGroup()->collisionData();
}

/// @addr{0x805903F4}
const System::KPad *KartObjectProxy::inputs() const {
    return System::RaceManager::Instance()->player().inputs();
}

/// @addr{0x80590A40}
Render::KartModel *KartObjectProxy::model() {
    return m_accessor->model;
}

/// @addr{0x80590A40}
const Render::KartModel *KartObjectProxy::model() const {
    return m_accessor->model;
}

/// @addr{0x80590834}
CollisionData &KartObjectProxy::collisionData(u16 tireIdx) {
    return tirePhysics(tireIdx)->hitboxGroup()->collisionData();
}

/// @addr{0x80590834}
const CollisionData &KartObjectProxy::collisionData(u16 tireIdx) const {
    return m_accessor->tires[tireIdx]->wheelPhysics()->hitboxGroup()->collisionData();
}

/// @addr{0x805914BC}
const EGG::Vector3f &KartObjectProxy::scale() const {
    return move()->scale();
}

/// @addr{0x80590264}
const EGG::Matrix34f &KartObjectProxy::pose() const {
    return physics()->pose();
}

/// @brief Returns the third column of the rotation matrix, which is the facing vector.
/// @addr{0x80590C94}
EGG::Vector3f KartObjectProxy::bodyFront() const {
    const EGG::Matrix34f &mtx = pose();
    return EGG::Vector3f(mtx[0, 2], mtx[1, 2], mtx[2, 2]);
}

/// @brief Returns the first column of the rotation matrix, which is the "right" direction.
/// @addr{0x80590C44}
EGG::Vector3f KartObjectProxy::bodyForward() const {
    const EGG::Matrix34f &mtx = pose();
    return EGG::Vector3f(mtx[0, 0], mtx[1, 0], mtx[2, 0]);
}

/// @addr{0x80590C6C}
/// @brief Returns the second column of the rotation matrix, which is the "up" direction.
EGG::Vector3f KartObjectProxy::bodyUp() const {
    const EGG::Matrix34f &mtx = pose();
    return EGG::Vector3f(mtx[0, 1], mtx[1, 1], mtx[2, 1]);
}

/// @addr{0x80590CBC}
const EGG::Vector3f &KartObjectProxy::componentXAxis() const {
    return physics()->xAxis();
}

/// @addr{0x80590CD0}
const EGG::Vector3f &KartObjectProxy::componentYAxis() const {
    return physics()->yAxis();
}

/// @addr{0x80590CE4}
const EGG::Vector3f &KartObjectProxy::componentZAxis() const {
    return physics()->zAxis();
}

/// @addr{0x8059020C}
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

const EGG::Vector3f &KartObjectProxy::velocity() const {
    return dynamics()->velocity();
}

/// @addr{0x80590CF8}
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

/// @addr{0x80590A6C}
bool KartObjectProxy::isBike() const {
    return param()->isBike();
}

/// @addr{0x805902DC}
u16 KartObjectProxy::suspCount() const {
    return param()->suspCount();
}

/// @addr{0x805902EC}
u16 KartObjectProxy::tireCount() const {
    return param()->tireCount();
}

/// @addr{0x80590338}
bool KartObjectProxy::hasFloorCollision(const WheelPhysics *wheelPhysics) const {
    return wheelPhysics->hitboxGroup()->collisionData().bFloor;
}

/// @addr{0x805901D0}
void KartObjectProxy::apply(size_t idx) {
    m_accessor = KartObjectManager::Instance()->object(idx)->accessor();
}

/// @addr{0x80590138}
/// @brief For all proxies in the static list, synchronizes all pointers to the KartAccessor.
/// @param pointers The pointer to synchronize all other proxies to.
void KartObjectProxy::ApplyAll(const KartAccessor *pointers) {
    for (auto iter = s_proxyList.begin(); iter != s_proxyList.end(); ++iter) {
        (*iter)->m_accessor = pointers;
    }
}

std::list<KartObjectProxy *> KartObjectProxy::s_proxyList; ///< @addr{0x809C1900}

} // namespace Kart
