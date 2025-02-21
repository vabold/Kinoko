#include "host/KSystem.hh"

#include <egg/core/SceneManager.hh>

#include <game/system/RaceConfig.hh>

/// @brief Kinoko system designed to execute replays.
class KReplaySystem : public KSystem {
public:
    void init() override;
    void calc() override;
    bool run() override;
    void parseOptions(int argc, char **argv) override;

    static KReplaySystem *CreateInstance();
    static void DestroyInstance();

    static KReplaySystem *Instance() {
        return static_cast<KReplaySystem *>(s_instance);
    }

private:
    typedef std::pair<const System::Timer &, const System::Timer &> DesyncingTimerPair;

    KReplaySystem();
    KReplaySystem(const KReplaySystem &) = delete;
    KReplaySystem(KReplaySystem &&) = delete;
    ~KReplaySystem() override;

    bool calcEnd() const;
    void reportFail(const std::string &msg) const;

    bool success() const;
    s32 getDesyncingTimerIdx() const;
    DesyncingTimerPair getDesyncingTimer(s32 i) const;

    static void OnInit(System::RaceConfig *config, void *arg);

    EGG::SceneManager *m_sceneMgr;

    const char *m_currentGhostFileName;
    const System::GhostFile *m_currentGhost;
    const u8 *m_currentRawGhost;
    size_t m_currentRawGhostSize;
};
