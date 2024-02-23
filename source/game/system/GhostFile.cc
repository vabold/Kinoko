#include "GhostFile.hh"

#include <egg/core/Decomp.hh>

#include <cstring>

namespace System {

GhostFile::GhostFile(RawGhostFile *raw) {
    read(raw);
}

GhostFile::~GhostFile() = default;

void GhostFile::read(RawGhostFile *raw) {
    readHeader(raw);
    m_inputSize = raw->parseAt<u16>(0xE);
    m_inputs = raw->buffer() + RKG_HEADER_SIZE;
}

void GhostFile::readHeader(RawGhostFile *raw) {
    memcpy(m_userData.data(), raw->buffer() + RKG_USER_DATA_OFFSET, RKG_USER_DATA_SIZE);
    m_userData[10] = L'\0';

    u32 data = raw->parseAt<u32>(0x8);
    m_year = (data >> 0xD) & 0x7F;
    m_month = (data >> 0x9) & 0xF;
    m_day = (data >> 0x4) & 0x1F;

    memcpy(m_miiData.data(), raw->buffer() + RKG_MII_DATA_OFFSET, RKG_MII_DATA_SIZE);

    m_lapCount = raw->parseAt<u8>(0x10);

    // TODO: Do we want to add a bounds check here?
    for (u8 lap = 0; lap < m_lapCount; ++lap) {
        m_lapTimes[lap] = Timer(raw->parseAt<u32>(0x11 + lap * 3));
    }

    m_raceTime = Timer(raw->parseAt<u32>(0x4));

    m_course = static_cast<Course>(raw->parseAt<u32>(0x4) >> 0x2 & 0x3F);
    m_character = static_cast<Character>(raw->parseAt<u16>(0x8) >> 0x4 & 0x3F);
    m_vehicle = static_cast<Vehicle>(raw->parseAt<u8>(0x8) >> 0x2);
    m_controllerId = raw->parseAt<u8>(0xB) & 0xF;
    m_type = raw->parseAt<u16>(0xC) >> 0x2 & 0x7F;
    m_location = raw->parseAt<u32>(0x34);
    m_driftIsAuto = raw->parseAt<u8>(0xD) >> 1 & 0x1;
}

Character GhostFile::character() const {
    return m_character;
}

Vehicle GhostFile::vehicle() const {
    return m_vehicle;
}

Course GhostFile::course() const {
    return m_course;
}

const u8 *GhostFile::inputs() const {
    return m_inputs;
}

bool GhostFile::driftIsAuto() const {
    return m_driftIsAuto;
}

RawGhostFile::RawGhostFile(const u8 *rkg) {
    if (!isValid(rkg)) {
        K_PANIC("Invalid RKG header");
    }

    if (compressed(rkg)) {
        if (!decompress(rkg)) {
            K_PANIC("Failed to decompress RKG!");
        }
    } else {
        memcpy(m_buffer, rkg, RKG_UNCOMPRESSED_FILE_SIZE);
    }
}

RawGhostFile::~RawGhostFile() = default;

bool RawGhostFile::decompress(const u8 *rkg) {
    memcpy(m_buffer, rkg, RKG_HEADER_SIZE);

    // Unset compressed flag
    *(m_buffer + 0xC) &= 0xF7;

    // Get uncompressed size. Skip past 0x4 bytes which represents the size of the compressed data
    s32 uncompressedSize = EGG::Decomp::GetExpandSize(rkg + RKG_HEADER_SIZE + 0x4);

    if (uncompressedSize <= 0 ||
            static_cast<u32>(uncompressedSize) > RKG_UNCOMPRESSED_INPUT_DATA_SECTION_SIZE) {
        return false;
    }

    EGG::Decomp::DecodeSZS(rkg + RKG_HEADER_SIZE + 0x4, m_buffer + RKG_HEADER_SIZE);

    // Set input data section length
    *reinterpret_cast<u16 *>(m_buffer + 0xE) = static_cast<u16>(uncompressedSize);

    return true;
}

bool RawGhostFile::isValid(const u8 *rkg) const {
    if (strncmp(reinterpret_cast<const char *>(rkg), "RKGD", 4) != 0) {
        K_PANIC("RKG header malformed");
        return false;
    }

    u32 ids = parse<u32>(*reinterpret_cast<const u32 *>(rkg + 0x8));
    Vehicle vehicle = static_cast<Vehicle>(ids >> 0x1a);
    Character character = static_cast<Character>((ids >> 0x14) & 0x3f);
    u8 year = (ids >> 0xd) & 0x7f;
    u8 day = (ids >> 0x4) & 0x1f;
    u8 month = (ids >> 0x9) & 0xf;

    if (vehicle >= Vehicle::Max || character >= Character::Max) {
        return false;
    }

    if (year >= 100 || day >= 32 || month > 12) {
        return false;
    }

    // Validate weight class match
    WeightClass charWeight = CharacterToWeight(character);
    WeightClass vehicleWeight = VehicleToWeight(vehicle);

    if (charWeight == WeightClass::Invalid) {
        K_PANIC("Invalid character weight class!");
    }
    if (vehicleWeight == WeightClass::Invalid) {
        K_PANIC("Invalid vehicle weight class!");
    }
    if (charWeight != vehicleWeight) {
        K_PANIC("Character/Bike weight class mismatch!");
    }

    // TODO:
    // - Check for valid controller type??
    // - Check lap times sum to race time??

    return true;
}

const u8 *RawGhostFile::buffer() const {
    return m_buffer;
}

template <typename T>
T RawGhostFile::parseAt(size_t offset) const {
    return parse<T>(*reinterpret_cast<const T *>(m_buffer + offset));
}

bool RawGhostFile::compressed(const u8 *rkg) const {
    return ((*(rkg + 0xC) >> 3) & 1) == 1;
}

} // namespace System
