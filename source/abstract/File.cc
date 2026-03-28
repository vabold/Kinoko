#include "File.hh"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

namespace Kinoko::Abstract::File {

u8 *Load(const char *path, size_t &size) {
    char filepath[256];

    if (path[0] == '/') {
        path++;
    }

    const char *root = std::getenv("KINOKO_FILESYSTEM_ROOT");

    if (root && root[0] != '\0') {
        // path relative to KINOKO_FILESYSTEM_ROOT
#ifdef _WIN32
        const char separator = '\\';
#else
        const char separator = '/';
#endif

        size_t rootLen = std::strlen(root);
        bool rootEndsWithSep = root[rootLen - 1] == separator;

        if (rootEndsWithSep) {
            std::snprintf(filepath, sizeof(filepath), "%s%s", root, path);
        } else {
            std::snprintf(filepath, sizeof(filepath), "%s%c%s", root, separator, path);
        }
    } else {
        // Path relative to current working directory
        std::snprintf(filepath, sizeof(filepath), "./%s", path);
    }

    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        PANIC("Failed to load file %s! (Was KINOKO_FILESYSTEM_ROOT specified properly?)", path);
    }

    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);

    u8 *buffer = new u8[size];
    file.read(reinterpret_cast<char *>(buffer), size);

    return buffer;
}

u8 *LoadHost(const char *path, size_t &size) {
    char filepath[256];

    std::snprintf(filepath, sizeof(filepath), "./%s", path);

    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        PANIC("Failed to load file %s!", path);
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

} // namespace Kinoko::Abstract::File
