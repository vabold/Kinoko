#pragma once

#include <Common.hh>

#include <filesystem>

namespace Abstract::File {

[[nodiscard]] std::filesystem::path Path(const char *path);
[[nodiscard]] u8 *Load(const std::filesystem::path &path, size_t &size);
[[nodiscard]] u8 *Load(const char *path, size_t &size);
void Append(const char *path, const char *data, size_t size);
int Remove(const char *path);

} // namespace Abstract::File
