#pragma once

#include <Logger.hh>

#include <cstdint>
#include <span>
#include <type_traits>

namespace Kinoko {

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

/// @brief A contiguous storage container that manages the lifecycle of a buffer of a given size.
/// @details Similar to std::unique_ptr in that we guarantee memory safety, however owning_span
/// points to a buffer rather than an object.
/// @tparam T type contained within the buffer
template <typename T>
class owning_span {
public:
    /// @brief Uninitialized buffer
    owning_span() : m_data(nullptr), m_size(0) {}

    /// @brief Allocates a buffer of T elements. Does not initialize any elements.
    owning_span(size_t count) : m_data(new T[count]), m_size(count) {}

    /// @brief Performs a deep copy from a std::span of const T
    /// @details Will compile to a memcpy for trivially copyable types
    owning_span(const std::span<const T> &span) : m_data(new T[span.size()]), m_size(span.size()) {
        std::copy(span.begin(), span.end(), m_data);
    }

    /// @brief Copy constructor
    /// @details Performs a deep copy
    owning_span(const owning_span &rhs) : m_size(rhs.m_size) {
        m_data = new T[m_size];
        std::copy(rhs.begin(), rhs.end(), m_data);
    }

    /// @brief Move constructor
    /// @details Transfers ownership of the buffer and leaves rhs in an invalid state
    owning_span(owning_span &&rhs) {
        m_data = rhs.m_data;
        rhs.m_data = nullptr;

        m_size = rhs.m_size;
        rhs.m_size = 0;
    }

    /// @brief Copy assignment operator
    /// @details Deletes the existing buffer and performs a deep copy
    owning_span &operator=(const owning_span &rhs) {
        if (this != &rhs) {
            delete[] m_data;
            m_size = rhs.m_size;
            m_data = new T[m_size];
            std::copy(rhs.begin(), rhs.end(), m_data);
        }

        return *this;
    }

    /// @brief Move assignment operator
    /// @details Transfers ownership of the buffer and leaves rhs in an invalid state
    owning_span &operator=(owning_span &&rhs) {
        if (this != &rhs) {
            delete[] m_data;
            m_data = rhs.m_data;
            rhs.m_data = nullptr;
            m_size = rhs.m_size;
            rhs.m_size = 0;
        }

        return *this;
    }

    /// @brief Destroys the underlying buffer on teardown
    ~owning_span() {
        delete[] m_data;
    }

    /// @brief Indexes into the underlying buffer
    /// @param idx The index of the element in the buffer to retrieve
    [[nodiscard]] T &operator[](size_t idx) {
        ASSERT(idx < m_size);
        return m_data[idx];
    }

    /// @brief Indexes into the underlying buffer
    /// @param idx The index of the element in the buffer to retrieve
    [[nodiscard]] const T &operator[](size_t idx) const {
        ASSERT(idx < m_size);
        return m_data[idx];
    }

    /// @brief Retrieves the first element in the buffer
    [[nodiscard]] T &front() {
        ASSERT(m_size > 0);
        return m_data[0];
    }

    /// @brief Retrieves the first element in the buffer
    [[nodiscard]] const T &front() const {
        ASSERT(m_size > 0);
        return m_data[0];
    }

    /// @brief Retrieves the last element in the buffer
    [[nodiscard]] T &back() {
        ASSERT(m_size > 0);
        return m_data[m_size - 1];
    }

    /// @brief Retrieves the last element in the buffer
    [[nodiscard]] const T &back() const {
        ASSERT(m_size > 0);
        return m_data[m_size - 1];
    }

    [[nodiscard]] T *begin() {
        return m_data;
    }

    [[nodiscard]] T *end() {
        return m_data + m_size;
    }

    [[nodiscard]] const T *begin() const {
        return m_data;
    }

    [[nodiscard]] const T *end() const {
        return m_data + m_size;
    }

    /// @brief Returns true if the buffer is uninitialized
    [[nodiscard]] bool empty() const {
        return m_size == 0;
    }

    /// @brief Returns the number of elements that fit in the buffer
    [[nodiscard]] size_t size() const {
        return m_size;
    }

    /// @brief Returns a read-only view of the entire buffer
    [[nodiscard]] std::span<const T> view() const {
        return {m_data, m_size};
    }

private:
    T *m_data;     ///< Pointer to the underlying buffer
    size_t m_size; ///< The number of T elements that fit in the buffer
};

} // namespace Kinoko
