#pragma once

#include <Logger.hh>

#include <cstdint>
#include <utility>

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

/// @brief Dynamically sized array that only allocates once.
/// @details It's possible that only an upper bound is known for a given vector.
/// However, in the event that there are more objects than expected, we want to error.
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

    /// @brief Destructor.
    /// @details Destroys all existing elements in the array in-place from the end to the start.
    ~fixed_vector() {
        while (m_size > 0) {
            pop_back();
        }

        operator delete(m_data);
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
    bool empty() const {
        return m_size == 0;
    }

    /// @brief Checks if all elements exist in the array.
    /// @return True if the array is full, otherwise false.
    bool full() const {
        return m_size == m_capacity;
    }

    /// @brief Checks if the array exists and if the capacity is non-zero.
    /// @return True if the array is initialized, otherwise false.
    bool initialized() const {
        return m_data && m_capacity != 0;
    }

    /// @brief Gets the first element in the array, whether it exists or not.
    /// @return The first element in the array.
    T &front() {
        return *m_data;
    }

    /// @brief Gets the first element in the array, whether it exists or not.
    /// @return The first element in the array.
    const T &front() const {
        return *m_data;
    }

    /// @brief Gets the last existing element in the array, or the first element if none exist.
    /// @return The last existing element in the array.
    T &back() {
        return *(m_data + m_size);
    }

    /// @brief Gets the last existing element in the array, or the first element if none exist.
    /// @return The last existing element in the array.
    const T &back() const {
        return *(m_data + m_size);
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
    /// @return A reference to the object at the corresponding index.
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
    /// @brief Allocates the array.
    /// @param capacity The number of elements to initialize the vector with.
    void allocate(size_t capacity) {
        ASSERT(!initialized());
        m_data = reinterpret_cast<T *>(operator new(sizeof(T) * capacity));
        m_capacity = capacity;
    }

    T *m_data;         // The underlying array pointer.
    size_t m_size;     // The number of existing elements in the array.
    size_t m_capacity; // The maximum number of elements that can exist in the array.
};
