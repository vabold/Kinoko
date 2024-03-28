#include <Common.hh>

namespace Abstract::File {

u8 *Load(const char *path, size_t &size);
void Append(const char *path, const char *data, size_t size);
int Remove(const char *path);

} // namespace Abstract::File
