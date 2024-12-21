#include <host/System.hh>

#include <test/TestDirector.hh>

#include <abstract/File.hh>

#include <cstring>
#include <fstream>
#include <iostream>

u8 *loadFile(const char *path, size_t &size) {
    char filepath[256];

    if (path[0] == '/') {
        path++;
    }

    snprintf(filepath, sizeof(filepath), "./%s", path);
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        PANIC("Failed to open file %s!", path);
    }

    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);

    u8 *buffer = new u8[size];
    std::cout << static_cast<void*>(buffer) << std::endl;
    file.read(reinterpret_cast<char *>(buffer), size);

    return buffer;
}

enum class Option {
    Invalid = -1,
    Suite,
};

Option option(char *arg) {
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

void handleOption(Option opt, int argc, char **argv, int &i, std::span<u8>& suiteData) {
    switch (opt) {
    case Option::Suite: {
        // We expect another arg following the option
        if (argc - i < 2) {
            PANIC("Expected argument after %s", argv[i]);
        }

        size_t size;
        u8 *data = loadFile(argv[++i], size);

        if (size == 0) {
            PANIC("Failed to load suite data!");
        }

        suiteData = std::span<u8>(data, size);
    } break;
    case Option::Invalid:
        break;
    }
}

int main(int argc, char **argv) {
    FlushDenormalsToZero();
    Host::KSystem& ksystem = Host::KSystem::Instance();
    ksystem.initMemory();

    std::span<u8> suiteData;
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

        handleOption(opt, argc, argv, i, suiteData);
    }

    Test::TestDirector *testDirector = new Test::TestDirector(suiteData);
    delete[] suiteData.data();

    ksystem.init();
    // Register the RaceConfig callback
    System::RaceConfig::RegisterInitCallback(std::bind(&Test::TestDirector::OnInit, testDirector, std::placeholders::_1, std::placeholders::_2), nullptr);
    EGG::SceneManager *sceneMgr = ksystem.sceneManager();
    sceneMgr->changeScene(0);

    bool success = true;
    while (true) {
        // complete a run
        while (testDirector->calc()) {
            ksystem.calc();
        }

        testDirector->writeTestOutput();
        success &= testDirector->sync();

        if (!testDirector->popTestCase()) {
            break;
        }

        // TODO: Use a system heap! The test director currently has a dependency on the scene heap.
        sceneMgr->destroyScene(sceneMgr->currentScene());
        testDirector->init();
        sceneMgr->createScene(2, sceneMgr->currentScene());
    }

    return success ? 0 : 1;
}
