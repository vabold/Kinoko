#include "ObjectDossunc.hh"

#include "game/field/obj/ObjectDossunNormal.hh"

namespace Field {

/// @addr{0x8075EAFC}
ObjectDossunc::ObjectDossunc(const System::MapdataGeoObj &params) : ObjectCollidable(params) {
    switch (params.setting(1)) {
    case 0: {
        auto *dossunNormal = new ObjectDossunNormal(params);
        dossunNormal->load();
    } break;
    default:
        break;
    }
}

/// @addr{0x80764B08}
ObjectDossunc::~ObjectDossunc() = default;

} // namespace Field
