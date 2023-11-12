#pragma once

#include <egg/core/SceneManager.hh>

namespace Host {

class KSystem {
public:
    int main();
    void init();
    void run();

    EGG::SceneManager *sceneMgr() const {
        return m_sceneMgr;
    }

    static KSystem &Instance();

private:
    KSystem();
    KSystem(const KSystem &) = delete;
    KSystem(KSystem &&) = delete;
    ~KSystem();

    EGG::SceneManager *m_sceneMgr;
};

} // namespace Host
