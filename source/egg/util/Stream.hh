#pragma once

#include <Common.hh>

#include <string>

namespace EGG {

class Stream {
public:
    Stream();
    virtual ~Stream();

    virtual void read(void *output, u32 size) = 0;
    virtual void write(void *input, u32 size) = 0;
    virtual bool eof() = 0;

    void skip(u32 count);
    void jump(u32 index);

    void setEndian(std::endian endian);

    u32 index() const;

    u8 read_u8();
    u16 read_u16();
    u32 read_u32();
    u64 read_u64();
    s8 read_s8();
    s16 read_s16();
    s32 read_s32();
    s64 read_s64();
    f32 read_f32();
    f64 read_f64();

protected:
    std::endian m_endian;
    u32 m_index;

private:
    template <ParseableType T>
    T read() {
        T val;
        read(&val, sizeof(val));
        m_index += sizeof(val);
        assert(!eof());

        return parse<T>(val, m_endian);
    }
};

class RamStream : public Stream {
public:
    RamStream();
    RamStream(u8 *buffer, u32 size);
    ~RamStream() override;

    void read(void *output, u32 size) override;
    void write(void *input, u32 size) override;
    bool eof() override;

    std::string read_string();
    RamStream split(u32 size);
    void setBufferAndSize(void *buffer, u32 size);

private:
    u8 *m_buffer;
    u32 m_size;
};

} // namespace EGG
