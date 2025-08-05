#include "SavestateManager.hh"

#include <egg/core/SceneManager.hh>

#include <game/field/CollisionDirector.hh>
#include <game/field/ObjectDirector.hh>
#include <game/field/ObjectDrivableDirector.hh>
#include <game/field/RailManager.hh>

#include <game/item/ItemDirector.hh>

#include <game/kart/KartObjectManager.hh>
#include <game/kart/KartParamFileManager.hh>

#include <game/system/CourseMap.hh>
#include <game/system/KPadDirector.hh>
#include <game/system/RaceConfig.hh>
#include <game/system/RaceManager.hh>

#include <fstream>

SavestateManager::SavestateManager()
    : m_bufferedInputs{}, m_framesBuffered(0), m_controller(nullptr), m_sceneMgr(nullptr),
      m_contextBlocks({nullptr}), m_contexts({}) {
    for (size_t i = 0; i < m_files.size(); ++i) {
        m_files[i] = GetFilePathForSlot(i);
    }

    // Flush the state file so we aren't appending to an older session's file.
    std::ofstream outFile(STATE_FILE, std::ios::trunc);
    if (!outFile) {
        PANIC("Could not truncate %s!", STATE_FILE);
    }
}

SavestateManager::~SavestateManager() = default;

/// @brief Saves the current input state to a buffer. When full, flushes the buffer to file.
/// It is the host system's responsibility that this is called on every calc function, or else you
/// run the risk of losing input information.
void SavestateManager::calcState() {
    const auto &state = m_controller->raceInputState();

    u16 face = state.buttons & 0xf;
    u16 trick = static_cast<u16>(state.trick) & 0xf;
    u8 x = System::StateToRawStick(state.stick.x);
    u8 y = System::StateToRawStick(state.stick.y);

    u16 packedState = (face << 12) | (trick << 8) | (x << 4) | y;
    m_bufferedInputs[m_framesBuffered++] = parse(packedState);

    if (m_framesBuffered == m_bufferedInputs.size()) {
        flushBufferToFile();
    }
}

/// @brief Attempts to save the current race's input sequence to a file.
/// @param slot The "slot" that the file corresponds to
/// @return Whether or not the savestate was saved to file successfully.
bool SavestateManager::saveState(size_t slot) {
    static constexpr u32 PARSED_SIG = parse(KSAV_SIGNATURE);

    if (slot >= SLOTS) {
        WARN("Invalid savestate slot %zu.", slot);
        return false;
    }

    flushBufferToFile();

    // Seek to end immediately so we can get framecount
    std::ifstream inFile(STATE_FILE, std::ios::ate);
    if (!inFile) {
        WARN("Could not open %s!", STATE_FILE);
        return false;
    }

    std::ofstream outFile(m_files[slot], std::ios::binary);
    if (!outFile) {
        WARN("Could not open %s!", m_files[slot]);
        return false;
    }

    // Write signature, params, framecount, and inputs to savestate file
    u32 params = parse(GetParamsFromRaceConfig());
    u32 frames = parse(static_cast<u32>(inFile.tellg() / sizeof(u16)));

    bool writeOK =
            outFile.write(reinterpret_cast<const char *>(&PARSED_SIG), sizeof(PARSED_SIG)).good();
    writeOK = writeOK && outFile.write(reinterpret_cast<char *>(&params), sizeof(params)).good();
    writeOK = writeOK && outFile.write(reinterpret_cast<char *>(&frames), sizeof(frames)).good();

    if (!writeOK) {
        WARN("Failed to write to %s!", m_files[slot]);
        return false;
    }

    // Clear eofbit and seek back to beginning of input file
    inFile.clear();
    inFile.seekg(0, std::ios::beg);
    outFile << inFile.rdbuf();

    REPORT("Saved state to slot %zu", slot);
    return true;
}

/// @brief Attempts to load a savestate from a file.
/// @param slot The slot that the file corresponds to
/// @return Whether or not the state was loaded successfully
bool SavestateManager::loadState(size_t slot) {
    if (slot >= SLOTS) {
        WARN("Invalid savestate slot %zu.", slot);
        return false;
    }

    std::ifstream inFile(m_files[slot], std::ios::binary);
    if (!inFile) {
        WARN("Cannot find/open savestate at %s!", m_files[slot]);
        return false;
    }

    u32 sig;
    inFile.read(reinterpret_cast<char *>(&sig), sizeof(sig));
    ASSERT(parse(sig) == KSAV_SIGNATURE);

    ASSERT(m_controller);
    ASSERT(m_sceneMgr);

    m_sceneMgr->destroyScene(m_sceneMgr->currentScene());

    u32 packedScenario;
    if (!inFile.read(reinterpret_cast<char *>(&packedScenario), sizeof(packedScenario)).good()) {
        WARN("Failed to read scenario data! Your savestate is corrupted at %s.", m_files[slot]);
        return false;
    }

    setRaceConfigFromParams(parse(packedScenario));

    m_sceneMgr->createScene(2, m_sceneMgr->currentScene());

    u32 framecount;
    if (!inFile.read(reinterpret_cast<char *>(&framecount), sizeof(framecount)).good()) {
        WARN("Failed to read framecount! Your savestate is corrupted at %s.", m_files[slot]);
        return false;
    }

    framecount = parse(framecount);

    // Fast-forward to load the state
    for (u32 i = 0; i < framecount; ++i) {
        u16 frameData;
        if (!inFile.read(reinterpret_cast<char *>(&frameData), sizeof(frameData)).good()) {
            WARN("Error on frame %u: Failed to read framedata! Your savestate is corrupted at %s.",
                    i, m_files[slot]);
            return false;
        }

        frameData = parse(frameData);

        u16 face = frameData >> 12;
        System::Trick trick = static_cast<System::Trick>((frameData >> 8) & 0xf);
        u8 x = (frameData >> 4) & 0xf;
        u8 y = frameData & 0xf;

        m_controller->setInputsRawStick(face, x, y, trick);

        m_sceneMgr->calc();
    }

    REPORT("Loaded state in slot %zu", slot);
    return true;
}

bool SavestateManager::saveContext(size_t slot) {
    if (slot >= SLOTS) {
        WARN("Invalid savestate slot %zu.", slot);
        return false;
    }

    getContext(slot);

    if (!m_contextBlocks[slot]) {
        initSlot(slot);
    }

    ASSERT(m_contextBlocks[slot]);
    memcpy(m_contextBlocks[slot], EGG::SceneManager::s_rootHeap, MEMORY_SPACE_SIZE);

    return true;
}

/// @brief Attempts to load an in-memory savestate from the provided slot.
/// @param slot The savestate slot to load from
/// @return Whether or not the savestate loaded successfully.
bool SavestateManager::loadContext(size_t slot) {
    if (slot >= SLOTS) {
        WARN("Invalid savestate slot %zu.", slot);
        return false;
    }

    if (!m_contextBlocks[slot]) {
        REPORT("No savestate in slot %zu", slot);
        return false;
    }

    memcpy(reinterpret_cast<void *>(EGG::SceneManager::s_rootHeap), m_contextBlocks[slot],
            MEMORY_SPACE_SIZE);

    setContext(slot);

    return true;
}

void SavestateManager::flushBufferToFile() {
    if (m_framesBuffered == 0) {
        return;
    }

    std::ofstream outFile(STATE_FILE, std::ios::app | std::ios::binary);
    if (!outFile) {
        PANIC("Could not flush inputs to %s!", STATE_FILE);
    }

    outFile.write(reinterpret_cast<const char *>(m_bufferedInputs.data()),
            m_framesBuffered * sizeof(u16));

    m_bufferedInputs = {};
    m_framesBuffered = 0;
}

void SavestateManager::initSlot(size_t slot) {
    // TODO: This is copied from main.cc. Should we store this in SceneManager?
    Abstract::Memory::MEMiHeapHead::OptFlag opt;
    opt.setBit(Abstract::Memory::MEMiHeapHead::eOptFlag::ZeroFillAlloc);

#ifdef BUILD_DEBUG
    opt.setBit(Abstract::Memory::MEMiHeapHead::eOptFlag::DebugFillAlloc);
#endif

    ASSERT(slot < SLOTS);
    m_contextBlocks[slot] = malloc(MEMORY_SPACE_SIZE);
}

std::optional<SavestateManager::Context> &SavestateManager::getContext(size_t slot) {
    ASSERT(slot < SLOTS);
    auto &context = m_contexts[slot];

    context = Context();
    context->m_rootList = Abstract::Memory::MEMiHeapHead::s_rootList;
    context->m_archiveList = EGG::Archive::s_archiveList;
    context->m_heapList = EGG::Heap::s_heapList;
    context->m_currentHeap = EGG::Heap::s_currentHeap;
    context->m_heapForCreateScene = EGG::SceneManager::s_heapForCreateScene;
    context->m_boxColMgr = Field::BoxColManager::s_instance;
    context->m_colDir = Field::CollisionDirector::s_instance;
    context->m_courseColMgr = Field::CourseColMgr::s_instance;
    context->m_objDir = Field::ObjectDirector::s_instance;
    context->m_objDrivableDir = Field::ObjectDrivableDirector::s_instance;
    context->m_railMgr = Field::RailManager::s_instance;
    context->m_itemDir = Item::ItemDirector::s_instance;
    context->m_kartObjMgr = Kart::KartObjectManager::s_instance;
    context->m_paramFileMgr = Kart::KartParamFileManager::s_instance;
    context->m_courseMap = System::CourseMap::s_instance;
    context->m_padDir = System::KPadDirector::s_instance;
    context->m_raceConfig = System::RaceConfig::s_instance;
    context->m_onInitCallback = System::RaceConfig::s_onInitCallback;
    context->m_onInitCallbackArg = System::RaceConfig::s_onInitCallbackArg;
    context->m_raceMgr = System::RaceManager::s_instance;
    context->m_resMgr = System::ResourceManager::s_instance;

    return context;
}

void SavestateManager::setContext(size_t slot) {
    ASSERT(slot < SLOTS);
    auto &context = m_contexts[slot];
    ASSERT(context);

    Abstract::Memory::MEMiHeapHead::s_rootList = *context->m_rootList;
    EGG::Archive::s_archiveList = *context->m_archiveList;
    EGG::Heap::s_heapList = *context->m_heapList;
    EGG::Heap::s_currentHeap = context->m_currentHeap;
    EGG::SceneManager::s_heapForCreateScene = context->m_heapForCreateScene;
    Field::BoxColManager::s_instance = context->m_boxColMgr;
    Field::CollisionDirector::s_instance = context->m_colDir;
    Field::CourseColMgr::s_instance = context->m_courseColMgr;
    Field::ObjectDirector::s_instance = context->m_objDir;
    Field::ObjectDrivableDirector::s_instance = context->m_objDrivableDir;
    Field::RailManager::s_instance = context->m_railMgr;
    Item::ItemDirector::s_instance = context->m_itemDir;
    Kart::KartObjectManager::s_instance = context->m_kartObjMgr;
    Kart::KartParamFileManager::s_instance = context->m_paramFileMgr;
    System::CourseMap::s_instance = context->m_courseMap;
    System::KPadDirector::s_instance = context->m_padDir;
    System::RaceConfig::s_instance = context->m_raceConfig;
    System::RaceConfig::s_onInitCallback = context->m_onInitCallback;
    System::RaceConfig::s_onInitCallbackArg = context->m_onInitCallbackArg;
    System::RaceManager::s_instance = context->m_raceMgr;
    System::ResourceManager::s_instance = context->m_resMgr;
}

/// @brief Sets the course, character, vehicle, and drift based on the provided params.
/// @param params From MSB to LSB each 1 byte wide: course, character, vehicle, and driftIsAuto.
void SavestateManager::setRaceConfigFromParams(u32 params) {
    auto &scenario = System::RaceConfig::Instance()->raceScenario();
    scenario.course = static_cast<Course>(params >> 24);

    auto &player = scenario.players[0];
    player.character = static_cast<Character>((params >> 16) & 0x1f);
    player.vehicle = static_cast<Vehicle>((params >> 8) & 0x3f);
    player.driftIsAuto = static_cast<bool>(params & 0x1);

    m_controller->setDriftIsAuto(static_cast<bool>(params & 0x1));
}

std::filesystem::path SavestateManager::GetFilePathForSlot(size_t slot) {
    char filename[16];
    snprintf(filename, sizeof(filename), "kinoko.s%02zu", slot);

    return std::filesystem::current_path().append(filename);
}

u32 SavestateManager::GetParamsFromRaceConfig() {
    const auto &scenario = System::RaceConfig::Instance()->raceScenario();
    u32 course = static_cast<u32>(scenario.course);

    const auto &player = scenario.players[0];
    u32 character = static_cast<u32>(player.character);
    u32 vehicle = static_cast<u32>(player.vehicle);
    u32 driftIsAuto = static_cast<u32>(player.driftIsAuto);

    return (course << 24) | (character << 16) | (vehicle << 8) | driftIsAuto;
}
