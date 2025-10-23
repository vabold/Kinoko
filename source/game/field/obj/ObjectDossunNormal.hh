#pragma once

#include "game/field/obj/ObjectDossun.hh"

namespace Field {

/// @brief A Thwomp that remains in one position and stomps downward.
class ObjectDossunNormal final : public ObjectDossun {
public:
    ObjectDossunNormal(const System::MapdataGeoObj &params);
    ~ObjectDossunNormal() override;

    void init() override;
    void calc() override;

    void startStill() override;

    void startBeforeFall();

private:
    void calcInactive();
};

} // namespace Field
