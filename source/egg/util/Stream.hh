#pragma once

#include <Common.hh>

#include <string>

namespace EGG {

/// @brief A stream of data, abstracted to allow for continuous seeking.
class Stream {
public:
    Stream();
    virtual ~Stream();

    virtual void read(void *output, u32 size) = 0;
    virtual void write(void *input, u32 size) = 0;
    virtual bool eof() const = 0;
    virtual bool safe(u32 size) const = 0;
    virtual bool bad() const = 0;

    void skip(u32 count);
    void jump(u32 index);

    void setEndian(std::endian endian) {
        m_endian = endian;
    }

    [[nodiscard]] u32 index() const {
        return m_index;
    }

    [[nodiscard]] u8 read_u8();
    [[nodiscard]] u16 read_u16();
    [[nodiscard]] u32 read_u32();
    [[nodiscard]] u64 read_u64();
    [[nodiscard]] s8 read_s8();
    [[nodiscard]] s16 read_s16();
    [[nodiscard]] s32 read_s32();
    [[nodiscard]] s64 read_s64();
    [[nodiscard]] f32 read_f32();
    [[nodiscard]] f64 read_f64();

protected:
    std::endian m_endian;
    u32 m_index;

private:
    template <ParseableType T>
    [[nodiscard]] T read() {
        ASSERT(safe(sizeof(T)));
        T val;
        read(&val, sizeof(val));
        m_index += sizeof(val);

        return parse<T>(val, m_endian);
    }
};

/// @brief A stream of data stored in memory.
/// @details RamStream solves two problems:
/// 1. Inconsistent endianness across Kinoko clients.
/// 2. Having to maintain pointer arithmetic while reading in various data types.
/// We specify the endianness of the data in the stream, and the stream will handle the rest.
class RamStream : public Stream {
public:
    RamStream();
    RamStream(const void *buffer, u32 size);
    ~RamStream() override = default;

    void read(void *output, u32 size) override;
    void write(void *input, u32 size) override;

    [[nodiscard]] bool eof() const override {
        return m_index == m_size;
    }

    [[nodiscard]] bool safe(u32 size) const override {
        return m_index + size <= m_size;
    }

    [[nodiscard]] bool bad() const override {
        return m_index > m_size;
    }

    [[nodiscard]] std::string read_string();
    [[nodiscard]] RamStream split(u32 size);
    void setBufferAndSize(void *buffer, u32 size);

    [[nodiscard]] u8 *data() {
        return m_buffer;
    }

    [[nodiscard]] u8 *dataAtIndex() {
        return m_buffer + m_index;
    }

private:
    u8 *m_buffer;
    u32 m_size;
};

} // namespace EGG
