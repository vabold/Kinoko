#include "System.hh"

#include "host/SceneCreatorDynamic.hh"

#include <abstract/File.hh>

#include <egg/core/ExpHeap.hh>

#include <cstring>

namespace Host {

/// @brief The main entry point for the program.
/// @addr{0x80008EF0}
int KSystem::main(int argc, char **argv) {
    initMemory();

    if (argc < 2) {
        PANIC("Expected file argument");
    }

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            PANIC("Invalid argument: %s", argv[i]);
        }

        Option opt = option(argv[i]);
        if (opt == Option::Invalid) {
            PANIC("Invalid option %s", argv[i]);
        }

        handleOption(opt, argc, argv, i);
    }

    init();
    m_sceneMgr->changeScene(0);
    bool success = true;

    while (true) {
        success &= run();

        if (!m_testDirector->popTestCase()) {
            break;
        }

        // TODO: Use a system heap! The test director currently has a dependency on the scene heap.
        m_sceneMgr->destroyScene(m_sceneMgr->currentScene());
        m_testDirector->init();
        m_sceneMgr->createScene(2, m_sceneMgr->currentScene());
    }

    // Shut down the RootScene
    m_sceneMgr->destroyToSelectSceneId(0);

    return success ? 0 : 1;
}

KSystem::Option KSystem::option(char *arg) {
    switch (arg[1]) {
    case '-':
        break;
    case 's':
        return Option::Suite;
    case '\0':
    default:
        return Option::Invalid;
    }

    if (strlen(arg) <= 2) {
        PANIC("Invalid argument: %s", arg);
    }

    if (strcmp(arg, "--suite") == 0) {
        return Option::Suite;
    } else {
        return Option::Invalid;
    }
}

void KSystem::handleOption(Option opt, int argc, char **argv, int &i) {
    switch (opt) {
    case Option::Suite: {
        // We expect another arg following the option
        if (argc - i < 2) {
            PANIC("Expected argument after %s", argv[i]);
        }

        size_t size;
        u8 *data = Abstract::File::Load(argv[++i], size);

        if (size == 0) {
            PANIC("Failed to load suite data!");
        }

        m_suiteData = std::span<u8>(data, size);
    } break;
    case Option::Invalid:
        break;
    }
}

/// @addr{0x80009194}
void KSystem::init() {
    auto *sceneCreator = new SceneCreatorDynamic;
    m_sceneMgr = new EGG::SceneManager(sceneCreator);
    m_testDirector = new Test::TestDirector(m_suiteData);
    delete[] m_suiteData.data();
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
/// @return Whether or not all test cases have passed.
/// @addr{0x8000951C}
bool KSystem::run() {
    while (m_testDirector->calc()) {
        m_sceneMgr->calc();
    }

    m_testDirector->writeTestOutput();
    return m_testDirector->sync();
}

EGG::Heap *KSystem::rootHeap() const {
    return m_rootHeap;
}

const Test::TestDirector *KSystem::testDirector() const {
    return m_testDirector;
}

/// @addr{0x80008E84}
KSystem &KSystem::Instance() {
    static KSystem instance;
    return instance;
}

KSystem::KSystem() = default;

KSystem::~KSystem() = default;

} // namespace Host
