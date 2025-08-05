#pragma once

#include "Common.hh"

#include <filesystem>
#include <functional>
#include <optional>

class KSystem;

namespace Abstract::Memory {
class MEMList;
} // namespace Abstract::Memory

namespace EGG {
class Archive;
class Heap;
class SceneManager;
} // namespace EGG

namespace Field {
class BoxColManager;
class CollisionDirector;
class CourseColMgr;
class ObjectCollisionBase;
class ObjectDirector;
class ObjectDrivableDirector;
class RailManager;
} // namespace Field

namespace Item {
class ItemDirector;
} // namespace Item

namespace Kart {
class KartObjectManager;
class KartParamFileManager;
} // namespace Kart

namespace System {
class CourseMap;
class KPadDirector;
class KPadHostController;
class RaceConfig;
typedef std::function<void(RaceConfig *, void *)> InitCallback;
class RaceInputState;
class RaceManager;
class ResourceManager;
class KSystem;
} // namespace System

/// @brief Implements state management for saving and loading race state.
/// @details We implement two different savestate approaches:
/// ### File-based
/// * Store history of inputs to file and replay inputs on load
/// * Easily sharable
/// * Filesize scales with framecount
/// * Save and load time are variable as input length is proportional to framecount
///
/// ### Memory-based
/// * Saves a copy of the memory heap, which is reliable thanks to @ref EGG::Heap
/// * Cannot be shared. Savestates only persist for the current Kinoko instance.
/// * Memory usage is consistent (~168MB)
/// * Save and load times are consistent (<1ms)
class SavestateManager {
private:
    static constexpr size_t SLOTS = 10;
    static constexpr u32 KSAV_SIGNATURE = 0x4b534156; // KSAV

public:
    SavestateManager();
    ~SavestateManager();

    void calcState();
    bool saveState(size_t slot);
    bool loadState(size_t slot);

    bool saveContext(size_t slot);
    bool loadContext(size_t slot);

    void setRaceConfigFromParams(u32 params);

    void setSceneMgr(EGG::SceneManager *sceneMgr) {
        m_sceneMgr = sceneMgr;
    }

    void setController(System::KPadHostController *controller) {
        m_controller = controller;
    }

private:
    /// @brief Used to store the location of singletons when using memory-based savestates.
    struct Context {
        std::optional<Abstract::Memory::MEMList> m_rootList;    ///< Optional for static initializer
        std::optional<Abstract::Memory::MEMList> m_archiveList; ///< Optional for static initializer
        std::optional<Abstract::Memory::MEMList> m_heapList;    ///< Optional for static initializer
        EGG::Heap *m_currentHeap;
        EGG::Heap *m_heapForCreateScene;
        Field::BoxColManager *m_boxColMgr;
        Field::CollisionDirector *m_colDir;
        Field::CourseColMgr *m_courseColMgr;
        Field::ObjectDirector *m_objDir;
        Field::ObjectDrivableDirector *m_objDrivableDir;
        Field::RailManager *m_railMgr;
        Item::ItemDirector *m_itemDir;
        Kart::KartObjectManager *m_kartObjMgr;
        Kart::KartParamFileManager *m_paramFileMgr;
        System::CourseMap *m_courseMap;
        System::KPadDirector *m_padDir;
        System::RaceConfig *m_raceConfig;
        System::InitCallback m_onInitCallback;
        void *m_onInitCallbackArg;
        System::RaceManager *m_raceMgr;
        System::ResourceManager *m_resMgr;
    };

    void flushBufferToFile();
    void initSlot(size_t slot);

    std::optional<SavestateManager::Context> &getContext(size_t slot);
    void setContext(size_t slot);

    std::array<u16, 128> m_bufferedInputs;
    size_t m_framesBuffered;

    std::array<std::filesystem::path, SLOTS> m_files;
    System::KPadHostController *m_controller;
    EGG::SceneManager *m_sceneMgr;

    std::array<void *, SLOTS> m_contextBlocks;

    std::array<std::optional<Context>, SLOTS + 2> m_contexts;

    const char *STATE_FILE = "StateBuffer.dat";

    static std::filesystem::path GetFilePathForSlot(size_t slot);
    static u32 GetParamsFromRaceConfig();

    // TODO: This is copied from main.cc. Should we store this in SceneManager?
    static constexpr size_t MEMORY_SPACE_SIZE = 0x1000000;
};
