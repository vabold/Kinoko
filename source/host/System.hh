#pragma once

#include <egg/core/SceneManager.hh>
#include <test/TestDirector.hh>

namespace Host {

class KSystem {
public:
    int main();
    void init();
    bool run();

    static KSystem &Instance();

private:
    KSystem();
    KSystem(const KSystem &) = delete;
    KSystem(KSystem &&) = delete;
    ~KSystem();

    EGG::SceneManager *m_sceneMgr;
    Test::TestDirector *m_testDirector;
};

} // namespace Host
