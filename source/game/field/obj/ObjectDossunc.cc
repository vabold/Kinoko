#include "ObjectDossunc.hh"

#include "game/field/obj/ObjectDossunNormal.hh"
#include "game/field/obj/ObjectDossunSyuukai.hh"
#include "game/field/obj/ObjectDossunTsuibiHolder.hh"
#include "game/field/obj/ObjectDossunYokoMove.hh"

#include "game/field/ObjectDirector.hh"

namespace Kinoko::Field {

/// @addr{0x8075EAFC}
ObjectDossunc::ObjectDossunc(const System::MapdataGeoObj &params) : ObjectCollidable(params) {
    switch (params.setting(1)) {
    case 0: {
        auto *dossunNormal = EGG::egg_new<ObjectDossunNormal>(params);
        dossunNormal->load();
    } break;
    case 1: {
        auto *dossunSyuukai = EGG::egg_new<ObjectDossunSyuukai>(params);
        dossunSyuukai->load();
    } break;
    case 2: {
        auto *dossunTsuibi = EGG::egg_new<ObjectDossunTsuibiHolder>(params);
        dossunTsuibi->load();
    } break;
    case 3: {
        auto *dossunYokoMove = EGG::egg_new<ObjectDossunYokoMove>(params);
        dossunYokoMove->load();
    } break;
    default:
        break;
    }
}

/// @addr{0x80764B08}
ObjectDossunc::~ObjectDossunc() = default;

/// @addr{0x80764A38}
void ObjectDossunc::load() {
    ObjectDirector::Instance()->addObjectNoImpl(this);
}

} // namespace Kinoko::Field
