#include "File.hh"

#include <fstream>

namespace Abstract::File {

std::filesystem::path Path(const char *path) {
    char filepath[256];

    if (path[0] == '/') {
        path++;
    }

    snprintf(filepath, sizeof(filepath), "%s", path);
    return std::filesystem::path(filepath);
}

u8 *Load(const std::filesystem::path &path, size_t &size) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        PANIC("File with provided path %s was not loaded correctly!", path.c_str());
    }

    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);

    u8 *buffer = new u8[size];
    file.read(reinterpret_cast<char *>(buffer), size);

    return buffer;
}

u8 *Load(const char *path, size_t &size) {
    return Load(Path(path), size);
}

void Append(const char *path, const char *data, size_t size) {
    std::ofstream stream;
    stream.open(path, std::ios::app | std::ios::binary);
    stream.write(data, size);
}

int Remove(const char *path) {
    return std::remove(path);
}

} // namespace Abstract::File
