#pragma once

#include <egg/core/SceneManager.hh>
#include <test/TestDirector.hh>

#include <span>

namespace Host {

/// @addr{0x802A4080}
class KSystem {
private:
    enum class Option {
        Invalid = -1,
        Suite,
    };

public:
    int main(int argc, char **argv);
    [[nodiscard]] Option option(char *arg);
    void handleOption(Option opt, int argc, char **argv, int &i);
    void init();
    [[nodiscard]] bool run();

    [[nodiscard]] const Test::TestDirector *testDirector() const;

    [[nodiscard]] static KSystem &Instance();

private:
    KSystem();
    KSystem(const KSystem &) = delete;
    KSystem(KSystem &&) = delete;
    ~KSystem();

    std::span<u8> m_suiteData;
    EGG::SceneManager *m_sceneMgr;
    Test::TestDirector *m_testDirector;
};

} // namespace Host
