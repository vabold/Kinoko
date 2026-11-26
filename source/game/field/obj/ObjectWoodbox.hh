#pragma once

#include "game/field/ObjectCollisionBase.hh"
#include "game/field/obj/ObjectBreakable.hh"

namespace Kinoko::Field {

/// @brief Represents a wooden box, like on DS Delfino Square
/// @details Normally these boxes are breakable via items or collisions, but in time trial mode,
/// these boxes become unbreakable "ironboxes".
class ObjectWoodbox : public ObjectBreakable {
public:
    /// @addr{0x8077E5E4}
    /// @copybrief ObjectBreakable::ObjectBreakable(const System::MapdataGeoObj &)
    /// @param params The parameters used to initialize the object
    ObjectWoodbox(const System::MapdataGeoObj &params) : ObjectBreakable(params) {}

    /// @addr{0x8077E620}
    /// @brief Default virtual destructor
    ~ObjectWoodbox() override = default;

    /// @addr{0x8077ED7C}
    /// @copybrief ObjectBase::getKclName()
    /// @return The model name of the ironbox (always `ironbox` in time trials).
    [[nodiscard]] const char *getKclName() const override {
        return "ironbox"; // woodbox when not in TTs
    }

    /// @addr{0x8077EBB8}
    /// @copybrief ObjectBase::calcCollisionTransform()
    /// @details Sets the collision transform by offsetting the box's position upwards by half its
    /// width.
    void calcCollisionTransform() override {
        constexpr f32 HALF_SIZE = 100.0f;
        constexpr EGG::Vector3f POS_OFFSET = EGG::Vector3f(0.0f, HALF_SIZE, 0.0f);

        if (!m_collision) {
            return;
        }

        calcTransform();
        EGG::Matrix34f mat = transform();
        mat.setBase(3, pos() + POS_OFFSET);
        m_collision->transform(mat, scale(), getCollisionTranslation());
    }
};

} // namespace Kinoko::Field
