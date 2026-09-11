/**
 * @file The suite's always-on tests: claims Appendices A and B make about inheritance and
 *       interfaces, checked.
 *
 *       Every lecture's suite has one binary that is built whether or not you have written
 *       anything, because qacademy::test::runAllTests() reports failure when it has no tests to
 *       run, and a suite that is red on a fresh clone looks broken when it is merely empty. It
 *       may as well hold something worth knowing, so these tests are the appendices' statements
 *       about the language, each in a form the compiler can confirm.
 */
#include <memory>
#include <type_traits>

#include "qacademy/test/test.hpp"

namespace
{
/** How many Derived objects have been destroyed, whichever way they were deleted. */
int destroyedDerived{};

/** A base class with protected members, as driver::Gpio has in Appendix A. */
class Base
{
public:
    virtual ~Base() noexcept = default;
    [[nodiscard]] int value() const noexcept { return myValue; }

protected:
    int myValue{1};
};

/** Public inheritance: Derived reaches Base's protected member and keeps its public ones. */
class Derived final : public Base
{
public:
    ~Derived() noexcept override { ++destroyedDerived; }
    void set(const int value) noexcept { myValue = value; }
};

/** Private inheritance: Base's public members become private in Hidden. */
class Hidden final : private Base
{
};

/** An interface: pure virtual methods and a virtual destructor, as in Appendix B. */
class Interface
{
public:
    virtual ~Interface() noexcept                 = default;
    [[nodiscard]] virtual int id() const noexcept = 0;
};

/** One implementation of the interface. */
class First final : public Interface
{
public:
    [[nodiscard]] int id() const noexcept override { return 1; }
};

/** Another implementation of the interface. */
class Second final : public Interface
{
public:
    [[nodiscard]] int id() const noexcept override { return 2; }
};

/** Code written against the interface, which works with any implementation of it. */
int idOf(const Interface& object) noexcept { return object.id(); }
} // namespace

/**
 * @brief A class with a pure virtual method is abstract, and cannot be instantiated; its
 *        implementations can.
 */
TEST(Language, PureVirtualMakesAClassAbstract)
{
    EXPECT_TRUE(std::is_abstract<Interface>::value);
    EXPECT_FALSE(std::is_constructible<Interface>::value);
    EXPECT_FALSE(std::is_abstract<First>::value);
    EXPECT_TRUE(std::is_default_constructible<First>::value);
}

/**
 * @brief Code written against an interface calls whichever implementation it is given.
 */
TEST(Language, CallsThroughAnInterfaceReachTheImplementation)
{
    const First first{};
    const Second second{};
    EXPECT_EQ(idOf(first), 1);
    EXPECT_EQ(idOf(second), 2);
}

/**
 * @brief With public inheritance a derived object is a base object: a Derived* converts to a
 *        Base*. With private inheritance it does not.
 */
TEST(Language, PublicInheritanceIsARelationship)
{
    EXPECT_TRUE((std::is_base_of<Base, Derived>::value));
    EXPECT_TRUE((std::is_convertible<Derived*, Base*>::value));
    EXPECT_TRUE((std::is_base_of<Base, Hidden>::value));
    EXPECT_FALSE((std::is_convertible<Hidden*, Base*>::value));
}

/**
 * @brief A derived class reaches its base's protected members; the rest of the program sees only
 *        the public interface.
 */
TEST(Language, ProtectedMembersReachDerivedClasses)
{
    Derived derived{};
    EXPECT_EQ(derived.value(), 1);
    derived.set(42);
    EXPECT_EQ(derived.value(), 42);
}

/**
 * @brief A virtual destructor is what makes deleting through a base pointer destroy the derived
 *        object too.
 */
TEST(Language, VirtualDestructorDestroysTheDerivedObject)
{
    EXPECT_TRUE(std::has_virtual_destructor<Base>::value);
    EXPECT_TRUE(std::has_virtual_destructor<Interface>::value);
    destroyedDerived = 0;
    std::unique_ptr<Base> object{new Derived{}};
    object.reset();
    EXPECT_EQ(destroyedDerived, 1);
}

/**
 * @brief final marks a class that cannot be inherited.
 */
TEST(Language, FinalClassesCannotBeInherited)
{
    EXPECT_TRUE(std::is_final<Derived>::value);
    EXPECT_TRUE(std::is_final<First>::value);
    EXPECT_FALSE(std::is_final<Base>::value);
    EXPECT_FALSE(std::is_final<Interface>::value);
}

/**
 * @brief A class with a virtual method is polymorphic, which is what makes its calls dispatch at
 *        run time.
 */
TEST(Language, VirtualMethodsMakeAClassPolymorphic)
{
    EXPECT_TRUE(std::is_polymorphic<Interface>::value);
    EXPECT_TRUE(std::is_polymorphic<First>::value);
}
