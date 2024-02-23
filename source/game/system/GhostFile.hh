#pragma once

#include "Common.hh"

#include <array>

namespace System {

struct Timer {
    Timer();
    Timer(u32 data);
    ~Timer();

    u16 min;
    u8 sec;
    u16 mil;
};

class RawGhostFile {
public:
    RawGhostFile(const u8 *rkg);
    ~RawGhostFile();

    bool decompress(const u8 *rkg);
    bool isValid(const u8 *rkg) const;

    const u8 *buffer() const;

    template <typename T>
    T parseAt(size_t offset) const;

private:
    bool compressed(const u8 *rkg) const;

    u8 m_buffer[0x27FC];
};
static_assert(sizeof(RawGhostFile) == 0x27FC);

class GhostFile {
public:
    GhostFile(RawGhostFile *raw);
    ~GhostFile();

    void read(RawGhostFile *raw);
    void readHeader(RawGhostFile *raw);

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
