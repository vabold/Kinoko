#include "ObjectFlowTable.hh"

#include "game/system/ResourceManager.hh"

#include <cstring>

namespace Field {

/// @addr{0x8082C10C}
ObjectFlowTable::ObjectFlowTable(const char *filename) {
    SFile *file = reinterpret_cast<SFile *>(System::ResourceManager::Instance()->getFile(filename,
            nullptr, System::ArchiveId::Core));

    m_count = parse<s16>(file->count);
    m_sets = file->sets;
    m_slots = reinterpret_cast<const s16 *>(m_sets + m_count);
}

/// @addr{0x8082C1F4}
ObjectFlowTable::~ObjectFlowTable() = default;

/// @addr{0x8082C178}
ObjectId ObjectFlowTable::getIdFromName(const char *name) const {
    for (s16 i = 0; i < m_count; ++i) {
        const auto *curSet = set(i);
        ASSERT(curSet);

        if (strncmp(name, curSet->name, sizeof(curSet->name)) == 0) {
            return static_cast<ObjectId>(parse<u16>(curSet->id));
        }
    }

    return ObjectId::None;
}

} // namespace Field
