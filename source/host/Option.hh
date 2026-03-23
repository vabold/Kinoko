#pragma once

#include <Common.hh>

#include <optional>

namespace Kinoko::Host {

enum class EOption {
    Invalid = -1,
    Mode,
    Suite,
    Ghost,
    KRKG,
    TargetFrame,
};

namespace Option {

std::optional<EOption> CheckFlag(const char *arg);

} // namespace Option
} // namespace Kinoko::Host
