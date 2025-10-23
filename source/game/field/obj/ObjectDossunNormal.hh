#pragma once

#include "game/field/obj/ObjectDossun.hh"

namespace Field {

class ObjectDossunNormal : public ObjectDossun {
public:
    ObjectDossunNormal(const System::MapdataGeoObj &params);
    ~ObjectDossunNormal() override;

    void init() override;
    void calc() override;

    void startStill() override;

    virtual void startBeforeFall();
};

} // namespace Field
