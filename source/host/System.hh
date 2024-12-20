#pragma once

#include <egg/core/SceneManager.hh>

#if defined(__arm64__) || defined(__aarch64__)
inline void FlushDenormalsToZero() {
    uint64_t fpcr;
    asm("mrs %0,   fpcr" : "=r"(fpcr));
    asm("msr fpcr, %0" ::"r"(fpcr | (1 << 24)));
}
#elif defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>

inline void FlushDenormalsToZero() {
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
}
#endif

namespace Host {

/// @addr{0x802A4080}
class KSystem {
public:
    void main(int argc, char **argv);
    void init();
    void initMemory();
    void run();
    void calc();

    [[nodiscard]] EGG::Heap *rootHeap() const;
    [[nodiscard]] const EGG::SceneManager *sceneManager() const { return m_sceneMgr; };
    [[nodiscard]] EGG::SceneManager *sceneManager() { return m_sceneMgr; };

    [[nodiscard]] static KSystem &Instance();

private:
    KSystem();
    KSystem(const KSystem &) = delete;
    KSystem(KSystem &&) = delete;
    ~KSystem();

    void *m_memorySpace;
    EGG::Heap *m_rootHeap;
    EGG::SceneManager *m_sceneMgr;
};

} // namespace Host
