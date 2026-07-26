#pragma once

#include "egg/core/Heap.hh"

namespace Kinoko::EGG {

template <typename T>
struct Allocator {
    using value_type = T;

    Allocator() = default;
    template <typename U>
    constexpr Allocator(const Allocator<U> &) noexcept {}

    [[nodiscard]] T *allocate(size_t n) {
        return static_cast<T *>(egg_alloc(n * sizeof(T), static_cast<s32>(alignof(T))));
    }

    void deallocate(T *ptr, size_t) noexcept {
        egg_free(ptr);
    }
};

template <typename T, typename U>
constexpr bool operator==(const Allocator<T> &, const Allocator<U> &) noexcept {
    return true;
}

} // namespace Kinoko::EGG
