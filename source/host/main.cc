#include "source/host/System.hh"

int main() {
    auto *system = Host::System::CreateInstance();
    system->init();

    return 0;
}
