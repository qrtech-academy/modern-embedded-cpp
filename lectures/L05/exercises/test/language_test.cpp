/**
 * @file The suite's always-on tests: claims Appendices A to C make about templates, checked.
 *
 *       Every lecture's suite has one binary that is built whether or not you have written
 *       anything, because qacademy::test::runAllTests() reports failure when it has no tests to
 *       run, and a suite that is red on a fresh clone looks broken when it is merely empty. It
 *       may as well hold something worth knowing, so these tests are the appendices' statements
 *       about templates, each in a form the compiler can confirm.
 */
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "qacademy/test/test.hpp"

namespace
{
/** The function template from Appendix A. */
template<typename T>
constexpr T add(const T x, const T y) noexcept
{
    return x + y;
}

/** Set one bit in a register of any integral type (Appendix A). */
template<typename T>
constexpr void set(T& reg, const std::uint8_t bit) noexcept
{
    reg |= (static_cast<T>(1U) << bit);
}

/** The custom trait from Appendix B: false for every type... */
template<typename T>
struct isUnsigned
{
    static constexpr bool value{false};
};

/** ...except those it is specialized for. */
template<>
struct isUnsigned<std::uint8_t>
{
    static constexpr bool value{true};
};

template<>
struct isUnsigned<std::uint16_t>
{
    static constexpr bool value{true};
};

template<>
struct isUnsigned<std::uint32_t>
{
    static constexpr bool value{true};
};

/** The variable template from Appendix B. */
template<typename T>
inline constexpr bool isUnsigned_v{isUnsigned<T>::value};

/** A class template, as in Appendix C: one blueprint, a separate type per argument. */
template<typename T>
class Vector
{
public:
    Vector() noexcept
        : myData{nullptr}
        , mySize{}
    {}

    [[nodiscard]] std::size_t size() const noexcept { return mySize; }

private:
    T* myData;
    std::size_t mySize;
};

/** A primary template and a full specialization, as the timer driver in Exercise 3.1 has. */
template<typename T>
struct Implementation
{
    static constexpr const char* name{"primary"};
};

template<>
struct Implementation<float>
{
    static constexpr const char* name{"specialization"};
};

/** A default template argument, as Timer<Type T = Type::Stub> has. */
template<std::size_t Capacity = 8U>
struct Buffer
{
    std::uint8_t data[Capacity];
};

/**
 * @brief Count the arguments in a parameter pack.
 *
 * @return How many arguments were passed.
 */
template<typename... Args>
constexpr std::size_t countArgs(const Args&...) noexcept
{
    return sizeof...(Args);
}
} // namespace

/**
 * @brief Appendix A: add(1, 2) and add(1.0, 2.0) instantiate add<int> and add<double>, and each
 *        returns its own type.
 */
TEST(Language, EachCallInstantiatesForItsType)
{
    const auto sum1 = add(1, 2);
    const auto sum2 = add(1.0, 2.0);
    EXPECT_TRUE((std::is_same<decltype(sum1), const int>::value));
    EXPECT_TRUE((std::is_same<decltype(sum2), const double>::value));
    EXPECT_EQ(sum1, 3);
    EXPECT_NEAR(sum2, 3.0, 1e-12);
    static_assert(add(20, 22) == 42, "a constexpr template is usable at compile time");
}

/**
 * @brief Appendix A: set(std::uint8_t&, ...) and set(std::uint32_t&, ...) are two separate
 *        functions, at two separate addresses, which is where a template's code size comes from.
 */
TEST(Language, EachTypeIsASeparateFunction)
{
    void (*const setByte)(std::uint8_t&, std::uint8_t) noexcept {&set<std::uint8_t>};
    void (*const setWord)(std::uint32_t&, std::uint8_t) noexcept {&set<std::uint32_t>};
    EXPECT_NE(reinterpret_cast<const void*>(setByte), reinterpret_cast<const void*>(setWord));

    std::uint8_t reg1{};
    std::uint32_t reg2{};
    set(reg1, 1U);
    set(reg2, 12U);
    EXPECT_EQ(static_cast<unsigned>(reg1), 0x02U);
    EXPECT_EQ(reg2, 0x1000U);
}

/**
 * @brief Appendix B: is_integral accepts only integers, while is_arithmetic lets floating point
 *        through, which is why a bit operation must check the first.
 */
TEST(Language, IntegralIsNarrowerThanArithmetic)
{
    EXPECT_TRUE(std::is_integral<int>::value);
    EXPECT_FALSE(std::is_integral<double>::value);
    EXPECT_TRUE(std::is_arithmetic<double>::value);
    EXPECT_TRUE(std::is_floating_point<float>::value);
    EXPECT_TRUE(std::is_integral_v<std::uint64_t>);
}

/**
 * @brief Appendix B: is_unsigned, the standard trait the custom isUnsigned imitates.
 */
TEST(Language, IsUnsignedSeparatesTheSignedTypes)
{
    EXPECT_TRUE(std::is_unsigned<std::uint8_t>::value);
    EXPECT_TRUE(std::is_unsigned<std::uint32_t>::value);
    EXPECT_FALSE(std::is_unsigned<int>::value);
    EXPECT_FALSE(std::is_unsigned<float>::value);
}

/**
 * @brief Appendix B: the custom trait is false for every type but those it is specialized for,
 *        and the variable template reads the same value.
 */
TEST(Language, CustomTraitsAreSpecializations)
{
    EXPECT_TRUE(isUnsigned<std::uint16_t>::value);
    EXPECT_FALSE(isUnsigned<int>::value);
    EXPECT_FALSE(isUnsigned<double>::value);
    EXPECT_TRUE(isUnsigned_v<std::uint32_t>);
    EXPECT_FALSE(isUnsigned_v<char>);
}

/**
 * @brief Appendix C: Vector<int> and Vector<double> are two different types, generated from one
 *        blueprint at compile time.
 */
TEST(Language, EachArgumentIsASeparateType)
{
    EXPECT_FALSE((std::is_same<Vector<int>, Vector<double>>::value));
    const Vector<int> a{};
    const Vector<double> b{};
    EXPECT_EQ(a.size(), b.size());
}

/**
 * @brief Exercise 3.1: a full specialization is chosen over the primary template for its own
 *        argument, and the primary template is used for every other.
 */
TEST(Language, FullSpecializationWinsForItsArgument)
{
    EXPECT_EQ(std::strcmp(Implementation<float>::name, "specialization"), 0);
    EXPECT_EQ(std::strcmp(Implementation<int>::name, "primary"), 0);
    EXPECT_EQ(std::strcmp(Implementation<double>::name, "primary"), 0);
}

/**
 * @brief A default template argument is used when none is given, so Buffer<> is Buffer<8>; and a
 *        value in the type is part of the type, so different capacities are different sizes.
 */
TEST(Language, TemplateArgumentsArePartOfTheType)
{
    EXPECT_TRUE((std::is_same<Buffer<>, Buffer<8U>>::value));
    EXPECT_EQ(sizeof(Buffer<4U>), static_cast<std::size_t>(4U));
    EXPECT_EQ(sizeof(Buffer<16U>), static_cast<std::size_t>(16U));
    EXPECT_EQ((std::array<std::uint8_t, 12U>{}.size()), static_cast<std::size_t>(12U));
}

/**
 * @brief Appendix C: sizeof... counts the arguments in a parameter pack, at compile time.
 */
TEST(Language, SizeofCountsAParameterPack)
{
    static_assert(countArgs() == 0U, "an empty pack has no arguments");
    EXPECT_EQ(countArgs(1, 2.0, 'c'), static_cast<std::size_t>(3U));
}
