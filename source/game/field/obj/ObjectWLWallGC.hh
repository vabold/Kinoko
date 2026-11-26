#pragma once

#include "game/field/obj/ObjectKCL.hh"

namespace Kinoko::Field {

/// @brief The horizontally moving Piranha Plant in GCN Waluigi Stadium
/// @details The Piranha Plant's motion is linear. The object's param settings define the
/// duration the Piranha Plant should remain inside the pipe, the duration it should remain extended
/// outside of the pipe, and the distance it should travel when extending.
/// @note This object is separate and distinct from the pipe, which is a primitive @ref ObjectKCL
/// object.
/// @note If param setting 3 is zero, then the game implements a fail-safe where the Piranha Plant
/// will always remain extended.
class ObjectWLWallGC final : public ObjectKCL {
public:
    ObjectWLWallGC(const System::MapdataGeoObj &params);
    ~ObjectWLWallGC() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

    [[nodiscard]] const EGG::Matrix34f &getUpdatedMatrix(u32 timeOffset) override;
    [[nodiscard]] f32 colRadiusAdditionalLength() const override;
    [[nodiscard]] bool checkCollision(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;

private:
    [[nodiscard]] EGG::Vector3f initExtendedPos(const System::MapdataGeoObj &params);
    [[nodiscard]] u32 cycleFrame(s32 t) const;

    const u32 m_rate; ///< Speed of movement, cached in Kinoko so dependent members can be const
    const s32 m_extendedDuration;     ///< Duration the Piranha remains extended
    const s32 m_moveDuration;         ///< Duration the Piranha takes to move in or out of the pipe
    const s32 m_startFrame;           ///< The frame at which the Piranha's movement cycle starts
    const s32 m_hiddenDuration;       ///< Duration the Piranha should remain hidden inside the pipe
    const s32 m_extendedFrame;        ///< Frame at which the Piranha Plant is fully extended
    const s32 m_retractingFrame;      ///< Frame at which the Piranha Plant starts retracting
    const s32 m_cycleDuration;        ///< Total duration of the Piranha Plant's movement cycle
    const EGG::Vector3f m_initialPos; ///< Initial position of the Piranha Plant
    const EGG::Vector3f m_extendedPos; ///< Position of the Piranha Plant when it is fully extended
    EGG::Matrix34f m_rtMat;            ///< Current rotation/translation matrix
};

} // namespace Kinoko::Field
