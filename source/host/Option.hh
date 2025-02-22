#pragma once

#include <Common.hh>

#include <optional>

namespace Host {

enum class EOption {
    Invalid = -1,
    Mode,
    Suite,
    Ghost,
};

namespace Option {

[[nodiscard]] std::optional<EOption> CheckFlag(const char *arg);

} // namespace Option
} // namespace Host
