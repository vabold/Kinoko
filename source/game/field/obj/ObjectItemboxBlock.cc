#include "ObjectItemboxBlock.hh"

#include "game/field/RailInterpolator.hh"
#include "game/field/obj/ObjectPress.hh"

namespace Kinoko::Field {

/// @addr{0x8076D9E4}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
ObjectItemboxBlock::ObjectItemboxBlock(const System::MapdataGeoObj &params)
    : ObjectCollidable(params) {}

/// @addr{0x8076DA48}
/// @brief Default virtual destructor
ObjectItemboxBlock::~ObjectItemboxBlock() = default;

/// @addr{0x8076DA88}
/// @copybrief ObjectBase::init()
/// @details Sets the initial state of the itembox/block to @ref State::Despawned.
void ObjectItemboxBlock::init() {
    m_state = State::Despawned;
}

/// @addr{0x8076DAF4}
/// @copybrief ObjectBase::calc()
/// @details Updates the position of the itembox/block along the rail if it is currently
/// spawned.
void ObjectItemboxBlock::calc() {
    constexpr f32 HEIGHT_OFFSET = 180.0f;

    switch (m_state) {
    case State::Itembox:
    case State::Block: {
        calcRail();
        const auto &railPos = m_railInterpolator->curPos();
        setPos(EGG::Vector3f(railPos.x, railPos.y + HEIGHT_OFFSET, railPos.z));
    } break;
    default:
        break;
    }
}

/// @addr{0x8076E9E4}
/// @copybrief ObjectBase::id()
/// @return The ID of an itembox, `ObjectId::Itembox`.
ObjectId ObjectItemboxBlock::id() const {
    return ObjectId::Itembox;
}

/// @addr{0x8076E9C4}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectItemboxBlock::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x8076E9CC}
/// @copybrief ObjectBase::getResources()
/// @return The resource name for the itembox, `itembox`.
const char *ObjectItemboxBlock::getResources() const {
    return "itembox";
}

/// @addr{0x8076E9D8}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the itembox, `itembox`.
const char *ObjectItemboxBlock::getKclName() const {
    return "itembox";
}

/// @addr{0x8076DA98}
/// @brief Spawns the itembox/block at the start of the rail
/// @details Sets @ref m_state to @ref State::Block, initializes the rail interpolator to the start
/// of the rail, and sets the per-point velocities flag to true.
void ObjectItemboxBlock::spawn() {
    m_state = State::Block;
    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setPerPointVelocities(true);
}

/// @brief Assigns the @ref ObjectPressSenko object associated with this itembox/block
/// @param senko Pointer to the @ref ObjectPressSenko object to associate with this
/// itembox/block.
void ObjectItemboxBlock::setSenko(ObjectPressSenko *senko) {
    m_senko = senko;
}

/// @addr{0x8076DF44}
/// @brief Updates the rail interpolator and stomper state
/// @details Updates the rail interpolator. If the itembox/block has reached the end of a rail
/// segment and the rail node has setting 2 set to 1, the associated stomper is activated.
/// Otherwise, if the itembox/block has reached the end of the rail, it is despawned.
void ObjectItemboxBlock::calcRail() {
    auto result = m_railInterpolator->calc();
    if (result == RailInterpolator::Status::SegmentEnd) {
        if (m_railInterpolator->curPoint().setting[1] == 1) {
            m_senko->beginStomp();
        }
    } else if (result == RailInterpolator::Status::RailEnd) {
        m_state = State::Despawned;
    }
}

} // namespace Kinoko::Field
