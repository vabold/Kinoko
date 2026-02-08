#include "Context.hh"

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

namespace Host {

Context::Context() {
    m_contextMemory = malloc(MEMORY_SPACE_SIZE);
    ASSERT(m_contextMemory && EGG::SceneManager::s_rootHeap);
    memcpy(m_contextMemory, static_cast<void*>(EGG::SceneManager::s_rootHeap), MEMORY_SPACE_SIZE);

    m_ptrs.m_rootList = Abstract::Memory::MEMiHeapHead::s_rootList;
    m_ptrs.m_archiveList = EGG::Archive::s_archiveList;
    m_ptrs.m_heapList = EGG::Heap::s_heapList;
    m_ptrs.m_currentHeap = EGG::Heap::s_currentHeap;
    m_ptrs.m_heapForCreateScene = EGG::SceneManager::s_heapForCreateScene;
    m_ptrs.m_boxColMgr = Field::BoxColManager::s_instance;
    m_ptrs.m_colDir = Field::CollisionDirector::s_instance;
    m_ptrs.m_courseColMgr = Field::CourseColMgr::s_instance;
    m_ptrs.m_objDir = Field::ObjectDirector::s_instance;
    m_ptrs.m_objDrivableDir = Field::ObjectDrivableDirector::s_instance;
    m_ptrs.m_railMgr = Field::RailManager::s_instance;
    m_ptrs.m_itemDir = Item::ItemDirector::s_instance;
    m_ptrs.m_kartObjMgr = Kart::KartObjectManager::s_instance;
    m_ptrs.m_paramFileMgr = Kart::KartParamFileManager::s_instance;
    m_ptrs.m_courseMap = System::CourseMap::s_instance;
    m_ptrs.m_padDir = System::KPadDirector::s_instance;
    m_ptrs.m_raceConfig = System::RaceConfig::s_instance;
    m_ptrs.m_onInitCallback = System::RaceConfig::s_onInitCallback;
    m_ptrs.m_onInitCallbackArg = System::RaceConfig::s_onInitCallbackArg;
    m_ptrs.m_raceMgr = System::RaceManager::s_instance;
    m_ptrs.m_resMgr = System::ResourceManager::s_instance;
}

Context::Context(const Context &c) {
    m_contextMemory = malloc(MEMORY_SPACE_SIZE);
    ASSERT(m_contextMemory && c.m_contextMemory);
    memcpy(m_contextMemory, c.m_contextMemory, MEMORY_SPACE_SIZE);
    m_ptrs = c.m_ptrs;
}

/// @brief Move constructs Context by stealing the memory block and ptrs from the provided context.
Context::Context(Context &&c) {
    m_contextMemory = c.m_contextMemory;
    c.m_contextMemory = nullptr;
    m_ptrs = c.m_ptrs;
    c.m_ptrs = {};
}

Context::~Context() {
    free(m_contextMemory);
}

Context &Context::operator=(const Context &rhs) {
    if (*this == rhs) {
        return *this;
    }

    ASSERT(m_contextMemory && rhs.m_contextMemory && m_contextMemory != rhs.m_contextMemory);
    memcpy(m_contextMemory, rhs.m_contextMemory, MEMORY_SPACE_SIZE);
    m_ptrs = rhs.m_ptrs;

    return *this;
}

Context &Context::operator=(Context &&rhs) {
    free(m_contextMemory);
    m_contextMemory = rhs.m_contextMemory;
    rhs.m_contextMemory = nullptr;
    m_ptrs = rhs.m_ptrs;
    rhs.m_ptrs = {};

    return *this;
}

bool Context::operator==(const Context &rhs) const {
    bool ret = m_contextMemory == rhs.m_contextMemory;
    ret = ret && m_ptrs.m_rootList == rhs.m_ptrs.m_rootList;
    ret = ret && m_ptrs.m_archiveList == rhs.m_ptrs.m_archiveList;
    ret = ret && m_ptrs.m_heapList == rhs.m_ptrs.m_heapList;
    ret = ret && m_ptrs.m_currentHeap == rhs.m_ptrs.m_currentHeap;
    ret = ret && m_ptrs.m_heapForCreateScene == rhs.m_ptrs.m_heapForCreateScene;
    ret = ret && m_ptrs.m_boxColMgr == rhs.m_ptrs.m_boxColMgr;
    ret = ret && m_ptrs.m_colDir == rhs.m_ptrs.m_colDir;
    ret = ret && m_ptrs.m_courseColMgr == rhs.m_ptrs.m_courseColMgr;
    ret = ret && m_ptrs.m_objDir == rhs.m_ptrs.m_objDir;
    ret = ret && m_ptrs.m_objDrivableDir == rhs.m_ptrs.m_objDrivableDir;
    ret = ret && m_ptrs.m_railMgr == rhs.m_ptrs.m_railMgr;
    ret = ret && m_ptrs.m_itemDir == rhs.m_ptrs.m_itemDir;
    ret = ret && m_ptrs.m_kartObjMgr == rhs.m_ptrs.m_kartObjMgr;
    ret = ret && m_ptrs.m_paramFileMgr == rhs.m_ptrs.m_paramFileMgr;
    ret = ret && m_ptrs.m_courseMap == rhs.m_ptrs.m_courseMap;
    ret = ret && m_ptrs.m_padDir == rhs.m_ptrs.m_padDir;
    ret = ret && m_ptrs.m_raceConfig == rhs.m_ptrs.m_raceConfig;
    ret = ret && m_ptrs.m_onInitCallbackArg == rhs.m_ptrs.m_onInitCallbackArg;
    ret = ret && m_ptrs.m_raceMgr == rhs.m_ptrs.m_raceMgr;
    ret = ret && m_ptrs.m_resMgr == rhs.m_ptrs.m_resMgr;

    return ret;
}

void Context::SetActiveContext(const Context &rhs) {
    ASSERT(EGG::SceneManager::s_rootHeap && rhs.m_contextMemory);
    memcpy(reinterpret_cast<void *>(EGG::SceneManager::s_rootHeap), rhs.m_contextMemory,
            MEMORY_SPACE_SIZE);

    Abstract::Memory::MEMiHeapHead::s_rootList = rhs.m_ptrs.m_rootList;
    EGG::Archive::s_archiveList = rhs.m_ptrs.m_archiveList;
    EGG::Heap::s_heapList = rhs.m_ptrs.m_heapList;
    EGG::Heap::s_currentHeap = rhs.m_ptrs.m_currentHeap;
    EGG::SceneManager::s_heapForCreateScene = rhs.m_ptrs.m_heapForCreateScene;
    Field::BoxColManager::s_instance = rhs.m_ptrs.m_boxColMgr;
    Field::CollisionDirector::s_instance = rhs.m_ptrs.m_colDir;
    Field::CourseColMgr::s_instance = rhs.m_ptrs.m_courseColMgr;
    Field::ObjectDirector::s_instance = rhs.m_ptrs.m_objDir;
    Field::ObjectDrivableDirector::s_instance = rhs.m_ptrs.m_objDrivableDir;
    Field::RailManager::s_instance = rhs.m_ptrs.m_railMgr;
    Item::ItemDirector::s_instance = rhs.m_ptrs.m_itemDir;
    Kart::KartObjectManager::s_instance = rhs.m_ptrs.m_kartObjMgr;
    Kart::KartParamFileManager::s_instance = rhs.m_ptrs.m_paramFileMgr;
    System::CourseMap::s_instance = rhs.m_ptrs.m_courseMap;
    System::KPadDirector::s_instance = rhs.m_ptrs.m_padDir;
    System::RaceConfig::s_instance = rhs.m_ptrs.m_raceConfig;
    System::RaceConfig::s_onInitCallback = rhs.m_ptrs.m_onInitCallback;
    System::RaceConfig::s_onInitCallbackArg = rhs.m_ptrs.m_onInitCallbackArg;
    System::RaceManager::s_instance = rhs.m_ptrs.m_raceMgr;
    System::ResourceManager::s_instance = rhs.m_ptrs.m_resMgr;
}

} // namespace Host
