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

/// @brief C++ implementation of the CTGP ghost footer, as documented by MrBean and Chadderz.
/// @details This footer was not created with C/C++ interfacing in mind.
/// In practice, the goal for the footer was to generate load instructions with negative offsets,
/// relative to the end of the file.
struct __attribute__((packed)) CTGPGhostFooter {
    u8 trackSHA1[20];
    u64 ghostDBPlayerID;
    f32 trueFinishTime;
    u8 _20[0x27 - 0x20];
    u8 regionLetter;
    u8 _28[0x38 - 0x28];
    std::array<f32, 10> trueLapTimes; ///< Indexed via 10 - i, such that i is one-indexed.
    u64 rtcTimeEnd;
    u64 rtcTimeStart;
    u64 rtcTimePaused;
    u8 ghostConsoleFlags;
    u8 shroom3Lap;
    u8 shroom2Lap;
    u8 shroom1Lap;
    u8 shortcutDefinitionVer;
    u8 ghostActionFlags;
    u8 tasCategory : 4; ///< Values 4-6 determine if a ghost is 200cc, if the category is 3.
    u8 category : 4;    ///< Value 3 determines TAS. Values 4-7 determine if a ghost is 200cc.
    u8 footerVersion;
    u32 footerLen;
    u32 magic;
    u32 checksum;

    static constexpr u32 CTGP_FOOTER_SIGNATURE = 0x434b4744; // CKGD
};
STATIC_ASSERT(sizeof(CTGPGhostFooter) == 0x8c);

struct CTGPMetadata {
    CTGPMetadata();

    void read(const CTGPGhostFooter *data);
    void read(EGG::RamStream &stream);

    bool m_isCTGP;
    bool m_is200cc;
};

/// @brief The binary data of a ghost saved to a file.
/// Offset  | Size | Description
///------------- | ------------- | -------------
/// 0x00  | 4 bytes | **RKGD** in ASCII.
/// 0x04  | 7 bits | **Minutes** field of finishing time
/// 0x04.7  | 7 bits | **Seconds** field of finishing time
/// 0x05.6  | 10 bits | **Milliseconds** field of finishing time
/// 0x07  | 6 bits | **Track ID**
/// 0x07.6  | 2 bits | **Unknown.** Probably padding.
/// 0x08 | 6 bits | **Vehicle ID**
/// 0x08.6 | 6 bits | **Character ID**
/// 0x09.4 | 7 bits | **Year** relative to 2000
/// 0x0A.3 | 4 bits | **Month**
/// 0x0A.7 | 5 bits | **Day**
/// 0x0B.4 | 4 bits | **Controller ID**
/// 0x0C | 4 bits | **Unknown.** Always 0?
/// 0x0C.4 | 1 bit | **Compressed** flag (1 for compressed, 0 for raw)
/// 0x0C.5 | 2 bits | **Unknown.** Always 0?
/// 0x0C.7 | 7 bits | **Ghost type**
/// 0x0D.6 | 1 bit | **Drift type** (1 for auto, 0 for manual)
/// 0x0E | 2 bytes | **Input data length** (measured after decompression)
/// 0x10 | 1 byte | **Lap count**
/// 0x11 | 5 x 3 bytes | Lap **split** times. Stored in the same 7-7-10 format as the finish time.
/// 0x20 | 0x14 bytes | **Unknown**
/// 0x34 | 1 byte | **Country code** or 0xFF if sharing location disabled.
/// 0x35 | 1 byte | **Status code** or 0xFF if sharing location disabled.
/// 0x36 | 2 bytes | **Location code** or 0xFFFF if sharing location disabled.
/// 0x38 | 4 bytes | **Unknown**
/// 0x3C | 0x4A bytes | **Mii data**
/// 0x86 | 2 bytes | **CRC-16-CCITT-XModem** of Mii data
/// Source: https://wiki.tockdom.com/wiki/RKG_(File_Format)
class RawGhostFile {
public:
    RawGhostFile();
    RawGhostFile(const u8 *rkg);
    ~RawGhostFile();

    RawGhostFile &operator=(const u8 *rkg);

    void init(const u8 *rkg);
    [[nodiscard]] bool decompress(const u8 *rkg);
    [[nodiscard]] bool isValid(const u8 *rkg) const;

    [[nodiscard]] const u8 *buffer() const;

    template <typename T>
    [[nodiscard]] T parseAt(size_t offset) const;

    [[nodiscard]] static const CTGPGhostFooter *FindCTGPFooter(const u8 *rkg, size_t size);

private:
    [[nodiscard]] static bool compressed(const u8 *rkg);

    u8 m_buffer[RKG_UNCOMPRESSED_FILE_SIZE];
};
STATIC_ASSERT(sizeof(RawGhostFile) == RKG_UNCOMPRESSED_FILE_SIZE);

/// @brief Parsed representation of a binary ghost file.
/// @nosubgrouping
class GhostFile {
public:
    GhostFile(const RawGhostFile &raw);
    ~GhostFile();

    void read(EGG::RamStream &stream); ///< Organizes binary data into members. See RawGhostFile.

    /// @beginGetters
    [[nodiscard]] Character character() const;
    [[nodiscard]] Vehicle vehicle() const;
    [[nodiscard]] Course course() const;
    [[nodiscard]] const u8 *inputs() const;
    [[nodiscard]] bool driftIsAuto() const;
    /// @endGetters

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
    u8 m_year; ///< The year, relative to 2000
    u8 m_month;
    u8 m_day;
    u32 m_type;         ///< The type of ghost
    bool m_driftIsAuto; ///< True for automatic, false for manual
    u32 m_location;     ///< 0xFFFF if sharing disabled
    u16 m_inputSize;    ///< The size of the decompressed input data section
    const u8 *m_inputs;
};

} // namespace System
