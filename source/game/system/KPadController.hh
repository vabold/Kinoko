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

struct RaceInputState {
    RaceInputState();
    virtual ~RaceInputState() {}

    void reset();

    u16 buttons;
    u16 buttonsRaw;
    EGG::Vector2f stick;
    u8 stickXRaw;
    u8 stickYRaw;
    u8 trick;
    u8 trickRaw;
};

struct KPadGhostButtonsStream {
    KPadGhostButtonsStream();
    ~KPadGhostButtonsStream();

    virtual u16 readFrame();
    virtual bool readIsNewSequence(const u16 *sequence) const;
    virtual u16 readVal(const u16 *sequence) const;

    const u16 *buffer;
    s32 sequenceCount;
    u32 size;
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

    bool readIsNewSequence(const u16 *sequence) const override;
    u16 readVal(const u16 *sequence) const override;
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

    void readGhostBuffer(const u16 *buffer, bool driftIsAuto);

    void calcImpl() override;

    void setAcceptingInputs(bool set);

private:
    const u16 *m_ghostBuffer;
    std::array<KPadGhostButtonsStream *, 3> m_buttonsStreams;
    bool m_acceptingInputs;
};

class KPad {
public:
    KPad();
    ~KPad();

    void calc();
    void reset();

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

private:
    u8 m_ghostBuffer[RKG_UNCOMPRESSED_INPUT_DATA_SECTION_SIZE];
};

} // namespace System
