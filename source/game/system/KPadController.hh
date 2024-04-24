#pragma once

#include "game/system/GhostFile.hh"

#include <egg/math/Vector.hh>

namespace System {

enum class ControlSource {
    Unknown = -1,
    Core = 0,      // WiiMote
    Freestyle = 1, // WiiMote + Nunchuk
    Classic = 2,
    Gamecube = 3,
    Ghost = 4,
    AI = 5,
};

enum class Trick {
    None = 0,
    Up = 1,
    Down = 2,
    Left = 3,
    Right = 4
};

struct RaceInputState {
    RaceInputState();
    virtual ~RaceInputState() {}

    void reset();

    bool accelerate() const;
    bool brake() const;
    bool item() const;
    bool drift() const;
    bool trickUp() const;

    u16 buttons;
    u16 buttonsRaw;
    EGG::Vector2f stick;
    u8 stickXRaw;
    u8 stickYRaw;
    Trick trick;
    u8 trickRaw;
};

struct KPadGhostButtonsStream {
    KPadGhostButtonsStream();
    ~KPadGhostButtonsStream();

    virtual u8 readFrame();
    virtual bool readIsNewSequence() const;
    virtual u8 readVal() const;

    EGG::RamStream buffer;
    u32 currentSequence;
    u16 readSequenceFrames;
    u32 state;
};

struct KPadGhostFaceButtonsStream : public KPadGhostButtonsStream {
    KPadGhostFaceButtonsStream();
    ~KPadGhostFaceButtonsStream();
};

struct KPadGhostDirectionButtonsStream : public KPadGhostButtonsStream {
    KPadGhostDirectionButtonsStream();
    ~KPadGhostDirectionButtonsStream();
};

struct KPadGhostTrickButtonsStream : public KPadGhostButtonsStream {
    KPadGhostTrickButtonsStream();
    ~KPadGhostTrickButtonsStream();

    bool readIsNewSequence() const override;
    u8 readVal() const override;
};

class KPadController {
public:
    KPadController();
    virtual ~KPadController() {}

    virtual ControlSource controlSource();
    virtual void reset(bool /*driftIsAuto*/) {}
    virtual void calcImpl() {}

    void calc();

    const RaceInputState &raceInputState() const;

    void setDriftIsAuto(bool driftIsAuto);

    bool driftIsAuto() const;

protected:
    RaceInputState m_raceInputState;
    bool m_connected;
    bool m_driftIsAuto;
};

class KPadGhostController : public KPadController {
public:
    KPadGhostController();
    ~KPadGhostController();

    ControlSource controlSource() override;
    void reset(bool driftIsAuto) override;

    void readGhostBuffer(const u8 *buffer, bool driftIsAuto);

    void calcImpl() override;

    void setAcceptingInputs(bool set);

private:
    const u8 *m_ghostBuffer;
    std::array<KPadGhostButtonsStream *, 3> m_buttonsStreams;
    bool m_acceptingInputs;
};

class KPad {
public:
    KPad();
    ~KPad();

    void calc();
    void reset();

    const RaceInputState &currentState() const;
    const RaceInputState &lastState() const;
    bool driftIsAuto() const;

protected:
    KPadController *m_controller;
    RaceInputState m_currentInputState;
    RaceInputState m_lastInputState;
};

class KPadPlayer : public KPad {
public:
    KPadPlayer();
    ~KPadPlayer();

    void setGhostController(KPadGhostController *controller, const u8 *inputs, bool driftIsAuto);

    void startGhostProxy();
    void endGhostProxy();

private:
    u8 m_ghostBuffer[RKG_UNCOMPRESSED_INPUT_DATA_SECTION_SIZE];
};

} // namespace System
