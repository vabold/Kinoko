#include "ObjectFlowTable.hh"

#include "game/system/ResourceManager.hh"

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

} // namespace Field
