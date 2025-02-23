#pragma once

#include "game/system/MultiDvdArchive.hh"

#include <egg/core/ExpHeap.hh>
#include <egg/core/Scene.hh>

#include <list>

/// @brief Pertains to scene handling.
namespace Scene {

/// @brief Interface for menu and race scenes.
class GameScene : public EGG::Scene {
public:
    GameScene();
    ~GameScene() override;

    void calc() final;
    void enter() final;
    void exit() final;
    void reinit() final;

    virtual void createEngines() = 0;
    virtual void initEngines() = 0;
    virtual void calcEngines() = 0;
    virtual void destroyEngines() = 0;
    virtual void configure() = 0;
    virtual void onReinit() {}

protected:
    void appendResource(System::MultiDvdArchive *archive, s32 id);

private:
    struct Resource {
        Resource(System::MultiDvdArchive *archive, s32 id);

        System::MultiDvdArchive *archive;
        s32 id;
    };

    void initScene();
    void deinitScene();
    void unmountResources();

#ifdef BUILD_DEBUG
    void checkMemory();
    void getMemoryLeakTags();
    size_t getMemoryLeakTagCount();

    static void ViewTags(void *block, Abstract::Memory::MEMiHeapHead *heap, uintptr_t param);
    static void IncreaseTagCount(void *block, Abstract::Memory::MEMiHeapHead *heap,
            uintptr_t param);
#endif // BUILD_DEBUG

    EGG::ExpHeap::GroupSizeRecord m_groupSizeRecord;
    std::list<Resource *> m_resources; ///< List of all active resources in the scene.
    int m_nextSceneId;

    size_t m_totalMemoryUsed;
};

} // namespace Scene
