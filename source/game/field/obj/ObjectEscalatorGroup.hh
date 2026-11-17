#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

class ObjectEscalator;

/// @brief Represents a group of two escalators and the Pianta dancing in the middle.
/// @details Because the dancing Pianta does not have tangible collision that affects time trial
/// playback, we take creative liberty to omit implementation for Kinoko.
class ObjectEscalatorGroup final : public ObjectCollidable {
public:
    ObjectEscalatorGroup(const System::MapdataGeoObj &params);
    ~ObjectEscalatorGroup() override;

    void createCollision() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getResources() const override;
    [[nodiscard]] const char *getKclName() const override;

private:
    ObjectEscalator *const m_rightEscalator; ///< Pointer to the right escalator
    ObjectEscalator *const m_leftEscalator;  ///< Pointer to the left escalator
};

} // namespace Kinoko::Field
