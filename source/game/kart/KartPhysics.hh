#pragma once

#include "game/kart/KartDynamics.hh"
#include "game/kart/KartParam.hh"

#include <egg/math/Matrix.hh>

namespace Kart {

class KartPhysics {
public:
    KartPhysics(bool isBike);

    void reset();
    void updatePose();

    KartDynamics *getDynamics();
    const KartDynamics *getDynamics() const;
    const EGG::Matrix34f &getPose() const;

    static KartPhysics *Create(const KartParam &param);

private:
    KartDynamics *m_dynamics;
    EGG::Matrix34f m_pose;
    EGG::Vector3f m_xAxis;
    EGG::Vector3f m_yAxis;
    EGG::Vector3f m_zAxis;
};

} // namespace Kart
