#pragma once

#include "game/system/GhostFile.hh"
#include "game/system/GhostWriter.hh"

#include <egg/math/Vector.hh>

namespace System {

/// @brief Converts a raw stick input into an input usable by the state.
/// @param rawStick The raw stick input to convert.
/// @return The converted input.
[[nodiscard]] static constexpr f32 RawStickToState(u8 rawStick) {
    return (static_cast<f32>(rawStick) - 7.0f) / 7.0f;
}

enum class ControlSource {
    Unknown = -1,
    Core = 0,      // WiiMote
    Freestyle = 1, // WiiMote + Nunchuk
    Classic = 2,
    Gamecube = 3,
    Ghost = 4,
    AI = 5,
    Host = 6, // Added in Kinoko, represents an external program
};

enum class Trick {
    None = 0,
    Up = 1,
    Down = 2,
    Left = 3,
    Right = 4,
};

/// @brief Represents a set of controller inputs.
struct RaceInputState {
    RaceInputState();
    virtual ~RaceInputState() {}

    void reset();

    [[nodiscard]] bool isValid() const;

    /// @brief Checks if there are any invalid buttons.
    /// @details Validatation with the previous input state doesn't happen because it doesn't exist.
    /// Therefore, we cannot check here if e.g. the drift button is pressed when it shouldn't be.
    /// @return If no invalid buttons are present.
    [[nodiscard]] bool isButtonsValid() const {
        return !(buttons & ~0xf);
    }

    [[nodiscard]] bool isStickValid(f32 stick) const;

    /// @brief Checks if the trick input is valid.
    /// @return If the trick input is valid.
    [[nodiscard]] bool isTrickValid() const {
        switch (trick) {
        case Trick::None:
        case Trick::Up:
        case Trick::Down:
        case Trick::Left:
        case Trick::Right:
            return true;
        default:
            return false;
        }
    }

    [[nodiscard]] bool accelerate() const {
        return !!(buttons & 0x1);
    }

    [[nodiscard]] bool brake() const {
        return !!(buttons & 0x2);
    }

    [[nodiscard]] bool item() const {
        return !!(buttons & 0x4);
    }

    [[nodiscard]] bool drift() const {
        return !!(buttons & 0x8);
    }

    [[nodiscard]] bool trickUp() const {
        return trick == Trick::Up;
    }

    [[nodiscard]] bool trickDown() const {
        return trick == Trick::Down;
    }

    u16 buttons;
    u16 buttonsRaw;
    EGG::Vector2f stick;
    u8 stickXRaw;
    u8 stickYRaw;
    Trick trick;
    u8 trickRaw;
};

/// @brief Represents a stream of button inputs from a ghost file.
struct KPadGhostButtonsStream {
    KPadGhostButtonsStream();
    ~KPadGhostButtonsStream();

    virtual void writeFrame(u8 data);
    [[nodiscard]] virtual u8 readFrame();

    /// @addr{0x80524F78} @addr{0x80524FE0}
    virtual void writeSequenceStart(u8 val) {
        // Increment the stream index so we can write to the next tuple
        buffer.skip(2);
        *buffer.dataAtIndex() = val;
        *(buffer.dataAtIndex() + 1) = 0;
    }

    /// @addr{0x80524F88}
    virtual void writeIncrementSequenceFrames() {
        ++*(buffer.dataAtIndex() + 1);
    }

    /// @addr{0x80524F98}
    [[nodiscard]] virtual bool writeIsNewSequence(u8 val) const {
        u8 bufferVal = *buffer.dataAtIndex();
        u8 bufferValLen = *(buffer.dataAtIndex() + 1);
        return bufferVal == val && bufferValLen != 0xFF;
    }

    /// @addr{0x8052502C} @addr{0x80524FC4}
    [[nodiscard]] virtual bool readIsNewSequence() const {
        return readSequenceFrames >= (sequenceCount & 0xFF);
    }

    /// @addr{0x80525024} @addr{0x80524FBC}
    [[nodiscard]] virtual u8 readVal() const {
        return sequenceCount >> 8;
    }

    EGG::RamStream buffer;
    s32 sequenceCount;
    u16 readSequenceFrames;
    u32 state;

    static constexpr u32 SEQUENCE_SIZE = 2;
};

/// @brief A specialized stream for button presses (not tricks).
/// @details Reads in the status for acceleration, braking, item usage, and drifting. The button
/// tuples take the following form:
/// Bitmask  | Description
///------------- | -------------
/// 0x01  | **Accelerating**
/// 0x02  | **Braking/Drifting**
/// 0x04  | **Item usage**
/// 0x08  | Set if braking/drifting pressed after pressing accelerating
/// @warning When bitmask 0x08 is set, the game will register a hop regardless of whether or
/// not the acceleration button is pressed. This can lead to "successful" synchronization of
/// ghosts which could not have been created legitimately in the first place.
struct KPadGhostFaceButtonsStream : public KPadGhostButtonsStream {
    KPadGhostFaceButtonsStream();
    ~KPadGhostFaceButtonsStream();
};

/// @brief A specialized stream for the analog stick.
/// Direction tuples take the following form:
/// Bitmask  | Description
///------------- | -------------
/// 0x0F  | Up/Down (0xE = Up, 0x0 = Down, 0x7 = Neutral)
/// 0xF0  | Left/Right (0xE0 = Right, 0x00 = Left, 0x70 = Neutral)
struct KPadGhostDirectionButtonsStream : public KPadGhostButtonsStream {
    KPadGhostDirectionButtonsStream();
    ~KPadGhostDirectionButtonsStream();
};

/// @brief A specialized stream for D-Pad inputs for tricking and wheeling.
/// Trick tuples take the following form:
/// Bitmask  | Description
///------------- | -------------
/// 0x0F  | The upper four bits of the tuple's duration, forming a 12-bit integer.
/// 0x70  | 0x00 = No trick, 0x10 = Up/Wheelie, 0x20 = Down, 0x30 = Left, 0x40 = Right
struct KPadGhostTrickButtonsStream : public KPadGhostButtonsStream {
    KPadGhostTrickButtonsStream();
    ~KPadGhostTrickButtonsStream();

    /// @addr{0x80525048}
    void writeSequenceStart(u8 val) override {
        *(buffer.dataAtIndex()) = val << 4;
    }

    /// @addr{0x8052505C}
    void writeIncrementSequenceFrames() override {
        u16 *tuple = reinterpret_cast<u16 *>(buffer.dataAtIndex());

        // Extract the duration component
        // TODO: Validate endianness is okay
        u16 duration = *tuple & 0xFFF;
        ++duration;
        *tuple = duration | (*tuple & 0xF000);
    }

    /// @addr{0x80525074}
    [[nodiscard]] bool writeIsNewSequence(u8 val) const override {
        const u16 *tuple = reinterpret_cast<const u16 *>(buffer.dataAtIndex());

        // TODO: Validate endianness is okay
        if ((*tuple >> 0xC) == val && (*tuple & 0xFFF) < 0xFFF) {
            return false;
        }

        return true;
    }

    /// @addr{0x805250A8}
    [[nodiscard]] bool readIsNewSequence() const override {
        u16 duration = sequenceCount & 0xFF;
        duration += 256 * (sequenceCount >> 8 & 0xF);
        return duration <= readSequenceFrames;
    }

    /// @addr{0x8052509C}
    [[nodiscard]] u8 readVal() const override {
        return sequenceCount >> 0x8 & ~0x80;
    }
};

/// @brief An abstraction for a controller object. It is associated with an input state.
class KPadController {
public:
    KPadController();
    virtual ~KPadController() {}

    /// @addr{0x8051CE7C}
    [[nodiscard]] virtual ControlSource controlSource() const {
        return ControlSource::Unknown;
    }

    virtual void reset(bool /*driftIsAuto*/) {}
    virtual void calcImpl() {}

    void calc();

    [[nodiscard]] const RaceInputState &raceInputState() const {
        return m_raceInputState;
    }

    /// @addr{0x8051F37C}
    void setDriftIsAuto(bool driftIsAuto) {
        m_driftIsAuto = driftIsAuto;
    }

    [[nodiscard]] bool driftIsAuto() const {
        return m_driftIsAuto;
    }

protected:
    RaceInputState m_raceInputState; ///< The current inputs from this controller.
    bool m_connected;                ///< Whether the controller is active.
    bool m_driftIsAuto;              ///< True for auto transmission, false for manual.
};

/// @brief The abstraction of a controller object but for ghost playback.
/// @details When playing back ghosts, their input state is managed by this class.
class KPadGhostController : public KPadController {
public:
    KPadGhostController();
    ~KPadGhostController() override;

    /// @addr{0x8052282C}
    [[nodiscard]] ControlSource controlSource() const override {
        return ControlSource::Ghost;
    }

    void reset(bool driftIsAuto) override;

    void readGhostBuffer(const u8 *buffer, bool driftIsAuto);

    void calcImpl() override;

    void setAcceptingInputs(bool set) {
        m_acceptingInputs = set;
    }

private:
    const u8 *m_ghostBuffer;
    std::array<KPadGhostButtonsStream *, 3> m_buttonsStreams;
    bool m_acceptingInputs;
};

/// @brief The abstraction of a controller object but for external usage.
/// @details The input state is managed externally by programs interfacing with Kinoko.
class KPadHostController : public KPadController {
public:
    KPadHostController();
    ~KPadHostController() override;

    [[nodiscard]] ControlSource controlSource() const override {
        return ControlSource::Host;
    }

    void reset(bool driftIsAuto) override;

    /// @brief Sets the inputs of the controller.
    /// @param state The specified inputs packaged in the state. Only buttons, stick, and trick
    /// matter.
    /// @return Input state validity.
    bool setInputs(const RaceInputState &state) {
        return setInputs(state.buttons, state.stick, state.trick);
    }

    /// @brief Sets the inputs of the controller.
    /// @param buttons The button inputs.
    /// @param stick The stick inputs, as a 2D vector.
    /// @param trick The trick input.
    /// @return Input state validity.
    bool setInputs(u16 buttons, const EGG::Vector2f &stick, Trick trick) {
        m_raceInputState.buttons = buttons;
        m_raceInputState.stick = stick;
        m_raceInputState.trick = trick;

        return m_raceInputState.isValid();
    }

    /// @brief Sets the inputs of the controller.
    /// @param buttons The button inputs.
    /// @param stickX The stick input on the X axis.
    /// @param stickY The stick input on the Y axis.
    /// @param trick The trick input.
    /// @return Input state validity.
    bool setInputs(u16 buttons, f32 stickX, f32 stickY, Trick trick) {
        m_raceInputState.buttons = buttons;
        m_raceInputState.stick.x = stickX;
        m_raceInputState.stick.y = stickY;
        m_raceInputState.trick = trick;

        return m_raceInputState.isValid();
    }

    /// @brief Sets the inputs of the controller.
    /// @details A different name is specified to avoid any ambiguity with the parameters.
    /// @param buttons The button inputs.
    /// @param stickXRaw The 7-centered raw stick input on the X axis.
    /// @param stickYRaw The 7-centered raw stick input on the Y axis.
    /// @param trick The trick input.
    /// @return Input state validity.
    bool setInputsRawStick(u16 buttons, u8 stickXRaw, u8 stickYRaw, Trick trick) {
        return setInputs(buttons, RawStickToState(stickXRaw), RawStickToState(stickYRaw), trick);
    }

    /// @brief Sets the inputs of the controller.
    /// @details A different name is specified to avoid any ambiguity with the parameters.
    /// @param buttons The button inputs.
    /// @param stickXRaw The 0-centered raw stick input on the X axis.
    /// @param stickYRaw The 0-centered raw stick input on the Y axis.
    /// @param trick The trick input.
    /// @return Input state validity.
    bool setInputsRawStickZeroCenter(u16 buttons, s8 stickXRaw, s8 stickYRaw, Trick trick) {
        return setInputsRawStick(buttons, stickXRaw + 7, stickYRaw + 7, trick);
    }
};

class KPad {
public:
    KPad();
    ~KPad();

    virtual void calc();
    void reset();

    [[nodiscard]] const RaceInputState &currentState() const {
        return m_currentInputState;
    }

    [[nodiscard]] const RaceInputState &lastState() const {
        return m_lastInputState;
    }

    [[nodiscard]] bool driftIsAuto() const {
        return m_controller->driftIsAuto();
    }

protected:
    KPadController *m_controller;
    RaceInputState m_currentInputState;
    RaceInputState m_lastInputState; ///< Used to determine changes in input state.
};

/// @brief A specialized KPad for player input, as opposed to CPU players for example.
class KPadPlayer : public KPad {
public:
    KPadPlayer();
    ~KPadPlayer();

    void calc() override;

    void end();

    void setGhostController(KPadGhostController *controller, const u8 *inputs, bool driftIsAuto);
    void setHostController(KPadHostController *controller, bool driftIsAuto);

    void startGhostProxy(); ///< Signals to start reading ghost data after fade-in.
    void endGhostProxy();   ///< Signals to stop reading ghost data after race completion.

private:
    u8 m_ghostBuffer[RKG_UNCOMPRESSED_INPUT_DATA_SECTION_SIZE];
    GhostWriter m_ghostWriter;
};

} // namespace System
