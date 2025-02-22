#include "Option.hh"

#include <cstring>

namespace Host::Option {

std::optional<EOption> CheckFlag(const char *arg) {
    ASSERT(arg);
    if (arg[0] != '-') {
        return std::nullopt;
    }

    // Verbose flag
    if (arg[1] == '-') {
        const char *verbose_arg = &arg[2];

        if (strcmp(verbose_arg, "mode") == 0) {
            return EOption::Mode;
        }

        if (strcmp(verbose_arg, "suite") == 0) {
            return EOption::Suite;
        }

        if (strcmp(verbose_arg, "ghost") == 0) {
            return EOption::Ghost;
        }

        if (strcmp(verbose_arg, "progress") == 0) {
            return EOption::Progress;
        }

        return EOption::Invalid;
    } else {
        switch (arg[1]) {
        case 'M':
        case 'm':
            return EOption::Mode;
        case 'S':
        case 's':
            return EOption::Suite;
        case 'G':
        case 'g':
            return EOption::Ghost;
        case 'P':
        case 'p':
            return EOption::Progress;
        default:
            return EOption::Invalid;
        }
    }

    // This is unreachable
    return std::nullopt;
}

} // namespace Host::Option
