#include "KSystem.hh"

#include <cstdarg>

namespace Kinoko {

KSystem *KSystem::s_instance = nullptr;

/// @brief Prints an error message followed by the system's help message, then exits.
/// @param fmt The printf-style format of the error message.
void KSystem::exitWithHelp(const char *fmt, ...) const {
    va_list args;
    va_start(args, fmt);
    printf("Error: ");
    vprintf(fmt, args);
    printf("\n\n");
    va_end(args);

    printHelp();
    exit(1);
}

} // namespace Kinoko
