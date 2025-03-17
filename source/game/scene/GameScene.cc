#include "GameScene.hh"

#include "game/system/KPadDirector.hh"
#include "game/system/ResourceManager.hh"

#include <egg/core/SceneManager.hh>

namespace Scene {

/// @addr{0x8051A1E0}
GameScene::GameScene() {
    m_heap->setName("DefaultGameSceneHeap");
    m_nextSceneId = -1;

#ifdef BUILD_DEBUG
    m_totalMemoryUsed = 0;
    EGG::ExpHeap *heap = EGG::Heap::dynamicCastToExp(m_heap);
    ASSERT(heap);

    heap->calcGroupSize(&m_groupSizeRecord);
    for (u16 groupID = 0; groupID < m_groupSizeRecord.size(); ++groupID) {
        m_totalMemoryUsed += m_groupSizeRecord.getGroupSize(groupID);
    }
#endif // BUILD_DEBUG
}

/// @addr{0x8051A3C0}
GameScene::~GameScene() {
    m_resources.clear();

#ifdef BUILD_DEBUG
    EGG::ExpHeap *heap = EGG::Heap::dynamicCastToExp(m_heap);
    ASSERT(heap);

    heap->calcGroupSize(&m_groupSizeRecord);
    size_t sum = 0;
    for (u16 groupID = 0; groupID < m_groupSizeRecord.size(); ++groupID) {
        sum += m_groupSizeRecord.getGroupSize(groupID);
    }

    if (sum > m_totalMemoryUsed) {
        WARN("MEMORY LEAK DETECTED: %zu bytes", sum - m_totalMemoryUsed);
        getMemoryLeakTags();
    }
#endif // BUILD_DEBUG
}

/// @addr{0x8051B3C8}
void GameScene::calc() {
    System::KPadDirector::Instance()->calc();
    calcEngines();
}

/// @addr{0x8051AB58}
void GameScene::enter() {
    configure();
    initScene();
}

/// @addr{0x8051B250}
void GameScene::exit() {
    deinitScene();
    unmountResources();
}

/// @addr{0x8051B7B0}
void GameScene::reinit() {
    exit();
    if (m_nextSceneId < 0) {
        onReinit();
        initScene();
    } else {
        m_sceneMgr->changeSiblingScene(m_nextSceneId);
    }
}

/// @addr{0x8051AA58}
void GameScene::appendResource(System::MultiDvdArchive *archive, s32 id) {
    m_resources.push_back(new Resource(archive, id));
}

/// @addr{Inlined in 0x8051AA58}
GameScene::Resource::Resource(System::MultiDvdArchive *archive, s32 id)
    : archive(archive), id(id) {}

/// @addr{0x8051A4DC}
void GameScene::initScene() {
    createEngines();
    System::KPadDirector::Instance()->reset();
    initEngines();
#ifdef BUILD_DEBUG
    checkMemory();
#endif // BUILD_DEBUG
}

/// @addr{0x8051B0F4}
void GameScene::deinitScene() {
    if (m_nextSceneId >= 0) {
        return;
    }

    destroyEngines();
    System::KPadDirector::Instance()->clear();
}

/// @addr{0x8051AAE8}
void GameScene::unmountResources() {
    auto *resourceManager = System::ResourceManager::Instance();
    for (auto iter = m_resources.begin(); iter != m_resources.end();) {
        Resource *resource = *iter;
        resourceManager->unmount(resource->archive);
        iter = m_resources.erase(iter);
        delete resource;
    }
}

#ifdef BUILD_DEBUG
void GameScene::checkMemory() {
    EGG::ExpHeap *heap = EGG::Heap::dynamicCastToExp(m_heap);
    ASSERT(heap);

    heap->calcGroupSize(&m_groupSizeRecord);
    size_t defaultSize = m_groupSizeRecord.getGroupSize(static_cast<u16>(GroupID::None));

    // Because we're working with a class that can be inherited (but not doubly-inherited),
    // it's possible that derived classes can have a different size compared to GameScene
    // We don't need to know the exact derived class, though, as we have the memory block head
    // The scene is the first, always group ID 0 allocation to happen in the scene's heap
    Abstract::Memory::MEMiExpBlockHead *blockHead =
            static_cast<Abstract::Memory::MEMiExpBlockHead *>(
                    SubOffset(this, sizeof(Abstract::Memory::MEMiExpBlockHead)));
    size_t initialAllocSize = blockHead->m_size;
    ASSERT(defaultSize >= initialAllocSize);

    defaultSize -= initialAllocSize;
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

void GameScene::getMemoryLeakTags() {
    size_t count = getMemoryLeakTagCount();
    if (count == 0) {
        return;
    }

    std::span<u32> tags = std::span<u32>(new u32[count], count);
    for (auto &tag : tags) {
        tag = 0; // empty tag
    }

    EGG::Heap::dynamicCastToExp(m_heap)->dynamicCastHandleToExp()->visitAllocated(ViewTags,
            GetAddrNum(&tags));

    DEBUG("TAGGED MEMORY BLOCKS:");
    printf("[%d", tags[0]);
    for (u32 i = 1; i < tags.size(); ++i) {
        printf(", %d", tags[i]);
    }
    printf("]\n");

    delete[] tags.data();
}

size_t GameScene::getMemoryLeakTagCount() {
    size_t count = 0;
    EGG::Heap::dynamicCastToExp(m_heap)->dynamicCastHandleToExp()->visitAllocated(IncreaseTagCount,
            GetAddrNum(&count));
    return count;
}

void GameScene::ViewTags(void *block, Abstract::Memory::MEMiHeapHead * /*heap*/, uintptr_t param) {
    Abstract::Memory::MEMiExpBlockHead *blockHead =
            static_cast<Abstract::Memory::MEMiExpBlockHead *>(
                    SubOffset(block, sizeof(Abstract::Memory::MEMiExpBlockHead)));
    std::span<u32> *span = reinterpret_cast<std::span<u32> *>(param);
    for (auto &tag : *span) {
        if (tag == 0) {
            tag = blockHead->m_tag;
            break;
        }
    }
}

void GameScene::IncreaseTagCount(void * /*block*/, Abstract::Memory::MEMiHeapHead * /*heap*/,
        uintptr_t param) {
    size_t &count = *reinterpret_cast<size_t *>(param);
    ++count;
}
#endif // BUILD_DEBUG

} // namespace Scene
