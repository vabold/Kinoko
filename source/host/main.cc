#include "host/KReplaySystem.hh"
#include "host/KTestSystem.hh"
#include "host/Option.hh"

#include <Version.hh>

#include <egg/core/ExpHeap.hh>

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

static void *s_memorySpace = nullptr;
static EGG::Heap *s_rootHeap = nullptr;

static void InitMemory() {
    constexpr size_t MEMORY_SPACE_SIZE = 0x1000000;
    Abstract::Memory::MEMiHeapHead::OptFlag opt;
    opt.setBit(Abstract::Memory::MEMiHeapHead::eOptFlag::ZeroFillAlloc);

#ifdef BUILD_DEBUG
    opt.setBit(Abstract::Memory::MEMiHeapHead::eOptFlag::DebugFillAlloc);
#endif

    s_memorySpace = malloc(MEMORY_SPACE_SIZE);
    s_rootHeap = EGG::ExpHeap::create(s_memorySpace, MEMORY_SPACE_SIZE, opt);
    s_rootHeap->setName("EGGRoot");
    s_rootHeap->becomeCurrentHeap();

    EGG::SceneManager::SetRootHeap(s_rootHeap);
}

static void DisplayVersion() {
    PRINT("Kinoko v%s\nCommit: %s\n", GIT_VERSION, GIT_COMMIT_HASH);
}

int main(int argc, char **argv) {
    FlushDenormalsToZero();
    InitMemory();

    // The hashmap cannot be constexpr, as it heap-allocates
    // Therefore, it cannot be static, as memory needs to be initialized first
    // TODO: Allow memory initialization before any other static initializers
    const std::unordered_map<std::string, std::function<KSystem *()>> modeMap = {
            {"test", []() -> KSystem * { return KTestSystem::CreateInstance(); }},
            {"replay", []() -> KSystem * { return KReplaySystem::CreateInstance(); }},
    };

    // The first argument is the executable, so we ignore it
    // The second argument is the mode flag, or the version flag
    // The third argument is the mode arg
    // TODO: Iterate until we find the index of the mode flag
    std::optional<Host::EOption> flag = Host::Option::CheckFlag(argv[1]);
    if (!flag) {
        PANIC("Not a flag!");
    }

    if (*flag == Host::EOption::Version) {
        DisplayVersion();
        return 0;
    }

    if (*flag != Host::EOption::Mode) {
        PANIC("First flag expected to be mode!");
    }

    if (argc < 3) {
        PANIC("Too few arguments!");
    }

    KSystem *sys = nullptr;
    const std::string mode = argv[2];

    auto it = modeMap.find(mode);
    if (it != modeMap.end()) {
        sys = it->second();
    } else {
        PANIC("Invalid mode!");
    }

    sys->parseOptions(argc - 3, argv + 3);
    sys->init();
    return sys->run() ? 0 : 1;
}
