#include "System.hh"

#include "host/SceneCreatorDynamic.hh"

#include <abstract/File.hh>

#include <cstring>

namespace Host {

/// @brief The main entry point for the program.
/// @addr{0x80008EF0}
int KSystem::main(int argc, char **argv) {
    if (argc < 2) {
        K_PANIC("Expected file argument");
    }

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            K_PANIC("Invalid argument: %s", argv[i]);
        }

        Option opt = option(argv[i]);
        if (opt == Option::Invalid) {
            K_PANIC("Invalid option %s", argv[i]);
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

        m_testDirector->init();
        m_sceneMgr->reinitCurrentScene();
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
        K_PANIC("Invalid argument: %s", arg);
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
            K_PANIC("Expected argument after %s", argv[i]);
        }

        size_t size;
        u8 *data = Abstract::File::Load(argv[++i], size);

        if (size == 0) {
            K_PANIC("Failed to load suite data!");
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
