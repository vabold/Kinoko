#include "Context.hh"

#include <egg/core/SceneManager.hh>

#include <game/field/CollisionDirector.hh>
#include <game/field/ObjectDirector.hh>
#include <game/field/ObjectDrivableDirector.hh>
#include <game/field/RailManager.hh>
#include <game/field/jugem/JugemDirector.hh>
#include <game/field/obj/ObjectBasabasa.hh>
#include <game/field/obj/ObjectFlamePoleFoot.hh>

#include <game/item/ItemDirector.hh>

#include <game/kart/KartObjectManager.hh>
#include <game/kart/KartParamFileManager.hh>

#include <game/render/KartCamera.hh>

#include <game/system/CourseMap.hh>
#include <game/system/KPadDirector.hh>
#include <game/system/RaceConfig.hh>
#include <game/system/RaceManager.hh>

namespace Kinoko::Host {

Context::Context() {
    m_contextMemory = malloc(MEMORY_SPACE_SIZE);
    ASSERT(m_contextMemory && EGG::SceneManager::s_rootHeap);
    memcpy(m_contextMemory, static_cast<void *>(EGG::SceneManager::s_rootHeap), MEMORY_SPACE_SIZE);

    m_statics.m_rootList = Abstract::Memory::MEMiHeapHead::s_rootList;
    m_statics.m_archiveList = EGG::Archive::s_archiveList;
    m_statics.m_heapList = EGG::Heap::s_heapList;
    m_statics.m_currentHeap = EGG::Heap::s_currentHeap;
    m_statics.m_allocatableHeap = EGG::Heap::s_allocatableHeap;
    m_statics.m_heapForCreateScene = EGG::SceneManager::s_heapForCreateScene;
    m_statics.m_boxColMgr = Field::BoxColManager::s_instance;
    m_statics.m_colDir = Field::CollisionDirector::s_instance;
    m_statics.m_courseColMgr = Field::CourseColMgr::s_instance;
    m_statics.m_jugemDir = Field::JugemDirector::s_instance;
    m_statics.m_objDir = Field::ObjectDirector::s_instance;
    m_statics.m_objDrivableDir = Field::ObjectDrivableDirector::s_instance;
    m_statics.m_railMgr = Field::RailManager::s_instance;
    m_statics.m_itemDir = Item::ItemDirector::s_instance;
    m_statics.m_kartObjMgr = Kart::KartObjectManager::s_instance;
    m_statics.m_paramFileMgr = Kart::KartParamFileManager::s_instance;
    m_statics.m_courseMap = System::CourseMap::s_instance;
    m_statics.m_padDir = System::KPadDirector::s_instance;
    m_statics.m_raceConfig = System::RaceConfig::s_instance;
    m_statics.m_onInitCallback = System::RaceConfig::s_onInitCallback;
    m_statics.m_onInitCallbackArg = System::RaceConfig::s_onInitCallbackArg;
    m_statics.m_raceMgr = System::RaceManager::s_instance;
    m_statics.m_resMgr = System::ResourceManager::s_instance;
    m_statics.m_kartCamera = Render::KartCamera::s_instance;
    m_statics.m_thunderScaleUpAnmChr = Kart::KartObjectManager::s_thunderScaleUpAnmChr;
    m_statics.m_thunderScaleDownAnmChr = Kart::KartObjectManager::s_thunderScaleDownAnmChr;
    m_statics.m_pressScaleUpAnmChr = Kart::KartObjectManager::s_pressScaleUpAnmChr;
    m_statics.m_frameCtrlBaseUpdateRate = Abstract::g3d::FrameCtrl::s_baseUpdateRate;
    m_statics.m_dotProductCache = Field::ObjectCollisionBase::s_dotProductCache;
    m_statics.m_wanwanMaxPitch = Field::ObjectDirector::s_wanwanMaxPitch;
    m_statics.m_basabasaInitialXRange = Field::ObjectBasabasa::s_initialXRange;
    m_statics.m_basabasaInitialYRange = Field::ObjectBasabasa::s_initialYRange;
    m_statics.m_flamePoleCount = Field::ObjectFlamePoleFoot::s_flamePoleCount;
}

Context::Context(const Context &c) {
    m_contextMemory = malloc(MEMORY_SPACE_SIZE);
    ASSERT(m_contextMemory && c.m_contextMemory);
    memcpy(m_contextMemory, c.m_contextMemory, MEMORY_SPACE_SIZE);
    m_statics = c.m_statics;
}

/// @brief Move constructs Context by stealing the memory block and ptrs from the provided context.
Context::Context(Context &&c) {
    m_contextMemory = c.m_contextMemory;
    c.m_contextMemory = nullptr;
    m_statics = c.m_statics;
    c.m_statics = {};
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
    m_statics = rhs.m_statics;

    return *this;
}

Context &Context::operator=(Context &&rhs) {
    free(m_contextMemory);
    m_contextMemory = rhs.m_contextMemory;
    rhs.m_contextMemory = nullptr;
    m_statics = rhs.m_statics;
    rhs.m_statics = {};

    return *this;
}

bool Context::operator==(const Context &rhs) const {
    bool ret = m_contextMemory == rhs.m_contextMemory;
    ret = ret && m_statics.m_rootList == rhs.m_statics.m_rootList;
    ret = ret && m_statics.m_archiveList == rhs.m_statics.m_archiveList;
    ret = ret && m_statics.m_heapList == rhs.m_statics.m_heapList;
    ret = ret && m_statics.m_currentHeap == rhs.m_statics.m_currentHeap;
    ret = ret && m_statics.m_allocatableHeap == rhs.m_statics.m_allocatableHeap;
    ret = ret && m_statics.m_heapForCreateScene == rhs.m_statics.m_heapForCreateScene;
    ret = ret && m_statics.m_heapOptionFlg == rhs.m_statics.m_heapOptionFlg;
    ret = ret && m_statics.m_rootHeap == rhs.m_statics.m_rootHeap;
    ret = ret && m_statics.m_boxColMgr == rhs.m_statics.m_boxColMgr;
    ret = ret && m_statics.m_colDir == rhs.m_statics.m_colDir;
    ret = ret && m_statics.m_courseColMgr == rhs.m_statics.m_courseColMgr;
    ret = ret && m_statics.m_jugemDir == rhs.m_statics.m_jugemDir;
    ret = ret && m_statics.m_objDir == rhs.m_statics.m_objDir;
    ret = ret && m_statics.m_objDrivableDir == rhs.m_statics.m_objDrivableDir;
    ret = ret && m_statics.m_railMgr == rhs.m_statics.m_railMgr;
    ret = ret && m_statics.m_itemDir == rhs.m_statics.m_itemDir;
    ret = ret && m_statics.m_kartObjMgr == rhs.m_statics.m_kartObjMgr;
    ret = ret && m_statics.m_paramFileMgr == rhs.m_statics.m_paramFileMgr;
    ret = ret && m_statics.m_courseMap == rhs.m_statics.m_courseMap;
    ret = ret && m_statics.m_padDir == rhs.m_statics.m_padDir;
    ret = ret && m_statics.m_raceConfig == rhs.m_statics.m_raceConfig;
    ret = ret && m_statics.m_onInitCallbackArg == rhs.m_statics.m_onInitCallbackArg;
    ret = ret && m_statics.m_raceMgr == rhs.m_statics.m_raceMgr;
    ret = ret && m_statics.m_resMgr == rhs.m_statics.m_resMgr;
    ret = ret && m_statics.m_kartCamera == rhs.m_statics.m_kartCamera;
    ret = ret && m_statics.m_thunderScaleUpAnmChr == rhs.m_statics.m_thunderScaleUpAnmChr;
    ret = ret && m_statics.m_thunderScaleDownAnmChr == rhs.m_statics.m_thunderScaleDownAnmChr;
    ret = ret && m_statics.m_pressScaleUpAnmChr == rhs.m_statics.m_pressScaleUpAnmChr;
    ret = ret && m_statics.m_frameCtrlBaseUpdateRate == rhs.m_statics.m_frameCtrlBaseUpdateRate;
    ret = ret && m_statics.m_dotProductCache == rhs.m_statics.m_dotProductCache;
    ret = ret && m_statics.m_wanwanMaxPitch == rhs.m_statics.m_wanwanMaxPitch;
    ret = ret && m_statics.m_basabasaInitialXRange == rhs.m_statics.m_basabasaInitialXRange;
    ret = ret && m_statics.m_basabasaInitialYRange == rhs.m_statics.m_basabasaInitialYRange;
    ret = ret && m_statics.m_flamePoleCount == rhs.m_statics.m_flamePoleCount;

    return ret;
}

void Context::SetActiveContext(const Context &rhs) {
    ASSERT(EGG::SceneManager::s_rootHeap && rhs.m_contextMemory);
    memcpy(reinterpret_cast<void *>(EGG::SceneManager::s_rootHeap), rhs.m_contextMemory,
            MEMORY_SPACE_SIZE);

    Abstract::Memory::MEMiHeapHead::s_rootList = rhs.m_statics.m_rootList;
    EGG::Archive::s_archiveList = rhs.m_statics.m_archiveList;
    EGG::Heap::s_heapList = rhs.m_statics.m_heapList;
    EGG::Heap::s_currentHeap = rhs.m_statics.m_currentHeap;
    EGG::Heap::s_allocatableHeap = rhs.m_statics.m_allocatableHeap;
    EGG::SceneManager::s_heapForCreateScene = rhs.m_statics.m_heapForCreateScene;
    EGG::SceneManager::s_heapOptionFlg = rhs.m_statics.m_heapOptionFlg;
    EGG::SceneManager::s_rootHeap = rhs.m_statics.m_rootHeap;
    Field::BoxColManager::s_instance = rhs.m_statics.m_boxColMgr;
    Field::CollisionDirector::s_instance = rhs.m_statics.m_colDir;
    Field::CourseColMgr::s_instance = rhs.m_statics.m_courseColMgr;
    Field::JugemDirector::s_instance = rhs.m_statics.m_jugemDir;
    Field::ObjectDirector::s_instance = rhs.m_statics.m_objDir;
    Field::ObjectDrivableDirector::s_instance = rhs.m_statics.m_objDrivableDir;
    Field::RailManager::s_instance = rhs.m_statics.m_railMgr;
    Item::ItemDirector::s_instance = rhs.m_statics.m_itemDir;
    Kart::KartObjectManager::s_instance = rhs.m_statics.m_kartObjMgr;
    Kart::KartParamFileManager::s_instance = rhs.m_statics.m_paramFileMgr;
    System::CourseMap::s_instance = rhs.m_statics.m_courseMap;
    System::KPadDirector::s_instance = rhs.m_statics.m_padDir;
    System::RaceConfig::s_instance = rhs.m_statics.m_raceConfig;
    System::RaceConfig::s_onInitCallback = rhs.m_statics.m_onInitCallback;
    System::RaceConfig::s_onInitCallbackArg = rhs.m_statics.m_onInitCallbackArg;
    System::RaceManager::s_instance = rhs.m_statics.m_raceMgr;
    System::ResourceManager::s_instance = rhs.m_statics.m_resMgr;
    Render::KartCamera::s_instance = rhs.m_statics.m_kartCamera;
    Kart::KartObjectManager::s_thunderScaleUpAnmChr = rhs.m_statics.m_thunderScaleUpAnmChr;
    Kart::KartObjectManager::s_thunderScaleDownAnmChr = rhs.m_statics.m_thunderScaleDownAnmChr;
    Kart::KartObjectManager::s_pressScaleUpAnmChr = rhs.m_statics.m_pressScaleUpAnmChr;
    Abstract::g3d::FrameCtrl::s_baseUpdateRate = rhs.m_statics.m_frameCtrlBaseUpdateRate;
    Field::ObjectCollisionBase::s_dotProductCache = rhs.m_statics.m_dotProductCache;
    Field::ObjectDirector::s_wanwanMaxPitch = rhs.m_statics.m_wanwanMaxPitch;
    Field::ObjectBasabasa::s_initialXRange = rhs.m_statics.m_basabasaInitialXRange;
    Field::ObjectBasabasa::s_initialYRange = rhs.m_statics.m_basabasaInitialYRange;
    Field::ObjectFlamePoleFoot::s_flamePoleCount = rhs.m_statics.m_flamePoleCount;
}

} // namespace Kinoko::Host
