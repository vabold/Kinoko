#include "KReplaySystem.hh"

#include "host/Option.hh"
#include "host/SceneCreatorDynamic.hh"

#include <abstract/File.hh>

#include <game/system/RaceManager.hh>

#include <iomanip>

/// @brief Initializes the system.
void KReplaySystem::init() {
    auto *sceneCreator = new Host::SceneCreatorDynamic;
    m_sceneMgr = new EGG::SceneManager(sceneCreator);

    System::RaceConfig::RegisterInitCallback(OnInit, nullptr);
    Abstract::File::Remove("results.txt");
}

/// @brief Executes a frame.
void KReplaySystem::calc() {
    m_sceneMgr->calc();
}

/// @brief Executes a run.
/// @details A run consists of replaying a ghost.
/// @return Whether the run was successful or not.
bool KReplaySystem::run() {
    bool success = true;

    while (true) {
        if (m_ghostArgs.size() == 0) {
            break;
        }

        std::filesystem::path nextPath = m_ghostArgs.front();
        m_ghostArgs.pop();

        if (std::filesystem::is_directory(nextPath)) {
            success &= runDirectory(nextPath);
        } else if (std::filesystem::is_regular_file(nextPath)) {
            success &= runGhost(nextPath);
        } else {
            WARN("Not a valid path: %s", nextPath.string().c_str());
            continue;
        }
    }

    return success;
}

/// @brief Parses non-generic command line options.
/// @details The only currently accepted option is the ghost flag.
/// @param argc The number of arguments.
/// @param argv The arguments.
void KReplaySystem::parseOptions(int argc, char **argv) {
    if (argc < 2) {
        PANIC("Expected ghost argument!");
    }

    for (int i = 0; i < argc; ++i) {
        std::optional<Host::EOption> flag = Host::Option::CheckFlag(argv[i]);
        if (!flag || *flag == Host::EOption::Invalid) {
            WARN("Expected a flag! Got: %s", argv[i]);
            continue;
        }

        switch (*flag) {
        case Host::EOption::Ghost: {
            ASSERT(i + 1 < argc);

            // Every argument (until we see another flag) should be a ghost filename or directory
            while (++i < argc) {
                // Decrement i if it represents some other flag
                if (Host::Option::CheckFlag(argv[i])) {
                    --i;
                    break;
                }

                std::filesystem::path filepath = Abstract::File::Path(argv[i]);

                if (std::filesystem::is_directory(filepath) ||
                        std::filesystem::is_regular_file(filepath)) {
                    m_ghostArgs.push(filepath);
                } else {
                    WARN("Unable to find %s. Skipping...", filepath.c_str());
                }
            }
        } break;
        case Host::EOption::Progress: {
            if (i + 1 >= argc) {
                PANIC("Expected progress interval argument!");
            }

            m_progressInterval = strtoul(argv[++i], nullptr, 0);
        } break;
        case Host::EOption::Invalid:
        default:
            PANIC("Invalid flag!");
            break;
        }
    }
}

KReplaySystem *KReplaySystem::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new KReplaySystem;
    return static_cast<KReplaySystem *>(s_instance);
}

void KReplaySystem::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

KReplaySystem::KReplaySystem()
    : m_progressInterval(0), m_replaysPlayed(0), m_replaysSynced(0), m_currentGhost(nullptr),
      m_currentRawGhost(nullptr), m_currentRawGhostSize(0) {}

KReplaySystem::~KReplaySystem() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("KReplaySystem instance not explicitly handled!");
    }

    delete m_sceneMgr;
    delete m_currentGhost;
    delete m_currentRawGhost;
}

/// @brief Determines whether or not the ghost simulation should end.
/// @return Whether the ghost should end or not.
bool KReplaySystem::calcEnd() const {
    constexpr u16 MAX_MINUTE_COUNT = 10;

    const auto *raceManager = System::RaceManager::Instance();
    if (raceManager->stage() == System::RaceManager::Stage::FinishGlobal) {
        return true;
    }

    if (raceManager->timerManager().currentTimer().min >= MAX_MINUTE_COUNT) {
        return true;
    }

    return false;
}

/// @brief Reports failure to file.
/// @param msg The message to report.
void KReplaySystem::reportFail(const std::string &msg) const {
    std::string report(m_currentGhostPath.string());
    report += "\n" + std::string(msg);
    Abstract::File::Append("results.txt", report.c_str(), report.size());
}

bool KReplaySystem::runDirectory(const std::filesystem::path &dirPath) {
    bool success = true;

    for (const auto &dir_entry : std::filesystem::recursive_directory_iterator(dirPath)) {
        const auto &entry_path = dir_entry.path();

        // Skip directories
        if (std::filesystem::is_regular_file(entry_path)) {
            success &= runGhost(entry_path);
        }
    }

    REPORT("Progress: %llu/%llu replays synced (Error rate: %.2f%)", m_replaysSynced,
            m_replaysPlayed,
            static_cast<f64>(m_replaysPlayed - m_replaysSynced) / m_replaysPlayed * 100);

    return success;
}

bool KReplaySystem::runGhost(const std::filesystem::path &ghostPath) {
    if (!ghostPath.has_extension()) {
        return true;
    }

    if (ghostPath.extension() != ".rkg") {
        WARN("Skipping %s", ghostPath.string().c_str());
        return true;
    }

    loadGhost(ghostPath);

    // Has the root scene been created?
    if (!m_sceneMgr->currentScene()) {
        m_sceneMgr->changeScene(0);
    } else {
        m_sceneMgr->createScene(2, m_sceneMgr->currentScene());
    }

    while (!calcEnd()) {
        calc();
    }

    bool isSuccess = success();

    if (!isSuccess) {
        WARN("DESYNC! Ghost path: %s", ghostPath.string().c_str());
    }

    m_sceneMgr->currentScene()->heap()->enableAllocation();
    m_sceneMgr->destroyScene(m_sceneMgr->currentScene());

    delete m_currentRawGhost;
    m_currentRawGhost = nullptr;
    m_currentRawGhostSize = 0;

    delete m_currentGhost;
    m_currentGhost = nullptr;

    ++m_replaysPlayed;

    if (isSuccess) {
        ++m_replaysSynced;
    }

    if (m_progressInterval > 0 && m_replaysPlayed % m_progressInterval == 0) {
        REPORT("Progress: %llu/%llu replays synced (Error rate: %.2f%)", m_replaysSynced,
                m_replaysPlayed,
                static_cast<f64>(m_replaysPlayed - m_replaysSynced) / m_replaysPlayed * 100);
    }

    return isSuccess;
}

void KReplaySystem::loadGhost(const std::filesystem::path &ghostPath) {
    m_currentGhostPath = ghostPath;
    m_currentRawGhost = Abstract::File::Load(m_currentGhostPath, m_currentRawGhostSize);
    if (m_currentRawGhostSize < System::RKG_HEADER_SIZE ||
            m_currentRawGhostSize > System::RKG_UNCOMPRESSED_INPUT_DATA_SECTION_SIZE) {
        PANIC("File cannot be a ghost! Check the file size. %llu", m_currentRawGhostSize);
    }

    // Creating the raw ghost file validates it
    System::RawGhostFile file = System::RawGhostFile(m_currentRawGhost);
    m_currentGhost = new System::GhostFile(file);
}

/// @brief Determines whether the simulation was a success or not.
/// @return Whether the simulation was a success or not.
bool KReplaySystem::success() const {
    auto format = [](const System::Timer &timer) {
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << timer.min << ":" << std::setw(2)
            << std::setfill('0') << timer.sec << "." << std::setw(3) << std::setfill('0')
            << timer.mil;
        return oss.str();
    };

    const auto *raceManager = System::RaceManager::Instance();
    if (raceManager->stage() != System::RaceManager::Stage::FinishGlobal) {
        m_sceneMgr->currentScene()->heap()->enableAllocation();
        reportFail("Race didn't finish");
        return false;
    }

    s32 desyncingTimerIdx = getDesyncingTimerIdx();
    if (desyncingTimerIdx != -1) {
        m_sceneMgr->currentScene()->heap()->enableAllocation();
        std::string msg;

        const auto [correct, incorrect] = getDesyncingTimer(desyncingTimerIdx);
        if (desyncingTimerIdx == 0) {
            msg = "Final timer desync!";
        } else {
            msg = "Lap " + std::to_string(desyncingTimerIdx) + " timer desync!";
        }

        msg += " Expected " + format(correct) + ", got " + format(incorrect);
        reportFail(msg);
        return false;
    }

    return true;
}

/// @brief Finds the desyncing timer index, if one exists.
/// @return -1 if there's no desync, 0 if the final timer desyncs, and 1+ if a lap timer desyncs.
s32 KReplaySystem::getDesyncingTimerIdx() const {
    const auto &player = System::RaceManager::Instance()->player();
    if (m_currentGhost->raceTimer() != player.raceTimer()) {
        return 0;
    }

    for (size_t i = 0; i < 3; ++i) {
        if (m_currentGhost->lapTimer(i) != player.getLapSplit(i + 1)) {
            return i + 1;
        }
    }

    return -1;
}

/// @brief Gets the desyncing timer according to the index.
/// @param i Index to the desyncing timer. Cannot be -1.
/// @return The pair of timers. The first is the correct one, and the second is the incorrect one.
KReplaySystem::DesyncingTimerPair KReplaySystem::getDesyncingTimer(s32 i) const {
    auto cond = i <=> 0;
    ASSERT(cond != std::strong_ordering::less);

    if (cond == std::strong_ordering::equal) {
        const auto &correct = m_currentGhost->raceTimer();
        const auto &incorrect = System::RaceManager::Instance()->player().raceTimer();
        ASSERT(correct != incorrect);
        return DesyncingTimerPair(correct, incorrect);
    } else if (cond == std::strong_ordering::greater) {
        const auto &correct = m_currentGhost->lapTimer(i - 1);
        const auto &incorrect = System::RaceManager::Instance()->player().lapTimer(i - 1);
        ASSERT(correct != incorrect);
        return DesyncingTimerPair(correct, incorrect);
    }

    // This is unreachable
    return DesyncingTimerPair(System::Timer(), System::Timer());
}

/// @brief Initializes the race configuration as needed for replays.
/// @param config The race configuration instance.
/// @param arg Unused optional argument.
void KReplaySystem::OnInit(System::RaceConfig *config, void * /* arg */) {
    config->setGhost(Instance()->m_currentRawGhost);
    config->raceScenario().players[0].type = System::RaceConfig::Player::Type::Ghost;
}
