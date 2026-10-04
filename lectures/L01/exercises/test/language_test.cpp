/**
 * @file The suite's always-on tests: claims Appendix B makes about C++, checked.
 *
 *       Every lecture's suite has one binary that is built whether or not you have written
 *       anything, because qacademy::test::runAllTests() reports failure when it has no tests to
 *       run, and a suite that is red on a fresh clone looks broken when it is merely empty. It
 *       may as well hold something worth knowing, so these tests are the appendix's statements
 *       about the language, each in a form the compiler can confirm.
 */
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <type_traits>

#include "qacademy/test/test.hpp"

namespace
{
/** A constexpr function, as in Appendix B.6. */
constexpr int add(const int x, const int y) { return x + y; }

/** One function that promises not to throw, and one that makes no promise (Appendix B.7). */
void quiet() noexcept {}
void loud() {}

/** A default argument, as in Appendix B.8. */
constexpr bool isHex(const std::uint32_t, const bool hex = false) noexcept { return hex; }

/** Modify the caller's variable through a reference (Appendix B.10). */
constexpr void toggle(bool& state) noexcept { state = !state; }

/** Set one bit in a register of any integral type (Appendix B.12). */
template<typename T>
constexpr void setBit(T& reg, const std::uint8_t bit) noexcept
{
    reg |= (static_cast<T>(1U) << bit);
}

/** Set any number of bits with a fold expression (Appendix B.12). */
template<typename T, typename... Bits>
constexpr void set(T& reg, const Bits... bits) noexcept
{
    static_assert(std::is_integral<T>::value, "T must be of integral type!");
    ((reg |= (static_cast<T>(1U) << bits)), ...);
}

/** A function that may have no value to return (Appendix B.14). */
constexpr std::optional<std::uint8_t> half(const std::uint8_t value) noexcept
{
    if (0U != (value % 2U)) { return std::nullopt; }
    return static_cast<std::uint8_t>(value / 2U);
}
} // namespace

/**
 * @brief A constexpr function can size an array: its result is known at compile time.
 */
TEST(Language, ConstexprValuesAreCompileTimeConstants)
{
    std::array<int, add(2, 3)> values{};
    EXPECT_EQ(values.size(), static_cast<std::size_t>(5U));
    static_assert(add(20, 22) == 42, "add() must be usable in a constant expression");
}

/**
 * @brief noexcept is part of what the compiler knows about a function, and can be queried.
 */
TEST(Language, NoexceptIsVisibleToTheCompiler)
{
    EXPECT_TRUE(noexcept(quiet()));
    EXPECT_FALSE(noexcept(loud()));
}

/**
 * @brief A parameter left out takes its default value.
 */
TEST(Language, DefaultArgumentsFillInFromTheRight)
{
    EXPECT_FALSE(isHex(42U));
    EXPECT_TRUE(isHex(42U, true));
}

/**
 * @brief A function taking a reference changes the variable it was given, not a copy.
 */
TEST(Language, ReferencesAliasTheCallersVariable)
{
    bool state{false};
    toggle(state);
    EXPECT_TRUE(state);
    toggle(state);
    EXPECT_FALSE(state);
}

/**
 * @brief auto deduces the type of its initializer, and in C++17 auto x{10} is an int.
 */
TEST(Language, AutoDeducesTheInitializersType)
{
    auto number  = 5;
    auto counter = 10U;
    auto voltage = 3.3;
    auto braced{10};
    EXPECT_TRUE((std::is_same<decltype(number), int>::value));
    EXPECT_TRUE((std::is_same<decltype(counter), unsigned int>::value));
    EXPECT_TRUE((std::is_same<decltype(voltage), double>::value));
    EXPECT_TRUE((std::is_same<decltype(braced), int>::value));
}

/**
 * @brief One call with a parameter pack sets all five bits.
 */
TEST(Language, FoldExpressionsSetEveryBitInThePack)
{
    std::uint8_t reg{};
    set(reg, 1U, 2U, 3U, 4U, 5U);
    EXPECT_EQ(static_cast<unsigned>(reg), 0x3EU);
}

/**
 * @brief A template is instantiated once per type: setBit<uint8_t> and setBit<uint32_t> are two
 *        different functions, at two different addresses, which is where a template's code size
 *        comes from.
 */
TEST(Language, EachTypeGetsItsOwnInstantiation)
{
    void (*const setByte)(std::uint8_t&, std::uint8_t) noexcept {&setBit<std::uint8_t>};
    void (*const setWord)(std::uint32_t&, std::uint8_t) noexcept {&setBit<std::uint32_t>};
    EXPECT_NE(reinterpret_cast<const void*>(setByte), reinterpret_cast<const void*>(setWord));
}

/**
 * @brief std::is_integral is what the bit utilities' static_assert relies on.
 */
TEST(Language, IsIntegralSeparatesIntegersFromFloatingPoint)
{
    EXPECT_TRUE(std::is_integral<std::uint8_t>::value);
    EXPECT_TRUE(std::is_integral<std::uint64_t>::value);
    EXPECT_FALSE(std::is_integral<float>::value);
    EXPECT_FALSE(std::is_integral<double>::value);
}

/**
 * @brief std::size_t is not a type of its own but another name for one of the fixed-width
 *        unsigned types, which is why Appendix B's isUnsigned trait must not specialize both.
 */
TEST(Language, SizeTIsAnAliasOfAFixedWidthType)
{
    constexpr bool isAlias{std::is_same<std::size_t, std::uint64_t>::value ||
                           std::is_same<std::size_t, std::uint32_t>::value};
    EXPECT_TRUE(isAlias);
}

/**
 * @brief A std::optional is empty until it is given a value, and std::nullopt is the empty state.
 */
TEST(Language, OptionalHoldsAValueOrNothing)
{
    const std::optional<std::uint8_t> empty{};
    EXPECT_FALSE(empty.has_value());
    EXPECT_FALSE(half(7U).has_value());

    const std::optional<std::uint8_t> four{half(8U)};
    EXPECT_TRUE(four.has_value());
    EXPECT_EQ(static_cast<unsigned>(*four), 4U);
}

/**
 * @brief In a condition, an optional says whether it holds a value, not what the value is: one
 *        holding 0 is true.
 */
TEST(Language, OptionalHoldingZeroIsNotEmpty)
{
    const std::optional<std::uint8_t> zero{half(0U)};
    EXPECT_TRUE(static_cast<bool>(zero));
    EXPECT_EQ(static_cast<unsigned>(*zero), 0U);
}

/**
 * @brief An optional stores its value inside itself: it is the size of the value plus a flag,
 *        rounded up to the value's alignment, and allocates nothing.
 */
TEST(Language, OptionalIsTheValuePlusAFlag)
{
    EXPECT_EQ(sizeof(std::optional<std::uint8_t>), static_cast<std::size_t>(2U));
    EXPECT_EQ(sizeof(std::optional<std::uint32_t>), static_cast<std::size_t>(8U));
}

/**
 * @brief Assigning std::nullopt empties an optional, and assigning a value fills it again.
 */
TEST(Language, OptionalCanBeEmptiedAndRefilled)
{
    std::optional<std::uint8_t> byte{0x41U};
    byte = std::nullopt;
    EXPECT_FALSE(byte.has_value());
    byte = 0x42U;
    EXPECT_TRUE(byte.has_value());
    EXPECT_EQ(static_cast<unsigned>(*byte), 0x42U);
}
