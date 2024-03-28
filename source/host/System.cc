#include "System.hh"

#include "host/SceneCreatorDynamic.hh"

#include <abstract/File.hh>

#include <cstring>

namespace Host {

int KSystem::main(int argc, char **argv) {
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
    return run() ? 0 : 1;
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
        m_binaryData = std::span<u8>(data, size);
    } break;
    case Option::Invalid:
        break;
    }
}

void KSystem::init() {
    auto *sceneCreator = new SceneCreatorDynamic;
    m_sceneMgr = new EGG::SceneManager(sceneCreator);

    if (m_binaryData.size() > 0) {
        m_testDirector = new Test::TestDirector(m_binaryData);
    } else {
        K_PANIC("Failed to load binary data!");
    }
}

bool KSystem::run() {
    while (m_testDirector->calc()) {
        m_sceneMgr->calc();
    }

    m_testDirector->printTestOutput();
    return true;
}

const Test::TestDirector *KSystem::testDirector() const {
    return m_testDirector;
}

KSystem &KSystem::Instance() {
    static KSystem instance;
    return instance;
}

KSystem::KSystem() {}

KSystem::~KSystem() = default;

} // namespace Host
