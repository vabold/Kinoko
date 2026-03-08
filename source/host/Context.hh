#pragma once

#include "Common.hh"

#include <filesystem>
#include <functional>
#include <optional>

class KSystem;

namespace Abstract::g3d {
class FrameCtrl;
class ResAnmChr;
} // namespace Abstract::g3d

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

namespace Host {

/// @brief Contexts can be used to restore a previous memory state for the current session.
/// @details Contexts can be thought of like savestates, where we can save and load a given
/// checkpoint. Contexts work by performing a memcpy of the entire game heap, which is reliable
/// since we override operator new with an EGG::Heap implementation. For variables with static
/// storage duration, they may exist out of the heap. Thus, we need to manually copy those.
class Context {
public:
    Context();
    Context(const Context &c);
    Context(Context &&c);
    ~Context();

    Context &operator=(const Context &rhs);
    Context &operator=(Context &&rhs);

    bool operator==(const Context &rhs) const;

    static void SetActiveContext(const Context &rhs);

private:
    struct Statics {
        Abstract::Memory::MEMList m_rootList;
        Abstract::Memory::MEMList m_archiveList;
        Abstract::Memory::MEMList m_heapList;
        EGG::Heap *m_currentHeap;
        EGG::Heap *m_allocatableHeap;
        EGG::Heap *m_heapForCreateScene;
        u16 m_heapOptionFlg;
        EGG::Heap *m_rootHeap;
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
        Abstract::g3d::ResAnmChr *m_thunderScaleUpAnmChr;
        Abstract::g3d::ResAnmChr *m_thunderScaleDownAnmChr;
        Abstract::g3d::ResAnmChr *m_pressScaleUpAnmChr;
        f32 m_frameCtrlBaseUpdateRate;
        std::array<std::array<f32, 4>, 4> m_dotProductCache;
        f32 m_wanwanMaxPitch;
        f32 m_basabasaInitialXRange;
        f32 m_basabasaInitialYRange;
        u32 m_flamePoleCount;
    };

    void *m_contextMemory;
    Statics m_statics;
};

} // namespace Host
