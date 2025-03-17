#include "KPadController.hh"

#include <cstring>

namespace System {

/// @addr{0x8051EBA8}
KPadController::KPadController() : m_connected(false) {}

/// @addr{0x8051ED14}
void KPadController::calc() {
    calcImpl();
}

/// @addr{0x80520730}
KPadGhostController::KPadGhostController() : m_acceptingInputs(false) {
    m_buttonsStreams[0] = new KPadGhostFaceButtonsStream;
    m_buttonsStreams[1] = new KPadGhostDirectionButtonsStream;
    m_buttonsStreams[2] = new KPadGhostTrickButtonsStream;
}

/// @addr{0x80520924}
KPadGhostController::~KPadGhostController() = default;

/// @addr{0x80520998}
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

/// @brief Reads in the raw input data section from the ghost RKG file.
/// @addr{Inlined in 0x80521844}
/// @details The buffer is split into three sections: face buttons, analog stick, and the D-Pad.
/// Each section is an array of tuples, where each tuple contains the input state and the duration
/// of that input state. This is used to minimize data consumption given that the user is not
/// changing inputs every frame. We first read in the header of the RKG input data section as
/// follows:
/// Offset  | Size | Description
///------------- | ------------- | -------------
/// 0x00  | 2 bytes | Count of face button input tuples
/// 0x02  | 2 bytes | Count of analog stick input tuples
/// 0x04  | 2 bytes | Count of D-Pad input tuples
/// 0x06  | 2 bytes | Unknown. Probably padding.
/// 0x08  | | End of header, beginning of face button input data.
void KPadGhostController::readGhostBuffer(const u8 *buffer, bool driftIsAuto) {
    constexpr u32 SEQUENCE_SIZE = 0x2;

    m_ghostBuffer = buffer;
    m_driftIsAuto = driftIsAuto;

    EGG::RamStream stream = EGG::RamStream(buffer, RKG_UNCOMPRESSED_INPUT_DATA_SECTION_SIZE);

    u16 faceCount = stream.read_u16();
    u16 directionCount = stream.read_u16();
    u16 trickCount = stream.read_u16();

    stream.skip(2);

    m_buttonsStreams[0]->buffer = stream.split(faceCount * SEQUENCE_SIZE);
    m_buttonsStreams[1]->buffer = stream.split(directionCount * SEQUENCE_SIZE);
    m_buttonsStreams[2]->buffer = stream.split(trickCount * SEQUENCE_SIZE);
}

/// @addr{0x80520B9C}
void KPadGhostController::calcImpl() {
    if (!m_ghostBuffer || !m_acceptingInputs) {
        return;
    }

    m_raceInputState.buttons = m_buttonsStreams[0]->readFrame();
    u8 sticks = m_buttonsStreams[1]->readFrame();
    m_raceInputState.stickXRaw = sticks >> 4 & 0xF;
    m_raceInputState.stickYRaw = sticks & 0xF;
    m_raceInputState.stick = EGG::Vector2f(RawStickToState(m_raceInputState.stickXRaw),
            RawStickToState(m_raceInputState.stickYRaw));
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

RaceInputState::RaceInputState() {
    reset();
}

/// @addr{0x8051E85C}
void RaceInputState::reset() {
    buttons = 0;
    buttonsRaw = 0;
    stick = EGG::Vector2f::zero;
    stickXRaw = 7;
    stickYRaw = 7;
    trick = Trick::None;
    trickRaw = 0;
}

/// @brief Checks if the input state is valid.
/// @return If the input state is valid.
bool RaceInputState::isValid() const {
    if (!isButtonsValid()) {
        return false;
    }

    if (!isStickValid(stick.x) || !isStickValid(stick.y)) {
        return false;
    }

    if (!isTrickValid()) {
        return false;
    }

    return true;
}

/// @brief Checks if the stick values are within the domain of the physics engine.
/// @details The set of valid stick values is \f$\{\frac{x-7}{7}|0\leq x\leq 14,\in\mathbb{Z}\}\f$.
/// It's possible for the stick input to be 8/7 with x = 15, but only with ghost controllers.
/// @return If the stick values are valid.
bool RaceInputState::isStickValid(f32 stick) const {
    if (stick > 1.0f || stick < -1.0f) {
        return false;
    }

    for (size_t i = 0; i <= 14; ++i) {
        auto cond = stick <=> RawStickToState(i);
        ASSERT(cond != std::partial_ordering::unordered);

        if (cond == std::partial_ordering::equivalent) {
            return true;
        } else if (cond == std::partial_ordering::less) {
            return false;
        }
    }

    // This is unreachable
    return false;
}

KPadGhostButtonsStream::KPadGhostButtonsStream()
    : currentSequence(std::numeric_limits<u32>::max()), state(2) {}

KPadGhostButtonsStream::~KPadGhostButtonsStream() = default;

/// @brief Reads the data from the corresponding tuple in the buffer.
/// @addr{0x80520D4C} @addr{0x80522C5C} @addr{0x80522F40}
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
            currentSequence = buffer.read_u16();
        }
    }

    ++readSequenceFrames;

    // In the base game, this check normally occurs before a new sequence is read. As a result, the
    // base game does not know that it has run out of inputs until the frame that it tries to access
    // past the last valid input. We stray from this behavior so that we can know when we are on the
    // last frame of input.
    if (buffer.eof() && readIsNewSequence()) {
        state = 2;
    }

    return readVal();
}

KPadGhostFaceButtonsStream::KPadGhostFaceButtonsStream() = default;

KPadGhostFaceButtonsStream::~KPadGhostFaceButtonsStream() = default;

KPadGhostDirectionButtonsStream::KPadGhostDirectionButtonsStream() = default;

KPadGhostDirectionButtonsStream::~KPadGhostDirectionButtonsStream() = default;

KPadGhostTrickButtonsStream::KPadGhostTrickButtonsStream() = default;

KPadGhostTrickButtonsStream::~KPadGhostTrickButtonsStream() = default;

/* ================================ *
 *     HOST CONTROLLER
 * ================================ */

KPadHostController::KPadHostController() = default;

KPadHostController::~KPadHostController() = default;

void KPadHostController::reset(bool driftIsAuto) {
    m_driftIsAuto = driftIsAuto;
    m_raceInputState.reset();
    m_connected = true;
}

/* ================================ *
 *     PADS
 * ================================ */

/// @addr{0x80520F64}
KPad::KPad() : m_controller(nullptr) {
    reset();
}

/// @addr{0x805222B4}
KPad::~KPad() = default;

/// @addr{0x80521198}
void KPad::calc() {
    m_lastInputState = m_currentInputState;
    m_currentInputState = m_controller->raceInputState();
}

/// @addr{0x80521110}
void KPad::reset() {
    if (m_controller) {
        m_controller->reset(m_controller->driftIsAuto());
    }
}

/// @addr{0x805220BC}
KPadPlayer::KPadPlayer() = default;

/// @addr{0x805222F4}
KPadPlayer::~KPadPlayer() = default;

/// @addr{0x80521844}
void KPadPlayer::setGhostController(KPadGhostController *controller, const u8 *inputs,
        bool driftIsAuto) {
    m_controller = controller;

    if (inputs) {
        memcpy(m_ghostBuffer, inputs, RKG_UNCOMPRESSED_INPUT_DATA_SECTION_SIZE);
    }

    controller->readGhostBuffer(m_ghostBuffer, driftIsAuto);
}

void KPadPlayer::setHostController(KPadHostController *controller, bool driftIsAuto) {
    m_controller = controller;
    m_controller->setDriftIsAuto(driftIsAuto);
}

/// @addr{0x805215D4}
void KPadPlayer::startGhostProxy() {
    if (!m_controller || m_controller->controlSource() != ControlSource::Ghost) {
        return;
    }

    KPadGhostController *ghostController = reinterpret_cast<KPadGhostController *>(m_controller);
    ghostController->setAcceptingInputs(true);
}

/// @addr{0x80521688}
void KPadPlayer::endGhostProxy() {
    if (!m_controller || m_controller->controlSource() != ControlSource::Ghost) {
        return;
    }

    KPadGhostController *ghostController = reinterpret_cast<KPadGhostController *>(m_controller);
    ghostController->setAcceptingInputs(false);
}

} // namespace System
