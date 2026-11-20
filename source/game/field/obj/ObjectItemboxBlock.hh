#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

class ObjectPressSenko;

/// @brief Represents an itembox or block that spawns from an @ref ObjectItemboxLine
/// @details Actually inherits from ObjectItembox, but we don't need to implement this functionality
/// in Kinoko. Manages stomper state for the first left/right set of Toad's Factory stompers.
class ObjectItemboxBlock final : public ObjectCollidable {
public:
    ObjectItemboxBlock(const System::MapdataGeoObj &params);
    ~ObjectItemboxBlock() override;

    void init() override;
    void calc() override;
    [[nodiscard]] ObjectId id() const override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getResources() const override;
    [[nodiscard]] const char *getKclName() const override;

    void spawn();

    /// @beginSetters

    void setSenko(ObjectPressSenko *senko);

    /// @endSetters

private:
    /// @brief Describes the current state of the itembox/block along the rail
    enum class State {
        Despawned = 0, ///< The itembox/block is not spawned
        Itembox = 1,   ///< The object has been crushed and turned into an itembox
        Block = 2,     ///< The object is spawned and is a block
    };

    void calcRail();

    State m_state;             ///< Current state of the itembox/block
    ObjectPressSenko *m_senko; ///< Pointer to the associated stomper object
};

} // namespace Kinoko::Field
