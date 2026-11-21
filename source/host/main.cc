#include "host/KReplaySystem.hh"
#include "host/KTestSystem.hh"
#include "host/Option.hh"

#include <egg/core/ExpHeap.hh>

#include <algorithm>
#include <array>
#include <string_view>

using namespace Kinoko;

#if defined(__arm64__) || defined(__aarch64__)
static void FlushDenormalsToZero() {
    uint64_t fpcr;
    asm("mrs %0,   fpcr" : "=r"(fpcr));
    asm("msr fpcr, %0" ::"r"(fpcr | (1 << 24)));
}
#elif defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>

static void FlushDenormalsToZero() {
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
}
#endif

struct ModeEntry {
    std::string_view name;
    std::string_view description;
    KSystem *(*create)();
};

static constexpr std::array<ModeEntry, 2> MODE_MAP = {{
        {"test", "Validate ghosts frame-by-frame against reference data",
                []() -> KSystem * { return KTestSystem::CreateInstance(); }},
        {"replay", "Replay a ghost and verify its pre-lap and final race times",
                []() -> KSystem * { return KReplaySystem::CreateInstance(); }},
}};

static void *s_memorySpace = nullptr;
static EGG::Heap *s_rootHeap = nullptr;

static void InitMemory() {
    s_memorySpace = malloc(MEMORY_SPACE_SIZE);
    s_rootHeap = EGG::ExpHeap::create(s_memorySpace, MEMORY_SPACE_SIZE, DEFAULT_OPT);
    s_rootHeap->setName("EGGRoot");
    s_rootHeap->becomeCurrentHeap();

    EGG::SceneManager::SetRootHeap(s_rootHeap);
}

static void PrintHelp() {
    printf("Usage: kinoko <mode> [options]\n"
           "       kinoko help [mode]\n"
           "\n"
           "Modes:\n");

    for (const auto &entry : MODE_MAP) {
        printf("  %-8.*s%.*s\n", static_cast<int>(entry.name.size()), entry.name.data(),
                static_cast<int>(entry.description.size()), entry.description.data());
    }

    printf("\n"
           "Options:\n"
           "  -h, --help  Show this help message\n"
           "\n"
           "Run 'kinoko <mode> --help' for mode-specific options.\n"
           "\n"
           "Environment Variables:\n"
           "  KINOKO_FILESYSTEM_ROOT  Directory containing the game's filesystem's Race folder\n"
           "                          (defaults to the current working directory)\n");
}

static bool IsHelpFlag(const char *arg) {
    return Host::Option::CheckFlag(arg) == Host::EOption::Help;
}

static const ModeEntry *FindMode(std::string_view mode) {
    auto it = std::ranges::find(MODE_MAP, mode, &ModeEntry::name);
    return it != MODE_MAP.end() ? &*it : nullptr;
}

/// @brief Handles `kinoko help [mode]`.
/// @param argc The number of arguments after "help".
/// @param argv The arguments after "help".
/// @return The process exit code.
static int RunHelp(int argc, char **argv) {
    if (argc < 1) {
        PrintHelp();
        return 0;
    }

    const ModeEntry *entry = FindMode(argv[0]);
    if (!entry) {
        printf("Error: Invalid mode '%s'!\n\n", argv[0]);
        PrintHelp();
        return 1;
    }

    entry->create()->printHelp();
    return 0;
}

int main(int argc, char **argv) {
    FlushDenormalsToZero();
    InitMemory();

    if (argc < 2) {
        PrintHelp();
        return 1;
    }

    // The first argument is the executable, so we ignore it
    // The second argument is the mode
    const std::string_view mode = argv[1];

    if (IsHelpFlag(argv[1])) {
        PrintHelp();
        return 0;
    }

    if (mode == "help") {
        return RunHelp(argc - 2, argv + 2);
    }

    const ModeEntry *entry = FindMode(mode);
    if (!entry) {
        printf("Error: Invalid mode '%s'!\n\n", argv[1]);
        PrintHelp();
        return 1;
    }

    KSystem *sys = entry->create();

    if (std::any_of(argv + 2, argv + argc, IsHelpFlag)) {
        sys->printHelp();
        return 0;
    }

    sys->parseOptions(argc - 2, argv + 2);
    sys->init();
    return sys->run() ? 0 : 1;
}
