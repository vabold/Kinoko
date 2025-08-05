#include "GhostWriter.hh"

#include "game/system/KPadController.hh"

#include <cstring>

namespace System {

/// @addr{0x80524AC4}
GhostWriter::GhostWriter() {
    m_buffer = std::span<u8>(new u8[TRIPLE_RKG_BUFFER_SIZE], TRIPLE_RKG_BUFFER_SIZE);

    m_buttonsStreams[0] = new KPadGhostFaceButtonsStream;
    m_buttonsStreams[1] = new KPadGhostDirectionButtonsStream;
    m_buttonsStreams[2] = new KPadGhostTrickButtonsStream;

    m_buttonsStreams[0]->buffer = EGG::RamStream(m_buffer.data(), RKG_BUFFER_SIZE);
    m_buttonsStreams[1]->buffer =
            EGG::RamStream(m_buffer.data() + RKG_BUFFER_SIZE, RKG_BUFFER_SIZE);
    m_buttonsStreams[2]->buffer =
            EGG::RamStream(m_buffer.data() + 2 * RKG_BUFFER_SIZE, RKG_BUFFER_SIZE);

    reset();
}

/// @addr{0x80524C2C}
GhostWriter::~GhostWriter() {
    delete[] m_buffer.data();
    delete m_buttonsStreams[0];
    delete m_buttonsStreams[1];
    delete m_buttonsStreams[2];
}

/// @addr{0x80524CA8}
void GhostWriter::reset() {
    memset(m_buffer.data(), 0, RKG_BUFFER_SIZE);

    m_buttonsStreams[0]->sequenceCount = -1;
    m_buttonsStreams[0]->state = 0;

    m_buttonsStreams[1]->sequenceCount = -1;
    m_buttonsStreams[1]->state = 0;

    m_buttonsStreams[2]->sequenceCount = -1;
    m_buttonsStreams[2]->state = 0;

    m_state = State::Inactive;
}

/// @addr{0x80524D20}
/// @param buffer The buffer to write bytes to
/// @return Number of bytes writtern
void GhostWriter::saveToBuffer(EGG::RamStream &outStream) {
    memset(outStream.data(), 0, 8);

    for (auto *&stream : m_buttonsStreams) {
        if (stream->sequenceCount < 0) {
            return;
        }

        u32 count = stream->sequenceCount + 1;
        outStream.write(&count, sizeof(u32));
        outStream.write(stream->buffer.data(), count * KPadGhostButtonsStream::SEQUENCE_SIZE);
    }
}

/// @addr{0x80524E74}
void GhostWriter::writeFrame(u8 buttons, u8 stickX, u8 stickY, u8 trick) {
    if (m_state != State::Active) {
        return;
    }

    m_buttonsStreams[0]->writeFrame(buttons);
    m_buttonsStreams[1]->writeFrame((stickX << 4) | stickY);
    m_buttonsStreams[2]->writeFrame(trick);

    size_t bufferedSize = 0;

    for (auto *&stream : m_buttonsStreams) {
        if (stream->sequenceCount < 0) {
            m_state = State::ExceededBufferSize;
            continue;
        }

        u32 count = stream->sequenceCount + 1;
        bufferedSize += count * KPadGhostButtonsStream::SEQUENCE_SIZE;
    }

    if (bufferedSize + 6 >= RKG_BUFFER_SIZE) {
        WARN("GHOST DATA CANNOT BE SAVED (Input limit reached)");
        m_state = State::ExceededBufferSize;
    }
}

} // namespace System
