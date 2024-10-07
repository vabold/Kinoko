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

    const SObjectCollisionSet *set(s16 slot) const;
    s16 slot(ObjectId id) const;

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
