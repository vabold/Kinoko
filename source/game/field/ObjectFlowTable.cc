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

const SObjectCollisionSet *ObjectFlowTable::set(s16 i) const {
    return i == -1 ? nullptr : i < m_count ? &m_sets[i] : nullptr;
}

s16 ObjectFlowTable::slot(ObjectId id) const {
    constexpr size_t SLOT_COUNT = 0x2f4;

    size_t i = static_cast<size_t>(id);
    return i < SLOT_COUNT ? parse<s16>(m_slots[i]) : -1;
}

} // namespace Field
