#pragma once

#include "game/field/obj/ObjectId.hh"

#include "game/kart/KartCollide.hh"

#include <span>

namespace Field {

class ObjectHitTable {
public:
    ObjectHitTable(const char *filename);
    ~ObjectHitTable();

    Kart::Reaction reaction(s16 i) const;
    s16 slot(ObjectId id) const;

private:
    s16 m_count;
    s16 m_fieldCount;
    std::span<s16> m_reactions;
    const s16 *m_slots;
};

} // namespace Field
