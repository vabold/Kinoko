#include "host/KReplaySystem.hh"
#include "host/KTestSystem.hh"
#include "host/Option.hh"

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
    s_memorySpace = malloc(MEMORY_SPACE_SIZE);
    s_rootHeap = EGG::ExpHeap::create(s_memorySpace, MEMORY_SPACE_SIZE, DEFAULT_OPT);
    s_rootHeap->setName("EGGRoot");
    s_rootHeap->becomeCurrentHeap();

    EGG::SceneManager::SetRootHeap(s_rootHeap);
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

    if (argc < 2) {
        PANIC("Too few arguments!");
    }

    KSystem *sys = nullptr;

    // The first argument is the executable, so we ignore it
    // The second argument is the mode
    const std::string mode = argv[1];

    auto it = modeMap.find(mode);
    if (it != modeMap.end()) {
        sys = it->second();
    } else {
        PANIC("Invalid mode!");
    }

    sys->parseOptions(argc - 2, argv + 2);
    sys->init();
    return sys->run() ? 0 : 1;
}
