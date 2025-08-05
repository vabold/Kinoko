#pragma once

#include "Common.hh"

#include <egg/util/Stream.hh>

#include <span>

namespace System {

struct KPadGhostButtonsStream;

class GhostWriter {
public:
    static constexpr size_t RKG_BUFFER_SIZE = 0x276C;
    static constexpr size_t TRIPLE_RKG_BUFFER_SIZE = RKG_BUFFER_SIZE * 3;

    enum class State {
        Uninitialized = 0,
        Active = 1,
        Inactive = 2,
        ExceededBufferSize = 3, ///< Set when we run out of buffer space
    };

    GhostWriter();
    ~GhostWriter();

    void reset();
    void saveToBuffer(EGG::RamStream &stream);

    void writeFrame(u8 buttons, u8 stickX, u8 stickY, u8 trick);

    void setState(State state) {
        m_state = state;
    }

    State state() const {
        return m_state;
    }

private:
    std::array<KPadGhostButtonsStream *, 3> m_buttonsStreams;
    std::span<u8> m_buffer;
    State m_state;

    static constexpr const char *FILENAME = "KinokoInput.sav";
};

} // namespace System
