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
    m_inputs = raw->buffer() + 0x88;
}

void GhostFile::readHeader(RawGhostFile *raw) {
    memcpy(m_userData.data(), raw->buffer() + 0x20, 0x14);
    m_userData[10] = L'\0';

    u32 data = raw->parseAt<u32>(0x8);
    m_year = (data >> 0xD) & 0x7F;
    m_month = (data >> 0x9) & 0xF;
    m_day = (data >> 0x4) & 0x1F;

    memcpy(m_miiData.data(), raw->buffer() + 0x3C, 0x4A);

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

Timer::Timer() : min(0), sec(0), mil(0) {}

Timer::Timer(u32 data) {
    min = static_cast<u16>(data >> 0x19);
    sec = static_cast<u8>(data >> 0x12) & 0x7F;
    mil = static_cast<u16>(data >> 8) & 0x3FF;
}

Timer::~Timer() = default;

RawGhostFile::RawGhostFile(const u8 *rkg) {
    if (!isValid(rkg)) {
        K_PANIC("Invalid RKG header");
    }

    if (compressed(rkg)) {
        if (!decompress(rkg)) {
            K_PANIC("Failed to decompress RKG!");
        }
    } else {
        memcpy(m_buffer, rkg, 0x27FC);
    }
}

RawGhostFile::~RawGhostFile() = default;

bool RawGhostFile::decompress(const u8 *rkg) {
    // Copy header
    memcpy(m_buffer, rkg, 0x88);

    // Unset compressed flag
    *(m_buffer + 0xC) &= 0xF7;

    // Get uncompressed size
    s32 uncompressedSize = EGG::Decomp::GetExpandSize(rkg + 0x8C);

    if (uncompressedSize == 0 || uncompressedSize > 0x2774) {
        return false;
    }

    EGG::Decomp::DecodeSZS(rkg + 0x8C, m_buffer + 0x88);

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
    WeightClass charWeight = characterToWeight(character);
    WeightClass vehicleWeight = vehicleToWeight(vehicle);
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
