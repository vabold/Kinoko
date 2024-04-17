#pragma once

#include "game/system/Timer.hh"

#include <egg/util/Stream.hh>

#include <array>

namespace System {

static constexpr size_t RKG_HEADER_SIZE = 0x88;
static constexpr size_t RKG_INPUT_DATA_HEADER_SIZE = 0x8;
static constexpr size_t RKG_UNCOMPRESSED_INPUT_DATA_SECTION_SIZE = 0x2774;
static constexpr size_t RKG_INPUT_DATA_SIZE =
        RKG_UNCOMPRESSED_INPUT_DATA_SECTION_SIZE - RKG_INPUT_DATA_HEADER_SIZE;
static constexpr size_t RKG_UNCOMPRESSED_FILE_SIZE =
        RKG_HEADER_SIZE + RKG_UNCOMPRESSED_INPUT_DATA_SECTION_SIZE;
static constexpr size_t RKG_USER_DATA_OFFSET = 0x20;
static constexpr size_t RKG_USER_DATA_SIZE = 0x14;
static constexpr size_t RKG_MII_DATA_OFFSET = 0x3C;
static constexpr size_t RKG_MII_DATA_SIZE = 0x4A;

class RawGhostFile {
public:
    RawGhostFile();
    RawGhostFile(const u8 *rkg);
    ~RawGhostFile();

    RawGhostFile &operator=(const u8 *rkg);

    void init(const u8 *rkg);
    bool decompress(const u8 *rkg);
    bool isValid(const u8 *rkg) const;

    const u8 *buffer() const;

    template <typename T>
    T parseAt(size_t offset) const;

private:
    bool compressed(const u8 *rkg) const;

    u8 m_buffer[RKG_UNCOMPRESSED_FILE_SIZE];
};
static_assert(sizeof(RawGhostFile) == RKG_UNCOMPRESSED_FILE_SIZE);

class GhostFile {
public:
    GhostFile(const RawGhostFile &raw);
    ~GhostFile();

    void read(EGG::RamStream &stream);

    Character character() const;
    Vehicle vehicle() const;
    Course course() const;
    const u8 *inputs() const;
    bool driftIsAuto() const;

private:
    std::array<wchar_t, 11> m_userData;
    std::array<u8, 76> m_miiData;
    u8 m_lapCount;
    std::array<Timer, 5> m_lapTimes;
    Timer m_raceTime;
    Character m_character;
    Vehicle m_vehicle;
    Course m_course;
    u32 m_controllerId;
    u8 m_year;
    u8 m_month;
    u8 m_day;
    u32 m_type;
    bool m_driftIsAuto;
    u32 m_location;
    u16 m_inputSize;
    const u8 *m_inputs;
};

} // namespace System
