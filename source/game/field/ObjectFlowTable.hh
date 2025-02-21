#pragma once

#include "game/field/obj/ObjectId.hh"

namespace Field {

/// @brief Maps to SObjectCollisionSet::mode. Determines what type of collision an object has.
enum class CollisionMode {
    None = 0,
    Sphere = 1,
    Cylinder = 2,
    Box = 3,
    Ground = 4,
    Original = 5,
};

/// @brief Structure of the ObjFlow.bin table entry. Contains dependencies and collision parameters.
/// @details For now, we only care about the collision mode and parameters.
/// @todo Are the names and resources necessary?
struct SObjectCollisionSet {
    u16 id;
    char name[32];
    char resources[64];
    u8 _62[0x6a - 0x62];
    s16 mode;
    union {
        struct {
            s16 radius;
        } sphere;
        struct {
            s16 radius;
            s16 height;
        } cylinder;
        struct {
            s16 x;
            s16 y;
            s16 z;
        } box;
    } params;
    u8 _72[0x74 - 0x72];
};
STATIC_ASSERT(sizeof(SObjectCollisionSet) == 0x74);

class ObjectFlowTable {
public:
    ObjectFlowTable(const char *filename);
    ~ObjectFlowTable();

    const SObjectCollisionSet *set(s16 slot) const {
        return slot == -1 ? nullptr : slot < m_count ? &m_sets[slot] : nullptr;
    }

    [[nodiscard]] s16 slot(ObjectId id) const {
        constexpr size_t SLOT_COUNT = 0x2f4;

        size_t i = static_cast<size_t>(id);
        return i < SLOT_COUNT ? parse<s16>(m_slots[i]) : -1;
    }

private:
    struct SFile {
        s16 count;
        SObjectCollisionSet sets[];
    };

    s16 m_count;
    const SObjectCollisionSet *m_sets;
    const s16 *m_slots;
};

} // namespace Field
