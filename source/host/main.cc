#include "source/host/System.hh"

int main() {
    auto *system = HostSystem::CreateInstance();
    system->create();
    system->init();

    return 0;
}
