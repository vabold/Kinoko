#include "source/host/System.hh"

int main() {
    // TODO: Use command line to initialize controller count
    auto *system = HostSystem::CreateInstance(0, 1);
    // TODO: Use command line to send characters/vehicles
    system->initControllers();

    system->create();
    system->init();

    return 0;
}
