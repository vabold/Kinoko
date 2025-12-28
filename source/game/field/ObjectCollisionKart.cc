#include "ObjectCollisionKart.hh"

#include "game/field/ObjectDirector.hh"

#include "game/kart/KartObjectManager.hh"

#include "game/system/RaceConfig.hh"

namespace Field {

/// @addr{0x8081E0CC}
ObjectCollisionKart::ObjectCollisionKart() : m_kartObject(nullptr) {}

/// @addr{0x8081E0E4}
ObjectCollisionKart::~ObjectCollisionKart() {
    delete m_hull;
}

/// @addr{0x8081D090}
void ObjectCollisionKart::init(u32 idx) {
    if (m_kartObject) {
        return;
    }

    m_kartObject = Kart::KartObjectManager::Instance()->object(idx);
    m_playerIdx = idx;

    auto vehicle = System::RaceConfig::Instance()->raceScenario().players[idx].vehicle;
    m_hull = new ObjectCollisionConvexHull(GetVehicleVertices(vehicle));
}

/// @addr{0x8081E170}
size_t ObjectCollisionKart::checkCollision(const EGG::Matrix34f &mat, const EGG::Vector3f &v) {
    if (!m_hull) {
        return 0;
    }

    const EGG::Vector3f &scale = m_kartObject->scale();
    m_hull->transform(mat, scale, v);
    m_hull->setBoundingRadius(scale.x * m_hull->initRadius());

    return ObjectDirector::Instance()->checkKartObjectCollision(m_kartObject, m_hull);
}

/// @addr{0x80572544}
EGG::Vector3f ObjectCollisionKart::GetHitDirection(u16 objKartHit) {
    EGG::Vector3f hitDepth = ObjectDirector::Instance()->hitDepth(objKartHit);
    hitDepth.normalise();
    return hitDepth;
}

/// @brief Helper function to map between a vehicle and its set of convex hull vertices.
constexpr std::span<const EGG::Vector3f> ObjectCollisionKart::GetVehicleVertices(Vehicle vehicle) {
    static constexpr std::array<EGG::Vector3f, 2> VERT_STANDARD_KART_S = {{
            {0.0f, 35.0f, -40.0f},
            {0.0f, 35.0f, 25.0f},
    }};

    static constexpr std::array<EGG::Vector3f, 4> VERT_STANDARD_KART_M = {{
            {0.0, 60.0, 10.0},
            {10.0, 35.0, -50.0},
            {-10.0, 35.0, -50.0},
            {0.0, 35.0, 65.0},
    }};

    static constexpr std::array<EGG::Vector3f, 4> VERT_STANDARD_KART_L = {{
            {0.0, 135.0, 30.0},
            {25.0, 35.0, -95.0},
            {-25.0, 35.0, -95.0},
            {0.0, 35.0, 105.0},
    }};

    static constexpr std::array<EGG::Vector3f, 4> VERT_BOOSTER_SEAT = {{
            {0.0, 120.0, 0.0},
            {0.0, 95.0, -55.0},
            {0.0, 10.0, -15.0},
            {0.0, 10.0, 20.0},
    }};

    static constexpr std::array<EGG::Vector3f, 5> VERT_CLASSIC_DRAGSTER = {{
            {0.0, 55.0, -25.0},
            {20.0, -5.0, -85.0},
            {-20.0, -5.0, -85.0},
            {20.0, -5.0, 60.0},
            {-20.0, -5.0, 60.0},
    }};

    static constexpr std::array<EGG::Vector3f, 5> VERT_OFFROADER = {{
            {0.0, 85.0, 30.0},
            {60.0, 5.0, -95.0},
            {-60.0, 5.0, -95.0},
            {50.0, 5.0, 130.0},
            {-50.0, 5.0, 130.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_MINI_BEAST = {{
            {0.0, 25.0, -10.0},
            {0.0, 5.0, -60.0},
            {0.0, 5.0, 40.0},
    }};

    static constexpr std::array<EGG::Vector3f, 4> VERT_WILD_WING = {{
            {0.0, 60.0, -10.0},
            {15.0, 25.0, -80.0},
            {-15.0, 25.0, -80.0},
            {0.0, 25.0, 80.0},
    }};

    static constexpr std::array<EGG::Vector3f, 5> VERT_FLAME_FLYER = {{
            {0.0, 80.0, 0.0},
            {30.0, 0.0, -105.0},
            {-30.0, 0.0, -105.0},
            {25.0, 0.0, 105.0},
            {-25.0, 0.0, 105.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_CHEEP_CHARGER = {{
            {0.0, 40.0, 0.0},
            {0.0, 25.0, -10.0},
            {0.0, 25.0, 10.0},
    }};

    static constexpr std::array<EGG::Vector3f, 4> VERT_SUPER_BLOOPER = {{
            {0.0, 50.0, 0.0},
            {0.0, 25.0, 50.0},
            {-25.0, 25.0, -20.0},
            {25.0, 25.0, -20.0},
    }};

    static constexpr std::array<EGG::Vector3f, 5> VERT_PIRANHA_PROWLER = {{
            {0.0, 95.0, -10.0},
            {35.0, -25.0, -115.0},
            {-35.0, -25.0, -115.0},
            {35.0, -25.0, 85.0},
            {-35.0, -25.0, 85.0},
    }};

    static constexpr std::array<EGG::Vector3f, 5> VERT_TINY_TITAN = {{
            {0.0, 55.0, -10.0},
            {25.0, -10.0, -40.0},
            {-25.0, -10.0, -40.0},
            {-25.0, -10.0, 30.0},
            {25.0, -10.0, 30.0},
    }};

    static constexpr std::array<EGG::Vector3f, 5> VERT_DAYTRIPPER = {{
            {0.0, 60.0, 0.0},
            {12.0, 5.0, -55.0},
            {-12.0, 5.0, -55.0},
            {8.0, 5.0, 35.0},
            {-8.0, 5.0, 35.0},
    }};

    static constexpr std::array<EGG::Vector3f, 5> VERT_JETSETTER = {{
            {0.0, 120.0, 30.0},
            {25.0, 30.0, -75.0},
            {-25.0, 30.0, -75.0},
            {20.0, 30.0, 115.0},
            {-20.0, 30.0, 115.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_BLUE_FALCON = {{
            {20.0, 20.0, -30.0},
            {-20.0, 20.0, -30.0},
            {0.0, 20.0, 80.0},
    }};

    static constexpr std::array<EGG::Vector3f, 5> VERT_SPRINTER = {{
            {0.0, 60.0, 0.0},
            {25.0, 20.0, -60.0},
            {-25.0, 20.0, -60.0},
            {20.0, 20.0, 75.0},
            {-20.0, 20.0, 75.0},
    }};

    static constexpr std::array<EGG::Vector3f, 7> VERT_HONEYCOUPE = {{
            {0.0, 100.0, 50.0},
            {60.0, 20.0, -110.0},
            {-60.0, 20.0, -110.0},
            {60.0, -40.0, -110.0},
            {-60.0, -40.0, -110.0},
            {50.0, 0.0, 130.0},
            {-50.0, 0.0, 130.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_STANDARD_BIKE_S = {{
            {0.0, 40.0, 0.0},
            {0.0, 5.0, -15.0},
            {0.0, 5.0, 20.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_STANDARD_BIKE_M = {{
            {0.0, 75.0, 10.0},
            {0.0, 10.0, -20.0},
            {0.0, 10.0, 20.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_STANDARD_BIKE_L = {{
            {0.0, 110.0, -15.0},
            {0.0, -5.0, -35.0},
            {0.0, -5.0, 40.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_BULLET_BIKE = {{
            {0.0, 25.0, 10.0},
            {0.0, -10.0, -45.0},
            {0.0, -10.0, 50.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_MACH_BIKE = {{
            {0.0, 50.0, 15.0},
            {0.0, 5.0, -30.0},
            {0.0, 5.0, 35.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_FLAME_RUNNER = {{
            {0.0, 105.0, -10.0},
            {0.0, -5.0, -50.0},
            {0.0, -5.0, 45.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_BIT_BIKE = {{
            {0.0, 40.0, -10.0},
            {0.0, 15.0, -15.0},
            {0.0, 15.0, 5.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_SUGARSCOOT = {{
            {0.0, 70.0, 10.0},
            {0.0, 10.0, -40.0},
            {0.0, 10.0, 15.0},
    }};

    static constexpr std::array<EGG::Vector3f, 4> VERT_WARIO_BIKE = {{
            {0.0, 110.0, 35.0},
            {0.0, 70.0, -65.0},
            {0.0, -10.0, -75.0},
            {0.0, -5.0, 85.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_QUACKER = {{
            {0.0, 40.0, 0.0},
            {0.0, 10.0, 20.0},
            {0.0, 10.0, -5.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_ZIP_ZIP = {{
            {0.0, 75.0, 10.0},
            {0.0, 10.0, -25.0},
            {0.0, 10.0, 20.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_SHOOTING_STAR = {{
            {0.0, 120.0, 35.0},
            {0.0, -15.0, -45.0},
            {0.0, -15.0, 100.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_MAGIKRUISER = {{
            {0.0, 35.0, -20.0},
            {0.0, 0.0, -70.0},
            {0.0, 0.0, 20.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_SNEAKSTER = {{
            {0.0, 50.0, 20.0},
            {0.0, 15.0, -50.0},
            {0.0, 15.0, 30.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_SPEAR = {{
            {0.0, 105.0, -20.0},
            {0.0, -15.0, -60.0},
            {0.0, -15.0, 75.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_JET_BUBBLE = {{
            {0.0, 35.0, 10.0},
            {0.0, -5.0, -45.0},
            {0.0, -5.0, 50.0},
    }};

    static constexpr std::array<EGG::Vector3f, 3> VERT_DOLPHIN_DASHER = {{
            {0.0, 50.0, 20.0},
            {0.0, -5.0, -35.0},
            {0.0, -5.0, 42.0},
    }};

    static constexpr std::array<EGG::Vector3f, 4> VERT_PHANTOM = {{
            {0.0, 120.0, 35.0},
            {20.0, -5.0, -85.0},
            {-20.0, -5.0, -85.0},
            {0.0, -5.0, 100.0},
    }};

    static constexpr std::array<EGG::Vector3f, 7> VERT_DEFAULT = {{
            {0.0, 140.0, 10.0},
            {-60.0, 70.0, 40.0},
            {60.0, 70.0, 40.0},
            {0.0, -40.0, -160.0},
            {-60.0, -40.0, 50.0},
            {60.0, -40.0, 50.0},
            {0.0, -40.0, 160.0},
    }};

    switch (vehicle) {
    case Vehicle::Standard_Kart_S:
        return VERT_STANDARD_KART_S;
    case Vehicle::Standard_Kart_M:
        return VERT_STANDARD_KART_M;
    case Vehicle::Standard_Kart_L:
        return VERT_STANDARD_KART_L;
    case Vehicle::Baby_Booster:
        return VERT_BOOSTER_SEAT;
    case Vehicle::Classic_Dragster:
        return VERT_CLASSIC_DRAGSTER;
    case Vehicle::Offroader:
        return VERT_OFFROADER;
    case Vehicle::Mini_Beast:
        return VERT_MINI_BEAST;
    case Vehicle::Wild_Wing:
        return VERT_WILD_WING;
    case Vehicle::Flame_Flyer:
        return VERT_FLAME_FLYER;
    case Vehicle::Cheep_Charger:
        return VERT_CHEEP_CHARGER;
    case Vehicle::Super_Blooper:
        return VERT_SUPER_BLOOPER;
    case Vehicle::Piranha_Prowler:
        return VERT_PIRANHA_PROWLER;
    case Vehicle::Tiny_Titan:
        return VERT_TINY_TITAN;
    case Vehicle::Daytripper:
        return VERT_DAYTRIPPER;
    case Vehicle::Jetsetter:
        return VERT_JETSETTER;
    case Vehicle::Blue_Falcon:
        return VERT_BLUE_FALCON;
    case Vehicle::Sprinter:
        return VERT_SPRINTER;
    case Vehicle::Honeycoupe:
        return VERT_HONEYCOUPE;
    case Vehicle::Standard_Bike_S:
        return VERT_STANDARD_BIKE_S;
    case Vehicle::Standard_Bike_M:
        return VERT_STANDARD_BIKE_M;
    case Vehicle::Standard_Bike_L:
        return VERT_STANDARD_BIKE_L;
    case Vehicle::Bullet_Bike:
        return VERT_BULLET_BIKE;
    case Vehicle::Mach_Bike:
        return VERT_MACH_BIKE;
    case Vehicle::Flame_Runner:
        return VERT_FLAME_RUNNER;
    case Vehicle::Bit_Bike:
        return VERT_BIT_BIKE;
    case Vehicle::Sugarscoot:
        return VERT_SUGARSCOOT;
    case Vehicle::Wario_Bike:
        return VERT_WARIO_BIKE;
    case Vehicle::Quacker:
        return VERT_QUACKER;
    case Vehicle::Zip_Zip:
        return VERT_ZIP_ZIP;
    case Vehicle::Shooting_Star:
        return VERT_SHOOTING_STAR;
    case Vehicle::Magikruiser:
        return VERT_MAGIKRUISER;
    case Vehicle::Sneakster:
        return VERT_SNEAKSTER;
    case Vehicle::Spear:
        return VERT_SPEAR;
    case Vehicle::Jet_Bubble:
        return VERT_JET_BUBBLE;
    case Vehicle::Dolphin_Dasher:
        return VERT_DOLPHIN_DASHER;
    case Vehicle::Phantom:
        return VERT_PHANTOM;
    default:
        return VERT_DEFAULT;
    }
}

/// @addr{0x80573464}
const EGG::Vector3f &ObjectCollisionKart::translation(size_t idx) {
    const auto *objCol = ObjectDirector::Instance()->collidingObject(idx)->collision();
    return objCol ? objCol->translation() : EGG::Vector3f::zero;
}

} // namespace Field
