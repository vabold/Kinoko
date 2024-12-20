#include "System.hh"

#include "host/SceneCreatorDynamic.hh"

#include <abstract/File.hh>

#include <egg/core/ExpHeap.hh>

#include <cstring>

namespace Host {

/// @brief The main entry point for the program.
/// @addr{0x80008EF0}
void KSystem::main(int /* argc */, char** /* argv */) {
    initMemory();
    init();

    m_sceneMgr->changeScene(0);
    run();
}

/// @addr{0x80009194}
void KSystem::init() {
    auto *sceneCreator = new SceneCreatorDynamic;
    m_sceneMgr = new EGG::SceneManager(sceneCreator);
}

/// @addr{0x80242504}
/// @brief Creates a memory space and a heap from that memory space.
/// @warning This must be called first, or any `new` calls will have a nullptr heap!
/// @details In the base game, the initial memory space is provided by the OS arena.
/// For local implementations, we request memory from the OS via malloc, as it's not overwritten.
void KSystem::initMemory() {
    constexpr size_t MEMORY_SPACE_SIZE = 0x1000000;
    Abstract::Memory::MEMiHeapHead::OptFlag opt;
    opt.setBit(Abstract::Memory::MEMiHeapHead::eOptFlag::ZeroFillAlloc);

#ifdef BUILD_DEBUG
    opt.setBit(Abstract::Memory::MEMiHeapHead::eOptFlag::DebugFillAlloc);
#endif

    m_memorySpace = malloc(MEMORY_SPACE_SIZE);
    m_rootHeap = EGG::ExpHeap::create(m_memorySpace, MEMORY_SPACE_SIZE, opt);
    m_rootHeap->setName("EGGRoot");
    m_rootHeap->becomeCurrentHeap();
}

/// @brief The main loop of the program.
/// @addr{0x8000951C}
void KSystem::run() {
    while (true) {
	calc();
    }
}

/// @brief Function not present in the original game. Useful for stepping the game one frame
void KSystem::calc() {
    m_sceneMgr->calc();
}

EGG::Heap *KSystem::rootHeap() const {
    return m_rootHeap;
}

/// @addr{0x80008E84}
KSystem &KSystem::Instance() {
    static KSystem instance;
    return instance;
}

KSystem::KSystem() = default;

KSystem::~KSystem() = default;

} // namespace Host
