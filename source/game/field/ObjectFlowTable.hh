#pragma once

#include <Common.hh>

namespace Field {

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
static_assert(sizeof(SObjectCollisionSet) == 0x74);

class ObjectFlowTable {
public:
    ObjectFlowTable(const char *filename);
    ~ObjectFlowTable();

private:
    struct SFile {
        s16 count;
        SObjectCollisionSet sets[];
    };

    s16 m_count;
    SObjectCollisionSet *m_sets;
    s16 *m_slots;
};

} // namespace Field
