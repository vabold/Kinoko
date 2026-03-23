#pragma once

#include <Common.hh>

namespace Kinoko::Abstract::File {

[[nodiscard]] u8 *Load(const char *path, size_t &size);
void Append(const char *path, const char *data, size_t size);
int Remove(const char *path);

} // namespace Kinoko::Abstract::File
