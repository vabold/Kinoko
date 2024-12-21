#include <Common.hh>

#include <optional>

namespace Host {

enum class EMode {
    Invalid = -1,
    Test,
};

enum class EOption {
    Invalid = -1,
    Mode,
    Suite,
};

namespace Option {

std::optional<EOption> CheckFlag(const char *arg);

EMode CheckModeArg(const char *arg);

} // namespace Option
} // namespace Host
