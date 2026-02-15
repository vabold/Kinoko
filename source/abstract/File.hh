#pragma once

#include <Common.hh>

namespace Abstract::File {

/// @brief Loads kinoko game files
[[nodiscard]] u8 *Load(const char *path, size_t &size);
/// @brief Loads files from host system
[[nodiscard]] u8 *LoadHost(const char *path, size_t &size);
void Append(const char *path, const char *data, size_t size);
int Remove(const char *path);

} // namespace Abstract::File
