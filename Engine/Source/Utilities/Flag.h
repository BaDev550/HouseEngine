#pragma once

#include <type_traits>

// Forked from https://github.com/Ynsefkksl/DnmGL

template <typename FlagBitsType>
struct FlagTraits {
    static constexpr bool is_bitmask = false;
};

template<typename T>
class Flags {
public:
    using Type = std::underlying_type_t<T>;

    constexpr Flags() noexcept : value(0) {}
    constexpr Flags(T bit) noexcept : value(static_cast<Type>(bit)) {}
    constexpr Flags(Type bits) noexcept : value(bits) {}

    constexpr Flags operator|(Flags o) const noexcept { return Flags(value | o.value); }
    constexpr Flags operator&(Flags o) const noexcept { return Flags(value & o.value); }
    constexpr Flags operator^(Flags o) const noexcept { return Flags(value ^ o.value); }
    constexpr bool operator<=>(const Flags& o) const noexcept = default;
    constexpr Flags operator~() const noexcept { return Flags(~value); }

    constexpr Flags& operator|=(Flags o) noexcept { value |= o.value; return *this; }
    constexpr Flags& operator&=(Flags o) noexcept { value &= o.value; return *this; }
    constexpr Flags& operator^=(Flags o) noexcept { value ^= o.value; return *this; }

    constexpr bool Any()  const noexcept { return value != 0; }
    constexpr bool None() const noexcept { return value == 0; }
    constexpr bool Has(T bit) const noexcept {
        return (value & static_cast<Type>(bit)) != 0;
    }

    constexpr explicit operator Type() const noexcept { return value; }
    constexpr explicit operator bool() const noexcept { return Any(); }
private:
    Type value;
};

template <typename BitType, typename std::enable_if<FlagTraits<BitType>::is_bitmask, bool>::type = true>
constexpr Flags<BitType> operator&( BitType lhs, BitType rhs ) noexcept {
    return Flags<BitType>( lhs ) & rhs;
}

template <typename BitType, typename std::enable_if<FlagTraits<BitType>::is_bitmask, bool>::type = true>
constexpr Flags<BitType> operator|( BitType lhs, BitType rhs ) noexcept {
    return Flags<BitType>( lhs ) | rhs;
}

template <typename BitType, typename std::enable_if<FlagTraits<BitType>::is_bitmask, bool>::type = true>
constexpr Flags<BitType> operator^( BitType lhs, BitType rhs ) noexcept {
    return Flags<BitType>( lhs ) ^ rhs;
}

template <typename BitType, typename std::enable_if<FlagTraits<BitType>::is_bitmask, bool>::type = true>
constexpr Flags<BitType> operator~( BitType bit ) noexcept {
    return ~( Flags<BitType>( bit ) );
}