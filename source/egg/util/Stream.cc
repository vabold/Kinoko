#include "Stream.hh"

namespace EGG {

Stream::Stream() : m_endian(std::endian::big), m_index(0) {}

Stream::~Stream() = default;

void Stream::skip(u32 count) {
    m_index += count;
    ASSERT(!eof());
}

void Stream::jump(u32 index) {
    m_index = index;
    ASSERT(!eof());
}

void Stream::setEndian(std::endian endian) {
    m_endian = endian;
}

u32 Stream::index() const {
    return m_index;
}

u8 Stream::read_u8() {
    return read<u8>();
}

u16 Stream::read_u16() {
    return read<u16>();
}

u32 Stream::read_u32() {
    return read<u32>();
}

u64 Stream::read_u64() {
    return read<u64>();
}

s8 Stream::read_s8() {
    return read<s8>();
}

s16 Stream::read_s16() {
    return read<s16>();
}

s32 Stream::read_s32() {
    return read<s32>();
}

s64 Stream::read_s64() {
    return read<s64>();
}

f32 Stream::read_f32() {
    return read<f32>();
}

f64 Stream::read_f64() {
    return read<f64>();
}

RamStream::RamStream() : m_buffer(nullptr), m_size(0) {}

RamStream::RamStream(u8 *buffer, u32 size) {
    setBufferAndSize(buffer, size);
}

RamStream::~RamStream() = default;

void RamStream::read(void *output, u32 size) {
    u8 *buffer = reinterpret_cast<u8 *>(output);
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = m_buffer[m_index + i];
    }
}

void RamStream::write(void *input, u32 size) {
    u8 *buffer = reinterpret_cast<u8 *>(input);
    for (size_t i = 0; i < size; ++i) {
        m_buffer[m_index + i] = buffer[i];
    }
}

bool RamStream::eof() {
    return m_index > m_size;
}

// Expects a null-terminated char array, and moves the index past the null terminator
std::string RamStream::read_string() {
    std::string ret(reinterpret_cast<char *>(m_buffer + m_index));
    m_index += ret.size() + 1;
    ASSERT(!eof());
    return ret;
}

void RamStream::setBufferAndSize(void *buffer, u32 size) {
    m_buffer = reinterpret_cast<u8 *>(buffer);
    m_size = size;
}

void RamStream::extendSize(u32 size) {
    m_size += size;
}

u8 *RamStream::data() {
    return m_buffer;
}

u8 *RamStream::dataAtIndex() {
    return m_buffer + m_index;
}

/// @brief Splits the current stream into two.
/// @details Segments the current stream at the current index. The returned stream is the data from
/// the current index to size bytes after. The current stream is then moved to `size` bytes after
/// the current index.
RamStream RamStream::split(u32 size) {
    RamStream stream = RamStream(m_buffer + m_index, size);
    m_index += size;
    ASSERT(!eof());

    return stream;
}

} // namespace EGG
