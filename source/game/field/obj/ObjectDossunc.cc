#include "ObjectDossunc.hh"

#include "game/field/obj/ObjectDossunNormal.hh"
#include "game/field/obj/ObjectDossunSyuukai.hh"
#include "game/field/obj/ObjectDossunTsuibiHolder.hh"

namespace Field {

/// @addr{0x8075EAFC}
ObjectDossunc::ObjectDossunc(const System::MapdataGeoObj &params) : ObjectCollidable(params) {
    switch (params.setting(1)) {
    case 0: {
        auto *dossunNormal = new ObjectDossunNormal(params);
        dossunNormal->load();
    } break;
    case 1: {
        auto *dossunSyuukai = new ObjectDossunSyuukai(params);
        dossunSyuukai->load();
    } break;
    case 2: {
        auto *dossunTsuibi = new ObjectDossunTsuibiHolder(params);
        dossunTsuibi->load();
    } break;
    default:
        break;
    }
}

/// @addr{0x80764B08}
ObjectDossunc::~ObjectDossunc() = default;

} // namespace Field
