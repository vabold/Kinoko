#pragma once

#include <Logger.hh>

#include <cstddef>
#include <limits>
#include <type_traits>

namespace EGG {

/// @brief Wrapper around an integral type with an enum corresponding to its bits.
/// @tparam T The underlying integral type.
/// @tparam E The enum to correspond to.
template <typename T, typename E>
    requires(std::is_enum_v<E> && std::is_integral_v<T>)
struct TBitFlag {
    /// @brief Default constructor, initializes all flags to off.
    /// @details Matches the expression `bits = 0`.
    constexpr TBitFlag() {
        makeAllZero();
    }

    /// @brief Constructor that initializes the bit flags with a given mask.
    /// @details Matches the expression `bits = mask`.
    /// @param mask The initial bit mask.
    constexpr TBitFlag(T mask) {
        setDirect(mask);
    }

    /// @brief Constructor that initializes the bit flags with a given enum.
    /// @details Matches the expression `bits = eEnum::val`.
    /// @param e The initial enum.
    constexpr TBitFlag(E e) : TBitFlag() {
        setBit(e);
    }

    /// @brief Conversion operator to the underlying integral type.
    [[nodiscard]] constexpr operator T() const {
        return getDirect();
    }

    /// @brief Assignment operator.
    /// @param rhs Source.
    /// @return Reference for chaining.
    constexpr TBitFlag<T, E> &operator=(const TBitFlag<T, E> &rhs) {
        bits = rhs.bits;
        return *this;
    }

    /// @brief Sets the corresponding bits for the provided enum values.
    /// @details Matches the expression `bits |= mask`.
    /// @tparam ...Es Variadic template for packing.
    /// @param ...es Enum values representing the bits to set.
    /// @return Reference for chaining.
    template <typename... Es>
        requires(std::is_same_v<Es, E> && ...)
    constexpr TBitFlag<T, E> &setBit(Es... es) {
        (setBit_(es), ...);
        return *this;
    }

    /// @brief Resets the corresponding bits for the provided enum values.
    /// @tparam ...Es Variadic template for packing.
    /// @param ...es Enum values representing the bits to reset.
    /// @return Reference for chaining.
    template <typename... Es>
        requires(std::is_same_v<Es, E> && ...)
    constexpr TBitFlag<T, E> &resetBit(Es... es) {
        (resetBit_(es), ...);
        return *this;
    }

    /// @brief Changes the state of the corresponding bits for the provided enum values.
    /// @tparam ...Es Variadic template for packing.
    /// @param on Determines whether to set or reset the bits.
    /// @param ...es Enum values representing the bits to change.
    /// @return Reference for chaining.
    template <typename... Es>
        requires(std::is_same_v<Es, E> && ...)
    constexpr TBitFlag<T, E> &changeBit(bool on, Es... es) {
        (changeBit_(on, es), ...);
        return *this;
    }

    /// @brief Toggles the corresponding bits for the provided enum values.
    /// @tparam ...Es Variadic template for packing.
    /// @param ...es Enum values representing the bits to toggle.
    /// @return Reference for chaining.
    template <typename... Es>
        requires(std::is_same_v<Es, E> && ...)
    constexpr TBitFlag<T, E> &toggleBit(Es... es) {
        (toggleBit_(es), ...);
        return *this;
    }

    /// @brief Checks if any of the corresponding bits for the provided enum values are on.
    /// @details Matches the expression `(bits & mask) != 0`.
    /// @tparam ...Es Variadic template for packing.
    /// @param ...es Enum values representing the bits to check.
    /// @return True if one of the specified bits are on, otherwise false.
    template <typename... Es>
        requires(std::is_same_v<Es, E> && ...)
    [[nodiscard]] constexpr bool onBit(Es... es) const {
        return (onBit_(es) || ...);
    }

    /// @brief Checks if all of the corresponding bits for the provided enum values are off.
    /// @details Matches the expression `(bits & mask) == 0`.
    /// @tparam ...Es Variadic template for packing.
    /// @param ...es Enum values representing the bits to check.
    /// @return True if all of the specified bits are off, otherwise false.
    template <typename... Es>
        requires(std::is_same_v<Es, E> && ...)
    [[nodiscard]] constexpr bool offBit(Es... es) const {
        return (offBit_(es) && ...);
    }

    /// @brief Creates an applied mask of the corresponding bits for the provided enum values.
    /// @details Matches the expression `(bits & mask)`.
    /// @tparam ...Es Variadic template for packing.
    /// @param ...es Enum values representing the bits to check.
    /// @return The applied mask for the specified bits.
    template <typename... Es>
        requires(std::is_same_v<Es, E> && ...)
    [[nodiscard]] constexpr T maskBit(Es... es) const {
        return bits & makeMask(es...);
    }

    /// @brief Creates a mask of the corresponding bits for the provided enum values.
    /// @tparam ...Es Variadic template for packing.
    /// @param ...es Enum values representing the bits to check.
    /// @return The mask for the specified bits.
    template <typename... Es>
        requires(std::is_same_v<Es, E> && ...)
    [[nodiscard]] constexpr T makeMask(Es... es) const {
        return (makeMask_(es) | ...);
    }

    /// @brief Sets the bits using a direct mask.
    /// @param mask The bit mask to set.
    /// @return Reference for chaining.
    constexpr TBitFlag<T, E> &set(T mask) {
        bits |= mask;
        return *this;
    }

    /// @brief Resets the bits using a direct mask.
    /// @param mask The bit mask to reset.
    /// @return Reference for chaining.
    constexpr TBitFlag<T, E> &reset(T mask) {
        bits &= ~mask;
        return *this;
    }

    /// @brief Changes the bits using a direct mask.
    /// @param on Determines whether to set or reset the bits.
    /// @param mask The bit mask to change.
    /// @return Reference for chaining.
    constexpr TBitFlag<T, E> &change(bool on, T mask) {
        return on ? set(mask) : reset(mask);
    }

    /// @brief Checks if any bits are on in the specified mask.
    /// @param mask The bit mask to check.
    /// @return True if any bits in the mask are on, otherwise false.
    [[nodiscard]] constexpr bool on(T mask) const {
        return (bits & mask) != 0;
    }

    /// @brief Checks if all bits are on in the specified mask.
    /// @param mask The bit mask to check.
    /// @return True if all bits in the mask are on, otherwise false.
    [[nodiscard]] constexpr bool onAll(T mask) const {
        return (bits | mask) == bits;
    }

    /// @brief Checks if all bits are off in the specified mask.
    /// @param mask The bit mask to check.
    /// @return True if all bits in the mask are off, otherwise false.
    [[nodiscard]] constexpr bool off(T mask) const {
        return (bits & mask) == 0;
    }

    /// @brief Resets all the bits to zero.
    constexpr void makeAllZero() {
        bits = 0;
    }

    /// @brief Gets the current bit mask.
    /// @return The current bit mask.
    [[nodiscard]] constexpr T getDirect() const {
        return bits;
    }

    /// @brief Sets the bits using a direct mask.
    /// @param mask The bit mask to set.
    constexpr void setDirect(T mask) {
        bits = mask;
    }

private:
    typedef std::underlying_type_t<E> EI;
    static constexpr size_t MAX_CAPACITY = std::numeric_limits<T>::digits;

    /// @brief Internal. Sets a specific bit.
    /// @details Validates that `e` is in the range of `T`.
    /// @param e Enum value representing the bit to set.
    constexpr void setBit_(E e) {
        ASSERT(static_cast<EI>(e) < MAX_CAPACITY);
        set(makeMask_(e));
    }

    /// @brief Internal. Resets a specific bit.
    /// @details Validates that `e` is in the range of `T`.
    /// @param e Enum value representing the bit to reset.
    constexpr void resetBit_(E e) {
        ASSERT(static_cast<EI>(e) < MAX_CAPACITY);
        reset(makeMask_(e));
    }

    /// @brief Internal. Changes a specific bit.
    /// @details Validates that `e` is in the range of `T`.
    /// @param on Determines whether to set or reset the bit.
    /// @param e Enum value respresenting the bit to change.
    constexpr void changeBit_(bool on, E e) {
        ASSERT(static_cast<EI>(e) < MAX_CAPACITY);
        change(on, makeMask_(e));
    }

    /// @brief Internal. Toggles a specific bit.
    /// @details Validates that `e` is in the range of `T`.
    /// @param e Enum value representing the bit to change.
    constexpr void toggleBit_(E e) {
        ASSERT(static_cast<EI>(e) < MAX_CAPACITY);
        changeBit_(offBit_(e), e);
    }

    /// @brief Checks if a specific bit is on.
    /// @details Validates that `e` is in the range of `T`.
    /// @param e Enum value representing the bit to change.
    /// @return True if the specified bit is on, otherwise false.
    [[nodiscard]] constexpr bool onBit_(E e) const {
        ASSERT(static_cast<EI>(e) < MAX_CAPACITY);
        return on(makeMask_(e));
    }

    /// @brief Checks if a specific bit is off.
    /// @details Validates that `e` is in the range of `T`.
    /// @param e Enum value representing the bit to change.
    /// @return True if the specified bit is off, otherwise false.
    [[nodiscard]] constexpr bool offBit_(E e) const {
        ASSERT(static_cast<EI>(e) < MAX_CAPACITY);
        return off(makeMask_(e));
    }

    /// @brief Creates a mask for a specific bit.
    /// @details Matches the expression `(1 << e)`. Validates that `e` is in the range of `T`.
    /// @param e
    /// @return The mask for the specified bit.
    [[nodiscard]] constexpr T makeMask_(E e) const {
        ASSERT(static_cast<EI>(e) < MAX_CAPACITY);
        return static_cast<T>(1) << static_cast<T>(e);
    }

    T bits; ///< The bit mask representing the flags.
};

} // namespace EGG
