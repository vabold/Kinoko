#pragma once

#include <Logger.hh>

#include <egg/core/Heap.hh>

#include <cstdint>
#include <span>
#include <type_traits>
#include <utility>

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
    owning_span(size_t count) : m_data(EGG::egg_new_array<T>(count)), m_size(count) {}

    /// @brief Performs a deep copy from a std::span of const T
    /// @details Will compile to a memcpy for trivially copyable types
    owning_span(const std::span<const T> &span)
        : m_data(EGG::egg_new_array<T>(span.size())), m_size(span.size()) {
        std::copy(span.begin(), span.end(), m_data);
    }

    /// @brief Copy constructor
    /// @details Performs a deep copy
    owning_span(const owning_span &rhs) : m_size(rhs.m_size) {
        m_data = EGG::egg_new_array<T>(m_size);
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
            EGG::egg_delete_array(m_data, m_size);
            m_size = rhs.m_size;
            m_data = EGG::egg_new_array<T>(m_size);
            std::copy(rhs.begin(), rhs.end(), m_data);
        }

        return *this;
    }

    /// @brief Move assignment operator
    /// @details Transfers ownership of the buffer and leaves rhs in an invalid state
    owning_span &operator=(owning_span &&rhs) {
        if (this != &rhs) {
            EGG::egg_delete_array(m_data, m_size);
            m_data = rhs.m_data;
            rhs.m_data = nullptr;
            m_size = rhs.m_size;
            rhs.m_size = 0;
        }

        return *this;
    }

    /// @brief Destroys the underlying buffer on teardown
    ~owning_span() {
        EGG::egg_delete_array(m_data, m_size);
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

/// @brief Dynamically sized array that only allocates once.
/// @details It's possible that only an upper bound is known for a given vector.
/// However, in the event that there are more objects than expected, we want to error. Effectively,
/// this behaves identically to <tt>std::vector<T, @ref EGG::Allocator<T>></tt> except that it
/// cannot be later resized.
/// @tparam T The type of objects in the array.
template <typename T>
class fixed_vector {
public:
    /// @brief Non-initializing constructor.
    fixed_vector() : m_data(nullptr), m_size(0), m_capacity(0) {}

    /// @brief Initializing constructor.
    /// @param capacity The number of elements to initialize the vector with.
    fixed_vector(size_t capacity) : fixed_vector() {
        allocate(capacity);
    }

    /// @brief Copy constructor
    /// @details Allocates a buffer of the same capacity and deep copies existing elements
    fixed_vector(const fixed_vector &rhs) : fixed_vector() {
        if (rhs.initialized()) {
            allocate(rhs.m_capacity);
            for (size_t i = 0; i < rhs.m_size; ++i) {
                push_back(rhs[i]);
            }
        }
    }

    /// @brief Move constructor
    /// @details Transfers ownership of the buffer and leaves rhs in an invalid state
    fixed_vector(fixed_vector &&rhs)
        : m_data(rhs.m_data), m_size(rhs.m_size), m_capacity(rhs.m_capacity) {
        rhs.m_data = nullptr;
        rhs.m_size = 0;
        rhs.m_capacity = 0;
    }

    /// @brief Copy assignment operator
    /// @details Destroys the existing buffer, then allocates a new one and deep copies
    fixed_vector &operator=(const fixed_vector &rhs) {
        if (this != &rhs) {
            destroy();
            if (rhs.initialized()) {
                allocate(rhs.m_capacity);
                for (size_t i = 0; i < rhs.m_size; ++i) {
                    push_back(rhs[i]);
                }
            }
        }

        return *this;
    }

    /// @brief Move assignment operator
    /// @details Destroys the existing buffer, then transfers ownership from rhs
    fixed_vector &operator=(fixed_vector &&rhs) {
        if (this != &rhs) {
            destroy();

            m_data = rhs.m_data;
            m_size = rhs.m_size;
            m_capacity = rhs.m_capacity;

            rhs.m_data = nullptr;
            rhs.m_size = 0;
            rhs.m_capacity = 0;
        }

        return *this;
    }

    /// @brief Destructor.
    /// @details Destroys all existing elements in the array in-place from the end to the start.
    ~fixed_vector() {
        destroy();
    }

    /// @brief Copies a new element into the array.
    /// @param obj The object to copy.
    /// @return A reference to the object.
    T &push_back(const T &obj) {
        ASSERT(initialized() && !full());
        new (m_data + m_size++) T(obj);
        return back();
    }

    /// @brief Moves a new element into the array.
    /// @param obj The object to move.
    /// @return A reference to the object.
    T &push_back(T &&obj) {
        ASSERT(initialized() && !full());
        new (m_data + m_size++) T(std::move(obj));
        return back();
    }

    /// @brief Creates a new element in-place in the array.
    /// @tparam ...Args Variadic template for packing.
    /// @param ...args Arguments to the constructor.
    /// @return A reference to the object.
    template <typename... Args>
    T &emplace_back(Args &&...args) {
        ASSERT(initialized() && !full());
        new (m_data + m_size++) T(std::forward<Args>(args)...);
        return back();
    }

    /// @brief Deletes the last existing element from the array.
    void pop_back() {
        ASSERT(initialized() && !empty());
        m_data[--m_size].~T();
    }

    /// @brief Initializes the vector with the provided capacity.
    /// @param capacity The number of elements to initialize the vector with.
    void reserve(size_t capacity) {
        ASSERT(!initialized());
        allocate(capacity);
    }

    /// @brief Checks if there are no existing elements in the array.
    /// @return True if the array is empty, otherwise false.
    [[nodiscard]] bool empty() const {
        return m_size == 0;
    }

    /// @brief Checks if all elements exist in the array.
    /// @return True if the array is full, otherwise false.
    [[nodiscard]] bool full() const {
        return m_size == m_capacity;
    }

    /// @brief Checks if the array exists and if the capacity is non-zero.
    /// @return True if the array is initialized, otherwise false.
    [[nodiscard]] bool initialized() const {
        return m_data && m_capacity != 0;
    }

    /// @brief Gets the number of existing elements in the array.
    /// @return The number of existing elements in the array.
    [[nodiscard]] size_t size() const {
        return m_size;
    }

    /// @brief Gets the maximum number of elements that can exist in the array.
    /// @return The maximum number of elements that can exist in the array.
    [[nodiscard]] size_t capacity() const {
        return m_capacity;
    }

    /// @brief Gets the first element in the array.
    /// @return A reference to the first element in the array.
    T &front() {
        ASSERT(m_size > 0);
        return *m_data;
    }

    /// @brief Gets the first element in the array.
    /// @return A const reference to the first element in the array.
    const T &front() const {
        ASSERT(m_size > 0);
        return *m_data;
    }

    /// @brief Gets the last existing element in the array.
    /// @return A reference to the last existing element in the array.
    T &back() {
        ASSERT(m_size > 0);
        return m_data[m_size - 1];
    }

    /// @brief Gets the last existing element in the array.
    /// @return A const reference to the last existing element in the array.
    const T &back() const {
        ASSERT(m_size > 0);
        return m_data[m_size - 1];
    }

    /// @brief Indexes the array. Validates that the object exists.
    /// @param idx The index to the array.
    /// @return A reference to the object at the corresponding index.
    T &operator[](size_t idx) {
        ASSERT(idx < m_size);
        return m_data[idx];
    }

    /// @brief Indexes the array. Validates that the object exists.
    /// @param idx The index to the array.
    /// @return A const reference to the object at the corresponding index.
    const T &operator[](size_t idx) const {
        ASSERT(idx < m_size);
        return m_data[idx];
    }

    /// @brief Iterator for the beginning of the existing array.
    /// @return Iterator.
    T *begin() noexcept {
        ASSERT(initialized());
        return m_data;
    }

    /// @brief Iterator for the beginning of the existing array.
    /// @return Const iterator.
    const T *begin() const noexcept {
        ASSERT(initialized());
        return m_data;
    }

    /// @brief Iterator for the end of the existing array.
    /// @return Iterator.
    T *end() noexcept {
        ASSERT(initialized());
        return m_data + m_size;
    }

    /// @brief Iterator for the end of the existing array.
    /// @return Const iterator.
    const T *end() const noexcept {
        ASSERT(initialized());
        return m_data + m_size;
    }

private:
    /// @brief Destroys existing elements and frees the buffer, resetting to an uninitialized state.
    void destroy() {
        while (m_size > 0) {
            pop_back();
        }

        EGG::egg_free(m_data);
        m_data = nullptr;
        m_capacity = 0;
    }

    /// @brief Allocates the array.
    /// @param capacity The number of elements to initialize the vector with.
    void allocate(size_t capacity) {
        ASSERT(!initialized());
        m_data = static_cast<T *>(
                EGG::egg_alloc(sizeof(T) * capacity, static_cast<s32>(alignof(T))));
        m_capacity = capacity;
    }

    T *m_data;         // The underlying array pointer.
    size_t m_size;     // The number of existing elements in the array.
    size_t m_capacity; // The maximum number of elements that can exist in the array.
};

} // namespace Kinoko
