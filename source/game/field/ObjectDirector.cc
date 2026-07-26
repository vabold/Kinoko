#include "ObjectDirector.hh"

#include "game/field/BoxColManager.hh"
#include "game/field/ObjectDrivableDirector.hh"
#include "game/field/obj/ObjectRegistry.hh"

#include "game/kart/KartObject.hh"

#include "game/system/CourseMap.hh"
#include "game/system/RaceConfig.hh"

namespace Kinoko::Field {

/// @addr{0x8082A2B4}
void ObjectDirector::init() {
    for (auto *&obj : m_objects) {
        obj->init();
        obj->calcModel();
    }

    ObjectDrivableDirector::Instance()->init();
}

/// @addr{0x8082A8F4}
void ObjectDirector::calc() {
    for (auto *&obj : m_calcObjects) {
        obj->calc();
    }

    for (auto *&obj : m_calcObjects) {
        obj->calcModel();
    }

    ObjectDrivableDirector::Instance()->calc();
}

/// @addr{0x8082B0E8}
void ObjectDirector::addObject(ObjectCollidable *obj) {
    u32 loadFlags = obj->loadFlags();

    if (loadFlags & 1) {
        m_calcObjects.push_back(obj);
    }

    const auto *set = m_flowTable.set(m_flowTable.slot(obj->id()));

    // In the base game, it's possible an object here will access slot -1 (e.g. Moonview Highway
    // cars). We add a nullptr check here to prevent this.
    if (set && set->mode != 0) {
        if (obj->collision()) {
            m_collisionObjects.push_back(obj);
        }
    }

    m_objects.push_back(obj);
}

void ObjectDirector::addObjectNoImpl(ObjectBase *obj) {
    m_objects.push_back(obj);
}

/// @addr{0x806C4ED4}
void ObjectDirector::addManagedObject(ObjectCollidable *obj) {
    m_managedObjects.push_back(obj);
}

/// @addr{0x8082AB04}
size_t ObjectDirector::checkKartObjectCollision(Kart::KartObject *kartObj,
        ObjectCollisionConvexHull *convexHull) {
    size_t count = 0;

    while (ObjectCollidable *obj = BoxColManager::Instance()->getNextObject()) {
        auto *objCollision = obj->collision();
        if (!objCollision) {
            continue;
        }

        obj->calcCollisionTransform();
        if (!obj->checkCollision(convexHull, m_hitDepths[count])) {
            continue;
        }

        // We have a collision, process it
        // Assume that we are not in a star, mega, or bullet
        Kart::Reaction reactionOnKart = m_hitTableKart.reaction(m_hitTableKart.slot(obj->id()));
        Kart::Reaction reactionOnObj =
                m_hitTableKartObject.reaction(m_hitTableKartObject.slot(obj->id()));

        // The object might change the reaction states
        obj->processKartReactions(kartObj, reactionOnKart, reactionOnObj);

        Kart::Reaction reaction =
                obj->onCollision(kartObj, reactionOnKart, reactionOnObj, m_hitDepths[count]);
        m_reactions[count] = reaction;

        if (reaction == Kart::Reaction::WallAllSpeed || reaction == Kart::Reaction::WallSpark) {
            obj->onWallCollision(kartObj, m_hitDepths[count]);
        } else {
            obj->onObjectCollision(kartObj);
        }

        m_collidingObjects[count] = obj;
        if (m_hitDepths[count].y < 0.0f) {
            m_hitDepths[count].y = 0.0f;
        }

        ++count;
    }

    return count;
}

/// @addr{0x8082B3EC}
f32 ObjectDirector::distAboveRisingWater(f32 offset) const {
    ASSERT(m_psea);
    return offset - m_psea->pos().y;
}

/// @addr{0x8082B400}
f32 ObjectDirector::risingWaterKillPlaneHeight() const {
    ASSERT(m_psea);
    return m_psea->pos().y - 260.0f;
}

/// @addr{0x8082A784}
ObjectDirector *ObjectDirector::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = EGG::egg_new<ObjectDirector>();

    ObjectDrivableDirector::CreateInstance();

    s_instance->createObjects();

    return s_instance;
}

/// @addr{0x8082A824}
void ObjectDirector::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    EGG::egg_delete(instance);

    ObjectDrivableDirector::DestroyInstance();
}

/// @addr{0x8082A38C}
ObjectDirector::ObjectDirector()
    : m_flowTable("ObjFlow.bin"), m_hitTableKart("GeoHitTableKart.bin"),
      m_hitTableKartObject("GeoHitTableKartObj.bin"), m_psea(nullptr) {}

/// @addr{0x8082A694}
ObjectDirector::~ObjectDirector() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("ObjectDirector instance not explicitly handled!");
    }

    for (auto *&obj : m_objects) {
        EGG::egg_delete(obj);
    }
}

/// @addr{0x80826E8C}
void ObjectDirector::createObjects() {
    const auto *courseMap = System::CourseMap::Instance();
    size_t objectCount = courseMap->getGeoObjCount();

    // It's possible for the KMP to specify settings for objects that aren't tracked here
    // MAX_UNIT_COUNT is the upper bound for tracked object count, so we reserve the minimum
    size_t maxCount = std::min(objectCount, MAX_UNIT_COUNT);
    m_objects.reserve(maxCount);
    m_calcObjects.reserve(maxCount);
    m_collisionObjects.reserve(maxCount);

    auto *objDrivableDir = ObjectDrivableDirector::Instance();
    auto course = System::RaceConfig::Instance()->raceScenario().course;
    bool rGV2 = course == Course::SNES_Ghost_Valley_2;
    bool sun = false;

    // The max pitch of the Chain Chomp only needs to be set once,
    // so we set it here instead of in the Chain Chomp constructor.
    s_wanwanMaxPitch = course == Course::GCN_Mario_Circuit ? -20.0f : -30.0f;

    for (size_t i = 0; i < objectCount; ++i) {
        const auto *pObj = courseMap->getGeoObj(i);
        ASSERT(pObj);

        // Assume one player - if the presence flag isn't set, don't construct it
        if (!(pObj->presenceFlag() & 1)) {
            continue;
        }

        // Prevent construction of objects with disabled or no collision
        if (IsObjectBlacklisted(pObj->id())) {
            continue;
        }

        // rGV2's blocks are created outside of the factory function
        if (rGV2) {
            switch (static_cast<ObjectId>(pObj->id())) {
            case ObjectId::ObakeBlockSFCc:
            case ObjectId::ObakeBlock2SFCc:
            case ObjectId::ObakeBlock3SFCc: {
                // Create the manager if this is the first block.
                auto *obakeManager = objDrivableDir->obakeManager();
                if (!obakeManager) {
                    objDrivableDir->createObakeManager(*pObj);
                } else {
                    obakeManager->addBlock(*pObj);
                }
            } break;
            default:
                break;
            }
        }

        ObjectBase *object = createObject(*pObj);
        object->load();

        if (object->id() == ObjectId::SunDS) {
            sun = true;
        }
    }

    if (course == Course::Moonview_Highway) {
        auto *highwayMgr = EGG::egg_new<ObjectHighwayManager>();
        highwayMgr->load();
    }

    if (sun) {
        auto *sunMgr = EGG::egg_new<ObjectSunManager>();
        sunMgr->load();
    }

    if (course == Course::GBA_Shy_Guy_Beach) {
        auto *shipMgr = EGG::egg_new<ObjectHeyhoShipManager>();
        shipMgr->load();
    }
}

/// @addr{0x80821E14}
ObjectBase *ObjectDirector::createObject(const System::MapdataGeoObj &params) {
    ObjectId id = static_cast<ObjectId>(params.id());
    switch (id) {
    case ObjectId::Psea:
        return EGG::egg_new<ObjectPsea>(params);
    case ObjectId::Woodbox:
        return EGG::egg_new<ObjectWoodbox>(params);
    case ObjectId::WLWallGC:
        return EGG::egg_new<ObjectWLWallGC>(params);
    case ObjectId::CarA1:
    case ObjectId::CarA2:
    case ObjectId::CarA3:
        return EGG::egg_new<ObjectCarA>(params);
    case ObjectId::Basabasa:
        return EGG::egg_new<ObjectBasabasa>(params);
    case ObjectId::HeyhoShipGBA:
        return EGG::egg_new<ObjectHeyhoShip>(params);
    case ObjectId::KartTruck:
    case ObjectId::CarBody:
        return EGG::egg_new<ObjectCarTGE>(params);
    case ObjectId::KoopaBall:
        return EGG::egg_new<ObjectKoopaBall>(params);
    case ObjectId::W_Woodbox:
        return EGG::egg_new<ObjectWoodboxW>(params);
    case ObjectId::SunDS:
        return EGG::egg_new<ObjectSunDS>(params);
    case ObjectId::ItemboxLine:
        return EGG::egg_new<ObjectItemboxLine>(params);
    case ObjectId::VolcanoBall:
        return EGG::egg_new<ObjectVolcanoBallLauncher>(params);
    case ObjectId::PenguinS:
        return EGG::egg_new<ObjectPenguinS>(params);
    case ObjectId::PenguinM:
        return EGG::egg_new<ObjectPenguin>(params);
    case ObjectId::Dossunc:
        return EGG::egg_new<ObjectDossunc>(params);
    case ObjectId::Boble:
        return EGG::egg_new<ObjectBoble>(params);
    case ObjectId::Hanachan:
        return EGG::egg_new<ObjectHanachan>(params);
    case ObjectId::Seagull:
        return EGG::egg_new<ObjectBird>(params);
    case ObjectId::Crab:
        return EGG::egg_new<ObjectCrab>(params);
    case ObjectId::Hwanwan:
        return EGG::egg_new<ObjectHwanwanManager>(params);
    case ObjectId::HeyhoBallGBA:
        return EGG::egg_new<ObjectHeyhoBall>(params);
    case ObjectId::DokanSFC:
        return EGG::egg_new<ObjectDokan>(params);
    case ObjectId::Pylon:
        return EGG::egg_new<ObjectPylon>(params);
    case ObjectId::OilSFC:
        return EGG::egg_new<ObjectOilSFC>(params);
    case ObjectId::ParasolR:
        return EGG::egg_new<ObjectParasolR>(params);
    case ObjectId::KoopaFigure64:
        return EGG::egg_new<ObjectKoopaFigure64>(params);
    case ObjectId::Kuribo:
        return EGG::egg_new<ObjectKuribo>(params);
    case ObjectId::Choropu:
    case ObjectId::Choropu2:
        return EGG::egg_new<ObjectChoropu>(params);
    case ObjectId::Cow:
        return EGG::egg_new<ObjectCowHerd>(params);
    case ObjectId::PakkunF:
        return EGG::egg_new<ObjectPakkunF>(params);
    case ObjectId::WLFirebarGC:
    case ObjectId::KoopaFirebar:
        return EGG::egg_new<ObjectFirebar>(params);
    case ObjectId::Wanwan:
        return EGG::egg_new<ObjectWanwan>(params);
    case ObjectId::Poihana:
        return EGG::egg_new<ObjectPoihana>(params);
    case ObjectId::Propeller:
        return EGG::egg_new<ObjectPropeller>(params);
    case ObjectId::DKRockGC:
        return EGG::egg_new<ObjectRock>(params);
    case ObjectId::Sanbo:
        return EGG::egg_new<ObjectSanbo>(params);
    case ObjectId::TruckWagon:
        return EGG::egg_new<ObjectTruckWagon>(params);
    case ObjectId::Heyho:
        return EGG::egg_new<ObjectHeyho>(params);
    case ObjectId::Press:
        return EGG::egg_new<ObjectPress>(params);
    case ObjectId::WLFireRingGC:
        return EGG::egg_new<ObjectFireRing>(params);
    case ObjectId::FireSnake:
        return EGG::egg_new<ObjectFireSnake>(params);
    case ObjectId::FireSnakeV:
        return EGG::egg_new<ObjectFireSnakeV>(params);
    case ObjectId::PuchiPakkun:
        return EGG::egg_new<ObjectPuchiPakkun>(params);
    case ObjectId::KinokoUd:
        return EGG::egg_new<ObjectKinokoUd>(params);
    case ObjectId::KinokoBend:
        return EGG::egg_new<ObjectKinokoBend>(params);
    case ObjectId::VolcanoRock:
        return EGG::egg_new<ObjectVolcanoRock>(params);
    case ObjectId::BulldozerL:
    case ObjectId::BulldozerR:
        return EGG::egg_new<ObjectBulldozer>(params);
    case ObjectId::KinokoNm:
        return EGG::egg_new<ObjectKinokoNm>(params);
    case ObjectId::Crane:
        return EGG::egg_new<ObjectCrane>(params);
    case ObjectId::VolcanoPiece:
        return EGG::egg_new<ObjectVolcanoPiece>(params);
    case ObjectId::FlamePole:
        return EGG::egg_new<ObjectFlamePoleFoot>(params);
    case ObjectId::TwistedWay:
        return EGG::egg_new<ObjectTwistedWay>(params);
    case ObjectId::TownBridge:
        return EGG::egg_new<ObjectTownBridge>(params);
    case ObjectId::DKShip64:
        return EGG::egg_new<ObjectShip64>(params);
    case ObjectId::Turibashi:
        return EGG::egg_new<ObjectTuribashi>(params);
    case ObjectId::Aurora:
        return EGG::egg_new<ObjectAurora>(params);
    case ObjectId::DCPillar:
        return EGG::egg_new<ObjectPillar>(params);
    case ObjectId::Sandcone:
        return EGG::egg_new<ObjectSandcone>(params);
    case ObjectId::FlamePoleV:
    case ObjectId::FlamePoleVBig:
        return EGG::egg_new<ObjectFlamePoleV>(params);
    case ObjectId::Ami:
        return EGG::egg_new<ObjectAmi>(params);
    case ObjectId::BeltEasy:
        return EGG::egg_new<ObjectBeltEasy>(params);
    case ObjectId::BeltCrossing:
        return EGG::egg_new<ObjectBeltCrossing>(params);
    case ObjectId::BeltCurveA:
        return EGG::egg_new<ObjectBeltCurveA>(params);
    case ObjectId::Escalator:
        return EGG::egg_new<ObjectEscalator>(params);
    case ObjectId::EscalatorGroup:
        return EGG::egg_new<ObjectEscalatorGroup>(params);

    // Non-specified objects are stock collidable objects by default
    // However, we need to specify an impl, so we don't use default
    case ObjectId::DummyPole:
    case ObjectId::CastleTree1c:
    case ObjectId::MarioTreeGCc:
    case ObjectId::PeachTreeGCc:
    case ObjectId::MarioGo64c:
    case ObjectId::KinokoT1:
    case ObjectId::PalmTree:
    case ObjectId::Parasol:
    case ObjectId::HeyhoTreeGBAc:
    case ObjectId::GardenTreeDSc:
    case ObjectId::DKtreeA64c:
    case ObjectId::DKTreeB64c:
    case ObjectId::TownTreeDsc:
    case ObjectId::PakkunDokan:
        return EGG::egg_new<ObjectCollidable>(params);
    case ObjectId::WLDokanGC:
    case ObjectId::Mdush:
        return EGG::egg_new<ObjectKCL>(params);
    default:
        return EGG::egg_new<ObjectNoImpl>(params);
    }
}

f32 ObjectDirector::s_wanwanMaxPitch; ///< @addr{0x808C70E8}

ObjectDirector *ObjectDirector::s_instance = nullptr; ///< @addr{0x809C4330}

} // namespace Kinoko::Field
