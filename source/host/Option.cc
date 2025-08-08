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

        if (strcmp(verbose_arg, "suite") == 0) {
            return EOption::Suite;
        }

        if (strcmp(verbose_arg, "ghost") == 0) {
            return EOption::Ghost;
        }

        if (strcmp(verbose_arg, "krkg") == 0) {
            return EOption::KRKG;
        }

        return EOption::Invalid;
    } else {
        switch (arg[1]) {
        case 'S':
        case 's':
            return EOption::Suite;
        case 'G':
        case 'g':
            return EOption::Ghost;
        case 'K':
        case 'k':
            return EOption::KRKG;
        default:
            return EOption::Invalid;
        }
    }

    // This is unreachable
    return std::nullopt;
}

} // namespace Host::Option
