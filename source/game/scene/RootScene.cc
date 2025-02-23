#include "RootScene.hh"

#include "game/system/KPadDirector.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/ResourceManager.hh"

#include <egg/core/SceneManager.hh>
#include <host/SceneId.hh>

#include <ScopeLock.hh>

namespace Scene {

/// @addr{0x80542878}
RootScene::RootScene() {
    m_heap->setName("RootSceneHeap");
}

/// @addr{0x805429A8}
RootScene::~RootScene() = default;

/// @addr{0x80543B84}
void RootScene::enter() {
    allocate();
    init();
#ifdef BUILD_DEBUG
    checkMemory();
#endif // BUILD_DEBUG
    m_sceneMgr->createChildScene(static_cast<int>(Host::SceneId::Race), this);
}

/// @addr{0x80542D4C}
void RootScene::allocate() {
    {
        ScopeLock<GroupID> lock(GroupID::Resource);
        System::ResourceManager::CreateInstance();
    }

    {
        ScopeLock<GroupID> lock(GroupID::Race);
        System::KPadDirector::CreateInstance();
        System::RaceConfig::CreateInstance();
    }
}

/// @addr{0x805438B4}
void RootScene::init() {
    {
        ScopeLock<GroupID> lock(GroupID::Race);
        System::RaceConfig::Instance()->init();
    }
}

#ifdef BUILD_DEBUG
void RootScene::checkMemory() {
    EGG::ExpHeap *heap = EGG::Heap::dynamicCastToExp(m_heap);
    ASSERT(heap);

    heap->calcGroupSize(&m_groupSizeRecord);
    size_t defaultSize = m_groupSizeRecord.getGroupSize(static_cast<u16>(GroupID::None));

    // The scene is the first, always group ID 0 allocation to happen in the scene's heap
    ASSERT(defaultSize >= sizeof(RootScene));
    defaultSize -= sizeof(RootScene);
    if (defaultSize > 0) {
        WARN("Default memory usage found! %zu bytes", defaultSize);
    }

    for (u16 groupID = 1; groupID < m_groupSizeRecord.size(); ++groupID) {
        size_t size = m_groupSizeRecord.getGroupSize(groupID);
        if (size == 0) {
            continue;
        }

        DEBUG("Group ID %d: Allocated %zu bytes", groupID, size);
    }
}
#endif // BUILD_DEBUG

} // namespace Scene
