#pragma once

#include "game/field/obj/ObjectKCL.hh"

namespace Kinoko::Field {

/// @brief Represents the sandcones that grow from the falling sand in the Dry Dry Ruins temple
/// @details The sandcones don't actually grow, rather their position gradually rises from beneath
/// the floor. The cone stops rising once it reaches the final height delta defined by param
/// setting 2. The growth speed is defined by param setting 1, and the start delay before growing is
/// defined by param setting 3.
class ObjectSandcone final : public ObjectKCL {
public:
    ObjectSandcone(const System::MapdataGeoObj &params);
    ~ObjectSandcone() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

    [[nodiscard]] const EGG::Matrix34f &getUpdatedMatrix(u32 timeOffset) override;
    [[nodiscard]] bool checkCollision(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;

private:
    [[nodiscard]] EGG::Matrix34f initMat() const;

    const f32 m_flowRate;         ///< Controls how fast the sandcone rises in units per frame
    const f32 m_finalHeightDelta; ///< The final height the sandcone gains
    const u16 m_startFrame;       ///< Race frame the sandcone starts rising
    const u16 m_duration;         ///< Total frames it takes the sandcone to reach its final height
    const EGG::Matrix34f m_initMat; ///< Initial rotation/translation matrix of the sandcone
    EGG::Matrix34f m_currentMtx;    ///< Up-to-date collision transformation matrix of the sandcone

    const EGG::Vector3f m_finalPos; ///< Not in base game. Stores position of fully poured sandcone.
};

} // namespace Kinoko::Field
