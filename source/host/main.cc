#include "host/KReplaySystem.hh"
#include "host/KTestSystem.hh"
#include "host/Option.hh"

#include <egg/core/ExpHeap.hh>

#include <algorithm>
#include <array>
#include <string_view>
#include <utility>

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

using ModeEntry = std::pair<std::string_view, KSystem *(*)()>;

static constexpr std::array<ModeEntry, 2> MODE_MAP = {{
        {"test", []() -> KSystem * { return KTestSystem::CreateInstance(); }},
        {"replay", []() -> KSystem * { return KReplaySystem::CreateInstance(); }},
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

int main(int argc, char **argv) {
    FlushDenormalsToZero();
    InitMemory();

    if (argc < 2) {
        PANIC("Too few arguments!");
    }

    // The first argument is the executable, so we ignore it
    // The second argument is the mode
    const std::string_view mode = argv[1];

    auto it = std::ranges::find(MODE_MAP, mode, &ModeEntry::first);
    if (it == MODE_MAP.end()) {
        PANIC("Invalid mode!");
    }

    KSystem *sys = it->second();
    sys->parseOptions(argc - 2, argv + 2);
    sys->init();
    return sys->run() ? 0 : 1;
}
