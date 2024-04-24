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
        stream->currentSequence = 0;
        stream->readSequenceFrames = 0;
        stream->state = 1;
    }

    m_acceptingInputs = false;
    m_connected = true;
}

void KPadGhostController::readGhostBuffer(const u8 *buffer, bool driftIsAuto) {
    constexpr u32 SEQUENCE_SIZE = 0x2;

    m_ghostBuffer = buffer;
    m_driftIsAuto = driftIsAuto;

    EGG::RamStream stream =
            EGG::RamStream(const_cast<u8 *>(buffer), RKG_UNCOMPRESSED_INPUT_DATA_SECTION_SIZE);

    u16 faceCount = stream.read_u16();
    u16 directionCount = stream.read_u16();
    u16 trickCount = stream.read_u16();

    stream.skip(2);

    m_buttonsStreams[0]->buffer = stream.split(faceCount * SEQUENCE_SIZE);
    m_buttonsStreams[1]->buffer = stream.split(directionCount * SEQUENCE_SIZE);
    m_buttonsStreams[2]->buffer = stream.split(trickCount * SEQUENCE_SIZE);
}

void KPadGhostController::calcImpl() {
    if (!m_ghostBuffer || !m_acceptingInputs) {
        return;
    }

    m_raceInputState.buttons = m_buttonsStreams[0]->readFrame();
    u8 sticks = m_buttonsStreams[1]->readFrame();
    m_raceInputState.stickXRaw = sticks >> 4 & 0xF;
    m_raceInputState.stickYRaw = sticks & 0xF;
    m_raceInputState.stick =
            EGG::Vector2f((static_cast<f32>(m_raceInputState.stickXRaw) - 7.0f) / 7.0f,
                    (static_cast<f32>(m_raceInputState.stickYRaw) - 7.0f) / 7.0f);
    m_raceInputState.trickRaw = m_buttonsStreams[2]->readFrame();

    switch (m_raceInputState.trickRaw >> 4) {
    case 1:
        m_raceInputState.trick = Trick::Up;
        break;
    case 2:
        m_raceInputState.trick = Trick::Down;
        break;
    case 3:
        m_raceInputState.trick = Trick::Left;
        break;
    case 4:
        m_raceInputState.trick = Trick::Right;
        break;
    default:
        m_raceInputState.trick = Trick::None;
        break;
    }
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
    trick = Trick::None;
    trickRaw = 0;
}

bool RaceInputState::accelerate() const {
    return !!(buttons & 0x1);
}

bool RaceInputState::brake() const {
    return !!(buttons & 0x2);
}

bool RaceInputState::item() const {
    return !!(buttons & 0x4);
}

bool RaceInputState::drift() const {
    return !!(buttons & 0x8);
}

bool RaceInputState::trickUp() const {
    return trick == Trick::Up;
}

KPadGhostButtonsStream::KPadGhostButtonsStream()
    : currentSequence(std::numeric_limits<u32>::max()), state(2) {}

KPadGhostButtonsStream::~KPadGhostButtonsStream() = default;

bool KPadGhostButtonsStream::readIsNewSequence() const {
    return readSequenceFrames >= (currentSequence & 0xFF);
}

u8 KPadGhostButtonsStream::readVal() const {
    return currentSequence >> 8;
}

u8 KPadGhostButtonsStream::readFrame() {
    if (state != 1) {
        return 0;
    }

    if (currentSequence == std::numeric_limits<u32>::max()) {
        readSequenceFrames = 0;
        currentSequence = buffer.read_u16();
    } else {
        if (readIsNewSequence()) {
            readSequenceFrames = 0;
            if (buffer.eof()) {
                state = 2;
                return 0;
            }
            currentSequence = buffer.read_u16();
        }
    }

    ++readSequenceFrames;

    return readVal();
}

KPadGhostFaceButtonsStream::KPadGhostFaceButtonsStream() = default;

KPadGhostFaceButtonsStream::~KPadGhostFaceButtonsStream() = default;

KPadGhostDirectionButtonsStream::KPadGhostDirectionButtonsStream() = default;

KPadGhostDirectionButtonsStream::~KPadGhostDirectionButtonsStream() = default;

KPadGhostTrickButtonsStream::KPadGhostTrickButtonsStream() = default;

KPadGhostTrickButtonsStream::~KPadGhostTrickButtonsStream() = default;

bool KPadGhostTrickButtonsStream::readIsNewSequence() const {
    u16 duration = currentSequence & 0xFF;
    duration += 256 * (currentSequence >> 8 & 0xF);
    return duration <= readSequenceFrames;
}

u8 KPadGhostTrickButtonsStream::readVal() const {
    return currentSequence >> 0x8 & ~0x80;
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

const RaceInputState &KPad::currentState() const {
    return m_currentInputState;
}

const RaceInputState &KPad::lastState() const {
    return m_lastInputState;
}

bool KPad::driftIsAuto() const {
    return m_controller->driftIsAuto();
}

KPadPlayer::KPadPlayer() = default;

KPadPlayer::~KPadPlayer() = default;

void KPadPlayer::setGhostController(KPadGhostController *controller, const u8 *inputs,
        bool driftIsAuto) {
    m_controller = controller;

    if (inputs) {
        memcpy(m_ghostBuffer, inputs, RKG_UNCOMPRESSED_INPUT_DATA_SECTION_SIZE);
    }

    controller->readGhostBuffer(m_ghostBuffer, driftIsAuto);
}

void KPadPlayer::startGhostProxy() {
    if (!m_controller || m_controller->controlSource() != ControlSource::Ghost) {
        return;
    }

    KPadGhostController *ghostController = reinterpret_cast<KPadGhostController *>(m_controller);
    ghostController->setAcceptingInputs(true);
}

void KPadPlayer::endGhostProxy() {
    if (!m_controller || m_controller->controlSource() != ControlSource::Ghost) {
        return;
    }

    KPadGhostController *ghostController = reinterpret_cast<KPadGhostController *>(m_controller);
    ghostController->setAcceptingInputs(false);
}

} // namespace System
