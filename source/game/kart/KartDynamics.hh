#pragma once

#include <egg/math/Matrix.hh>

namespace Kart {

class KartDynamics {
public:
    KartDynamics();
    void init();

    const EGG::Vector3f &getPos() const;
    const EGG::Vector3f &getSpeed() const;
    const EGG::Vector3f &getTop() const;
    const EGG::Quatf &getFullRot() const;

    void setPos(const EGG::Vector3f &pos);
    void setTop(const EGG::Vector3f &top);
    void setMainRot(const EGG::Quatf &q);
    void setFullRot(const EGG::Quatf &q);

private:
    // These are the only vars needed in setInitialPhysicsValues
    EGG::Vector3f m_pos;
    EGG::Vector3f m_speed;
    EGG::Vector3f m_top;
    EGG::Quatf m_mainRot;
    EGG::Quatf m_fullRot;
};

class KartDynamicsBike : public KartDynamics {};

} // namespace Kart
