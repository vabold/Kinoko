#pragma once

/// Based off https://github.com/em-eight/mkw/blob/master/source/game/util/Random.cpp

#include <Common.hh>

namespace System {

class Random {
public:
    Random(u32 seed) : m_x(seed), m_seed(seed) {}

    explicit Random(const Random &rhs) = default;

    ~Random() = default;

    void next() {
        m_x = A * m_x + C;
    }

    [[nodiscard]] u32 getU32() {
        next();
        return static_cast<u32>(m_x >> 0x20);
    }

    [[nodiscard]] u32 getU32(u32 range) {
        next();
        return ((m_x >> 0x20) * range) >> 0x20;
    }

    [[nodiscard]] f32 getF32() {
        return MUL * static_cast<f32>(getU32());
    }

    [[nodiscard]] f32 getF32(f32 range) {
        return range * getF32();
    }

private:
    u64 m_x;
    u64 m_seed;

    static constexpr u64 A = 0x690379B2B2E3D431;
    static constexpr u32 C = 0x508EBD;
    static constexpr f32 MUL = 1.0f / 4294967296.0f; // 1 / (2 ^ 32)
};

} // namespace System
