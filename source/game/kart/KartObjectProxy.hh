#pragma once

#include <egg/math/Quat.hh>

#include <abstract/List.hh>

namespace Kart {

class KartBody;
class KartDynamics;
class KartMove;
class KartParam;
struct BSP;
class KartPhysics;
class KartState;
class KartSub;

struct KartAccessor {
    KartParam *m_param;
    KartBody *m_body;
    KartSub *m_sub;
    KartMove *m_move;
    KartState *m_state;
};

class KartObjectProxy {
    friend class KartObject;

public:
    KartObjectProxy();
    ~KartObjectProxy();

    KartBody *body();
    const KartBody *body() const;
    KartMove *move();
    const KartMove *move() const;
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

    const EGG::Vector3f &scale() const;

    void setPos(const EGG::Vector3f &pos);
    void setRot(const EGG::Quatf &q);

    Abstract::List *list() const;

protected:
    KartAccessor *m_accessor;

private:
    static void ApplyAll(KartAccessor *pointers);

    // Used to initialize multiple KartObjectProxy instances at once
    // Lists are created on the stack in KartObject::Create
    static Abstract::List *s_list;
};

} // namespace Kart
