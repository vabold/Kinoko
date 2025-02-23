#include "ObjectHitTable.hh"

#include "game/system/ResourceManager.hh"

#include <egg/util/Stream.hh>

namespace Field {

/// @addr{0x807F9278}
ObjectHitTable::ObjectHitTable(const char *filename) {
    size_t size;
    void *file =
            System::ResourceManager::Instance()->getFile(filename, &size, System::ArchiveId::Core);

    EGG::RamStream stream = EGG::RamStream(file, size);

    m_count = stream.read_s16();
    m_fieldCount = stream.read_s16();
    m_reactions = std::span<s16>(new s16[m_count], m_count);

    for (auto &reaction : m_reactions) {
        stream.skip(0x2);
        reaction = stream.read_s16();
        stream.skip(m_fieldCount * 2 - 2);
    }

    m_slots = reinterpret_cast<const s16 *>(stream.dataAtIndex());
}

/// @addr{0x807F9348}
ObjectHitTable::~ObjectHitTable() {
    delete m_reactions.data();
}

Kart::Reaction ObjectHitTable::reaction(s16 i) const {
    ASSERT(i != -1);
    ASSERT(i < m_count);
    return static_cast<Kart::Reaction>(m_reactions[i]);
}

s16 ObjectHitTable::slot(ObjectId id) const {
    constexpr size_t SLOT_COUNT = 0x2f4;

    size_t i = static_cast<std::underlying_type_t<ObjectId>>(id);
    return i < SLOT_COUNT ? parse<s16>(m_slots[i]) : -1;
}

} // namespace Field
