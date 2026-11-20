#include "ObjectHighwayManager.hh"

#include "game/field/ObjectDirector.hh"
#include "game/field/obj/ObjectCarTGE.hh"

#include <cstring>

namespace Kinoko::Field {

/// @addr{0x806D2908}
/// @brief Constructor that assigns this manager to all cars and trucks
/// @pre All @ref ObjectCarTGE objects must be constructed and registered to the vector of managed
/// objects in @ref ObjectDirector. Cars registered after this manager is constructed will have no
/// squish cooldown at all.
/// @details Constructs the object named `HighwayManager`. Counts the number of managed cars and
/// trucks and sizes @ref m_cars accordingly. For each of these managed cars/trucks, caches a
/// pointer to that object and sets its highway manager to this instance.
ObjectHighwayManager::ObjectHighwayManager()
    : ObjectCollidable("HighwayManager", EGG::Vector3f::zero, EGG::Vector3f::ez,
              EGG::Vector3f::unit) {
    size_t carCount = 0;
    auto *objDir = ObjectDirector::Instance();

    for (auto *const &obj : objDir->managedObjects()) {
        const char *objName = obj->getName();

        if ((strcmp(objName, "car_body") == 0) || (strcmp(objName, "kart_truck") == 0)) {
            ++carCount;
        }
    }

    m_cars = owning_span<ObjectCarTGE *>(carCount);
    size_t idx = 0;

    for (auto *const &obj : objDir->managedObjects()) {
        const char *objName = obj->getName();

        if ((strcmp(objName, "car_body") == 0) || (strcmp(objName, "kart_truck") == 0)) {
            auto *carObj = reinterpret_cast<ObjectCarTGE *>(obj);
            m_cars[idx++] = carObj;
            carObj->setHighwayManager(this);
        }
    }
}

/// @addr{0x806D2FE8}
/// @brief Default virtual destructor
ObjectHighwayManager::~ObjectHighwayManager() = default;

/// @addr{0x806D332C}
/// @copybrief ObjectBase::init()
/// @details Initializes @ref m_squashTimer to @ref SQUASH_MAX so that the player is initially
/// vulnerable to being squashed.
void ObjectHighwayManager::init() {
    m_squashTimer = SQUASH_MAX;
}

/// @addr{0x806D345C}
/// @copybrief ObjectBase::calc()
/// @details Simply calls @ref calcSquash() to check if any managed @ref ObjectCarTGE has
/// squashed the player this frame.
void ObjectHighwayManager::calc() {
    calcSquash();
}

/// @addr{0x806D5C68}
/// @copybrief ObjectBase::loadGraphics()
/// @details Does nothing since this is just a watcher class.
void ObjectHighwayManager::loadGraphics() {}

/// @addr{0x806D5C60}
/// @copybrief ObjectBase::createCollision()
/// @details Does nothing since this is just a watcher class.
void ObjectHighwayManager::createCollision() {}

/// @addr{0x806D5C64}
/// @copybrief ObjectBase::loadRail()
/// @details Does nothing since this is just a watcher class.
void ObjectHighwayManager::loadRail() {}

/// @addr{0x806D5C6C}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectHighwayManager::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806D50AC}
/// @brief Iterates all vehicles and resets the squash timer if any vehicle has squashed the player
/// @details If any vehicle has squashed the player and the player is currently vulnerable, resets
/// the squash timer to 0. Regardless, increments the squash timer once per frame, capped at @ref
/// SQUASH_MAX.
void ObjectHighwayManager::calcSquash() {
    constexpr u32 SQUASH_INVULNERABILITY = 200;

    bool vulnerable = m_squashTimer >= SQUASH_INVULNERABILITY;

    for (auto *&obj : m_cars) {
        if (obj->squashed() && vulnerable) {
            m_squashTimer = 0;
        }

        obj->reset();
    }

    m_squashTimer = std::min<u32>(m_squashTimer + 1, SQUASH_MAX);
}

} // namespace Kinoko::Field
