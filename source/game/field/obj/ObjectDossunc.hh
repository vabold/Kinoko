#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief Holder class for one of various Thwomp "modes".
class ObjectDossunc final : public ObjectCollidable {
public:
    ObjectDossunc(const System::MapdataGeoObj &params);
    ~ObjectDossunc() override;

    /// @addr{0x80764A38}
    void load() override;
};

} // namespace Field
