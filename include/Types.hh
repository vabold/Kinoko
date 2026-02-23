#pragma once

#include <Logger.hh>

#include <array>
#include <cstdint>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

/// @brief Similar to std::queue but with a fixed storage container to avoid reallocation.
/// @details Wrapper around an std::array. Keeps track of the "front" index which represents the
/// front of the queue, and keeps tracks of how many items are in the queue. Can only fit N items,
/// so it is expected the the caller checks the current size before pushing.
/// @tparam T The type stored in the queue
/// @tparam N The maximum size of the queue
template <typename T, size_t N>
class fixed_queue {
public:
    constexpr fixed_queue() : m_frontIdx(0), m_count(0) {}
    constexpr ~fixed_queue() = default;

    constexpr void push(T &&elem) {
        ASSERT(m_count < N);
        new (&back()) T(std::move(elem));
        ++m_count;
    }

    [[nodiscard]] constexpr T &front() {
        ASSERT(m_count > 0);
        return m_arr[m_frontIdx];
    }

    constexpr void pop() {
        ASSERT(m_count > 0);
        delete (&m_arr[m_frontIdx]);
        m_frontIdx = (m_frontIdx + 1) % N;
        --m_count;
    }

    [[nodiscard]] constexpr size_t size() const {
        return m_count;
    }

    [[nodiscard]] constexpr size_t capacity() const {
        return N;
    }

    [[nodiscard]] constexpr bool empty() const {
        return m_count == 0;
    }

private:
    [[nodiscard]] constexpr T &back() {
        size_t idx = (m_frontIdx + m_count) % N;
        return m_arr[idx];
    }

    std::array<T, N> m_arr; ///< The underlying storage container
    size_t m_frontIdx;      ///< Index of the front of the queue
    size_t m_count;         ///< Number of elements in the queue
};
