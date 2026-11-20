#include "ObjectItemboxLine.hh"

#include "game/field/obj/ObjectPress.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x8076D044}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details To prevent frequent dereferencing of the map object, for Kinoko we cache the time
/// between spawns to @ref m_spawnCooldownDuration based on param setting 6. Constructs and
/// loads the @ref ObjectPressSenko object that represents the left/right stompers. Finally,
/// constructs the @ref ObjectItemboxBlock objects for each block that can spawn. Even though
/// these blocks are not tangible/visible in time trials, they still influence the behavior of
/// the stompers.
ObjectItemboxLine::ObjectItemboxLine(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_spawnCooldownDuration(static_cast<u32>(params.setting(5))) {
    constexpr u32 DEFAULT_PRESS_COUNT = 5;

    auto *senko = EGG::egg_new<ObjectPressSenko>(params);
    senko->load();

    u32 pressCount = params.setting(6);
    if (pressCount == 0) {
        pressCount = DEFAULT_PRESS_COUNT;
    }

    m_blocks = owning_span<ObjectItemboxBlock *>(pressCount);

    for (auto *&block : m_blocks) {
        block = EGG::egg_new<ObjectItemboxBlock>(params);
        block->load();
        block->setSenko(senko);
    }
}

/// @addr{0x8076D558}
/// @brief Default virtual destructor
ObjectItemboxLine::~ObjectItemboxLine() = default;

/// @addr{0x8076D604}
/// @copybrief ObjectBase::init()
/// @details The block spawn cooldown is initialized based on param setting 5. If the setting is
/// 0, then @ref m_spawnCooldownDuration is used instead. Finally, initializes @ref
/// m_curBlockIdx to 0.
void ObjectItemboxLine::init() {
    ASSERT(m_mapObj);
    u32 timer = static_cast<u32>(m_mapObj->setting(4));

    if (timer == 0) {
        timer = m_spawnCooldownDuration;
    }

    m_spawnCooldown = timer;
    m_curBlockIdx = 0;
}

/// @addr{0x8076D64C}
/// @copybrief ObjectBase::calc()
/// @details Decrements the spawn cooldown timer and spawns the next block when it reaches 0.
void ObjectItemboxLine::calc() {
    if (--m_spawnCooldown > 0) {
        return;
    }

    m_spawnCooldown = m_spawnCooldownDuration;

    m_blocks[m_curBlockIdx]->spawn();
    m_curBlockIdx = (m_curBlockIdx + 1) % m_blocks.size();
}

/// @addr{0x8076E9BC}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectItemboxLine::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

} // namespace Kinoko::Field
