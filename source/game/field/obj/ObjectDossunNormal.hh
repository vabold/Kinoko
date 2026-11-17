#pragma once

#include "game/field/obj/ObjectDossun.hh"

namespace Kinoko::Field {

/// @brief A Thwomp that remains in one position and stomps downward, like on GBA Bowser Castle 3
class ObjectDossunNormal final : public ObjectDossun {
public:
    ObjectDossunNormal(const System::MapdataGeoObj &params);
    ~ObjectDossunNormal() override;

    void init() override;
    void calc() override;

private:
    void startStill() override;

    void calcInactive();
    void startBeforeFall();

    /// @brief Number of frames the Thwomp remains still for
    /// @details This member does not exist in the base game, but we cache it in Kinoko to avoid
    /// repeatedly fetching the param settings.
    const s32 m_stillDuration;
};

} // namespace Kinoko::Field
