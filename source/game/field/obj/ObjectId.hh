#pragma once

#include <Common.hh>

namespace Field {

enum class ObjectId {
    DokanSFC = 0x12e,
    OilSFC = 0x15d,
};

enum class BlacklistedObjectId {
    Itembox = 0x65,
    Hanabi = 0x16a,
};

static constexpr bool IsObjectBlacklisted(u16 id) {
    BlacklistedObjectId objectId = static_cast<BlacklistedObjectId>(id);
    switch (objectId) {
    // Disabled collision
    case BlacklistedObjectId::Itembox:
        return true;

    // No collision
    case BlacklistedObjectId::Hanabi:
        return true;

    default:
        return false;
    }
}

} // namespace Field
