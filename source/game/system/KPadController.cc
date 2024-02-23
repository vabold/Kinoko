#include "KPadController.hh"

#include <cstring>

namespace System {

KPadController::KPadController() : m_connected(false) {}

ControlSource KPadController::controlSource() {
    return ControlSource::Unknown;
}

void KPadController::calc() {
    calcImpl();
}

const RaceInputState &KPadController::raceInputState() const {
    return m_raceInputState;
}

void KPadController::setDriftIsAuto(bool driftIsAuto) {
    m_driftIsAuto = driftIsAuto;
}

bool KPadController::driftIsAuto() const {
    return m_driftIsAuto;
}

KPadGhostController::KPadGhostController() : m_acceptingInputs(false) {
    m_buttonsStreams[0] = new KPadGhostFaceButtonsStream;
    m_buttonsStreams[1] = new KPadGhostDirectionButtonsStream;
    m_buttonsStreams[2] = new KPadGhostTrickButtonsStream;
}

KPadGhostController::~KPadGhostController() = default;

ControlSource KPadGhostController::controlSource() {
    return ControlSource::Ghost;
}

void KPadGhostController::reset(bool driftIsAuto) {
    m_driftIsAuto = driftIsAuto;
    m_raceInputState.reset();

    for (auto &stream : m_buttonsStreams) {
        stream->sequenceCount = -1;
        stream->readSequenceFrames = 0;
        stream->state = 1;
    }

    m_acceptingInputs = false;
    m_connected = true;
}

void KPadGhostController::readGhostBuffer(const u16 *buffer, bool driftIsAuto) {
    constexpr u32 SEQUENCE_SIZE = 0x2;

    m_ghostBuffer = buffer;
    m_driftIsAuto = driftIsAuto;

    u16 faceCount = parse<u16>(*buffer);
    u16 directionCount = parse<u16>(*(buffer + 1));
    u16 trickCount = parse<u16>(*(buffer + 2));

    buffer += 0x4;

    m_buttonsStreams[0]->buffer = buffer;
    m_buttonsStreams[0]->size = faceCount * SEQUENCE_SIZE;
    buffer += faceCount;

    m_buttonsStreams[1]->buffer = buffer;
    m_buttonsStreams[1]->size = directionCount * SEQUENCE_SIZE;
    buffer += directionCount;

    m_buttonsStreams[2]->buffer = buffer;
    m_buttonsStreams[2]->size = trickCount * SEQUENCE_SIZE;
}

void KPadGhostController::calcImpl() {
    if (!m_ghostBuffer || !m_acceptingInputs) {
        return;
    }

    m_raceInputState.buttons = m_buttonsStreams[0]->readFrame() & 0xFF;
    u16 sticks = m_buttonsStreams[1]->readFrame();
    m_raceInputState.stickXRaw = sticks >> 4 & 0xF;
    m_raceInputState.stickYRaw = sticks & 0xF;
    m_raceInputState.stick =
            EGG::Vector2f((static_cast<f32>(m_raceInputState.stickXRaw) - 7.0f) / 7.0f,
                    (static_cast<f32>(m_raceInputState.stickYRaw) - 7.0f) / 7.0f);
    m_raceInputState.trickRaw = m_buttonsStreams[2]->readFrame() >> 0x4;
    m_raceInputState.trick = m_raceInputState.trickRaw;
}

void KPadGhostController::setAcceptingInputs(bool set) {
    m_acceptingInputs = set;
}

RaceInputState::RaceInputState() {
    reset();
}

void RaceInputState::reset() {
    buttons = 0;
    buttonsRaw = 0;
    stick = EGG::Vector2f::zero;
    stickXRaw = 7;
    stickYRaw = 7;
    trick = 0;
    trickRaw = 0;
}

KPadGhostButtonsStream::KPadGhostButtonsStream()
    : buffer(nullptr), sequenceCount(-1), size(RKG_INPUT_DATA_SIZE), state(2) {}

KPadGhostButtonsStream::~KPadGhostButtonsStream() = default;

bool KPadGhostButtonsStream::readIsNewSequence(const u16 *sequence) const {
    return readSequenceFrames >= (*sequence >> 0x8);
}

u16 KPadGhostButtonsStream::readVal(const u16 *sequence) const {
    return *sequence;
}

u16 KPadGhostButtonsStream::readFrame() {
    constexpr u32 SEQUENCE_SIZE = 0x2;

    if (state != 1) {
        return 0;
    }

    bool bVar1 = false;
    const u16 *sequence = nullptr;

    if (sequenceCount >= 0) {
        sequence = buffer + sequenceCount;

        if (readIsNewSequence(sequence)) {
            bVar1 = true;
            if (size <= ++sequenceCount * SEQUENCE_SIZE) {
                state = 2;
            }
        }
    } else {
        sequenceCount = 0;
        bVar1 = true;
    }

    if (state != 1) {
        return 0;
    }

    sequence = buffer + sequenceCount;

    if (bVar1) {
        readSequenceFrames = 0;
    }

    ++readSequenceFrames;

    return readVal(sequence);
}

KPadGhostFaceButtonsStream::KPadGhostFaceButtonsStream() = default;

KPadGhostFaceButtonsStream::~KPadGhostFaceButtonsStream() = default;

KPadGhostDirectionButtonsStream::KPadGhostDirectionButtonsStream() = default;

KPadGhostDirectionButtonsStream::~KPadGhostDirectionButtonsStream() = default;

KPadGhostTrickButtonsStream::KPadGhostTrickButtonsStream() = default;

KPadGhostTrickButtonsStream::~KPadGhostTrickButtonsStream() = default;

bool KPadGhostTrickButtonsStream::readIsNewSequence(const u16 *sequence) const {
    return (*sequence & 0xFFF) <= readSequenceFrames;
}

u16 KPadGhostTrickButtonsStream::readVal(const u16 *sequence) const {
    return *sequence >> 0xC;
}

KPad::KPad() : m_controller(nullptr) {
    reset();
}

KPad::~KPad() = default;

void KPad::calc() {
    m_lastInputState = m_currentInputState;
    m_currentInputState = m_controller->raceInputState();
}

void KPad::reset() {
    if (m_controller) {
        m_controller->reset(m_controller->driftIsAuto());
    }
}

KPadPlayer::KPadPlayer() = default;

KPadPlayer::~KPadPlayer() = default;

void KPadPlayer::setGhostController(KPadGhostController *controller, const u8 *inputs,
        bool driftIsAuto) {
    m_controller = controller;

    if (inputs) {
        memcpy(m_ghostBuffer, inputs, RKG_UNCOMPRESSED_INPUT_DATA_SECTION_SIZE);
    }

    controller->readGhostBuffer(reinterpret_cast<const u16 *>(m_ghostBuffer), driftIsAuto);
}

void KPadPlayer::startGhostProxy() {
    if (!m_controller || m_controller->controlSource() != ControlSource::Ghost) {
        return;
    }

    KPadGhostController *ghostController = reinterpret_cast<KPadGhostController *>(m_controller);
    ghostController->setAcceptingInputs(true);
}

} // namespace System
