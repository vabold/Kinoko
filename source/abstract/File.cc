#include "File.hh"

#include <fstream>

namespace Abstract::File {

u8 *Load(const char *path) {
    std::ifstream file(path, std::ios::binary);
    assert(file);

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    u8 *buffer = new u8[size];
    file.read(reinterpret_cast<char *>(buffer), size);

    return buffer;
}

} // namespace Abstract::File
