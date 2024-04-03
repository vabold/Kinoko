#include "File.hh"

#include <fstream>

namespace Abstract::File {

u8 *Load(const char *path, size_t &size) {
    char filepath[256];

    if (path[0] == '/') {
        path++;
    }

    snprintf(filepath, sizeof(filepath), "./%s", path);
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        K_PANIC("File with provided path %s was not loaded correctly!", path);
    }

    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);

    u8 *buffer = new u8[size];
    file.read(reinterpret_cast<char *>(buffer), size);

    return buffer;
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
