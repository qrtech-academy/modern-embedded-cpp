/**
 * @file The suite's always-on tests: claims Appendices A and B make about classes, checked.
 *
 *       Every lecture's suite has one binary that is built whether or not you have written
 *       anything, because qacademy::test::runAllTests() reports failure when it has no tests to
 *       run, and a suite that is red on a fresh clone looks broken when it is merely empty. It
 *       may as well hold something worth knowing, so these tests are the appendices' statements
 *       about classes, each in a form the compiler can confirm or the running program can show.
 */
#include <array>
#include <cstdint>
#include <cstdio>
#include <type_traits>
#include <utility>

#include "qacademy/test/test.hpp"
#include "support/output.hpp"

using support::captureOutput;

namespace
{
/** A struct and a class that differ only in the keyword (Appendix A, Terminology). */
struct PlainStruct
{
    int value;
};

class PlainClass
{
    int value;

public:
    /** A way to use the member, so the compiler does not warn that it is never used. */
    int get() const noexcept { return value; }
};

/**
 * @brief True if T has a member named value that code outside the class may use. Access checking
 *        is part of template argument substitution, so a private member makes this false.
 */
template<typename T, typename = void>
struct HasAccessibleValue : std::false_type
{
};

template<typename T>
struct HasAccessibleValue<T, std::void_t<decltype(std::declval<T&>().value)>> : std::true_type
{
};

/** The Led of Appendix B, first with an implicit constructor and then with an explicit one. */
class ImplicitLed final
{
public:
    ImplicitLed(const std::uint8_t pin) noexcept
        : myPin{pin}
        , myState{false}
    {}

    [[nodiscard]] std::uint8_t pin() const noexcept { return myPin; }
    [[nodiscard]] bool isEnabled() const noexcept { return myState; }

private:
    const std::uint8_t myPin;
    bool myState;
};

class ExplicitLed final
{
public:
    explicit ExplicitLed(const std::uint8_t pin) noexcept
        : myPin{pin}
    {}

    [[nodiscard]] std::uint8_t pin() const noexcept { return myPin; }

private:
    const std::uint8_t myPin;
};

/** The function Appendix B passes an LED to by value. */
void ledPrint(const ImplicitLed led) noexcept
{
    const char* state{led.isEnabled() ? "on" : "off"};
    std::printf("The LED is %s!\n", state);
}

/** A class that counts its instances with static members (Appendix B, static members). */
class Gpio final
{
public:
    explicit Gpio(const std::uint8_t pin) noexcept
        : myPin{pin}
    {
        ++ourInstanceCount;
    }

    ~Gpio() noexcept { --ourInstanceCount; }

    [[nodiscard]] static std::uint8_t instanceCount() noexcept { return ourInstanceCount; }
    [[nodiscard]] std::uint8_t pin() const noexcept { return myPin; }

    static constexpr std::uint8_t MaxInstances{40U};

    Gpio(const Gpio&)            = delete; // No copy constructor.
    Gpio(Gpio&&)                 = delete; // No move constructor.
    Gpio& operator=(const Gpio&) = delete; // No copy assignment.
    Gpio& operator=(Gpio&&)      = delete; // No move assignment.

private:
    const std::uint8_t myPin;
    static std::uint8_t ourInstanceCount;
};

std::uint8_t Gpio::ourInstanceCount{0U};

/** A class holding a const member, as Appendix B's Gpio holds myPin and myDirection. */
struct ConstMember
{
    const std::uint8_t pin;
};

/** A member that records how it was last created: copied, or moved. */
struct Probe
{
    Probe() = default;
    Probe(const Probe&) noexcept
        : moved{false}
    {}
    Probe(Probe&&) noexcept
        : moved{true}
    {}
    bool moved{false};
};

/** A class that gets its move constructor from the compiler... */
struct WithoutDestructor
{
    Probe probe{};
};

/** ...and one that does not, because it declares a destructor. */
struct WithDestructor
{
    Probe probe{};
    ~WithDestructor() {}
};

/** The enumeration class of Appendix A, step 2. */
enum class Direction : std::uint8_t
{
    Input,
    InputPullup,
    Output,
    Count,
};

[[nodiscard]] constexpr bool isDirectionValid(const Direction direction) noexcept
{
    return static_cast<std::uint8_t>(Direction::Count) > static_cast<std::uint8_t>(direction);
}
} // namespace

/**
 * @brief Appendix A: the only formal difference between a struct and a class is the default
 *        access. The struct's member is public, the class's private.
 */
TEST(Language, StructMembersArePublicAndClassMembersPrivateByDefault)
{
    EXPECT_TRUE(HasAccessibleValue<PlainStruct>::value);
    EXPECT_FALSE(HasAccessibleValue<PlainClass>::value);
}

/**
 * @brief Appendix B: a constructor that is not explicit lets a pin number turn into an LED on its
 *        own, which is how ledPrint(8U) compiles and prints an LED that was never created.
 */
TEST(Language, AnImplicitConstructorConvertsQuietly)
{
    EXPECT_TRUE((std::is_convertible<std::uint8_t, ImplicitLed>::value));
    EXPECT_OUTPUT(captureOutput([] { ledPrint(8U); }), "The LED is off!\n");
}

/**
 * @brief Appendix B: marking the constructor explicit stops the conversion, and nothing else;
 *        the LED can still be constructed on purpose.
 */
TEST(Language, AnExplicitConstructorDoesNotConvert)
{
    EXPECT_FALSE((std::is_convertible<std::uint8_t, ExplicitLed>::value));
    EXPECT_TRUE((std::is_constructible<ExplicitLed, std::uint8_t>::value));
    const ExplicitLed led{8U};
    EXPECT_EQ(static_cast<unsigned>(led.pin()), 8U);
}

/**
 * @brief Appendix B: a static method is called through the class, and a static member counts
 *        the instances every object shares.
 */
TEST(Language, StaticMembersBelongToTheClass)
{
    EXPECT_EQ(static_cast<unsigned>(Gpio::instanceCount()), 0U);
    {
        const Gpio led{13U};
        const Gpio button{2U};
        EXPECT_EQ(static_cast<unsigned>(Gpio::instanceCount()), 2U);
    }
    EXPECT_EQ(static_cast<unsigned>(Gpio::instanceCount()), 0U);
}

/**
 * @brief Appendix B: a static constexpr member is a compile-time constant that needs no
 *        definition in a source file; it can even size an array.
 */
TEST(Language, StaticConstexprMembersAreCompileTimeConstants)
{
    std::array<bool, Gpio::MaxInstances> used{};
    EXPECT_EQ(used.size(), static_cast<std::size_t>(40U));
}

/**
 * @brief Appendices A and B: deleting the copy and move operations makes a class impossible to
 *        duplicate, which is what a class standing for one physical pin needs.
 */
TEST(Language, DeletedOperationsMakeAClassUnique)
{
    EXPECT_FALSE(std::is_copy_constructible<Gpio>::value);
    EXPECT_FALSE(std::is_move_constructible<Gpio>::value);
    EXPECT_FALSE(std::is_copy_assignable<Gpio>::value);
    EXPECT_FALSE(std::is_move_assignable<Gpio>::value);
}

/**
 * @brief Appendix B: a const member can be copied into a new object, but an existing object
 *        cannot be overwritten, so the compiler deletes the assignment operators.
 */
TEST(Language, AConstMemberRulesOutAssignment)
{
    EXPECT_TRUE(std::is_copy_constructible<ConstMember>::value);
    EXPECT_FALSE(std::is_copy_assignable<ConstMember>::value);
    EXPECT_FALSE(std::is_move_assignable<ConstMember>::value);
}

/**
 * @brief Appendix B: std::move() is what selects the move constructor. A class that declares a
 *        destructor gets no move constructor from the compiler, so the same std::move() falls
 *        back to copying.
 */
TEST(Language, StdMoveSelectsTheMoveConstructorIfThereIsOne)
{
    WithoutDestructor first{};
    const WithoutDestructor moved{std::move(first)};
    EXPECT_TRUE(moved.probe.moved);

    WithDestructor second{};
    const WithDestructor copied{std::move(second)};
    EXPECT_FALSE(copied.probe.moved);
}

/**
 * @brief Appendix A, step 2: an enumeration class has the underlying type it is given, does not
 *        convert to an integer on its own, and Count makes validation one comparison.
 */
TEST(Language, EnumerationClassesAreScopedAndCompact)
{
    EXPECT_TRUE((std::is_same<std::underlying_type<Direction>::type, std::uint8_t>::value));
    EXPECT_EQ(sizeof(Direction), static_cast<std::size_t>(1U));
    EXPECT_FALSE((std::is_convertible<Direction, int>::value));
    EXPECT_TRUE(isDirectionValid(Direction::Output));
    EXPECT_FALSE(isDirectionValid(Direction::Count));
}
