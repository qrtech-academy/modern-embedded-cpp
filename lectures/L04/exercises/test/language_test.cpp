/**
 * @file The suite's always-on tests: what Appendices A and B say about owning an object, checked.
 *
 *       Every lecture's suite has one binary that is built whether or not you have written
 *       anything, because qacademy::test::runAllTests() reports failure when it has no tests to
 *       run. This one holds the appendices' claims about new, delete and std::unique_ptr, each in
 *       a form the compiler or a running program can confirm.
 */
#include <memory>
#include <type_traits>
#include <utility>

#include "qacademy/test/test.hpp"

namespace
{
/** How many Derived objects have been destroyed. */
int destroyed{};

/** A base class with a virtual destructor, like every interface in the course. */
struct Base
{
    virtual ~Base() noexcept = default;
};

/** A derived class that counts its own destruction. */
struct Derived final : Base
{
    ~Derived() noexcept override { ++destroyed; }
};
} // namespace

/**
 * @brief Appendix A: new calls the constructor, and delete through a base-class pointer calls the
 *        derived class's destructor, because the base's destructor is virtual.
 */
TEST(Ownership, DeleteThroughTheInterfaceDestroysTheDriver)
{
    destroyed = 0;
    Base* object{new Derived{}};
    delete object;
    EXPECT_EQ(destroyed, 1);
    EXPECT_TRUE(std::has_virtual_destructor<Base>::value);
}

/**
 * @brief Appendix B: a std::unique_ptr cannot be copied, only moved.
 */
TEST(Ownership, UniquePtrIsMoveOnly)
{
    using Pointer = std::unique_ptr<Base>;
    EXPECT_FALSE(std::is_copy_constructible<Pointer>::value);
    EXPECT_FALSE(std::is_copy_assignable<Pointer>::value);
    EXPECT_TRUE(std::is_move_constructible<Pointer>::value);
    EXPECT_TRUE(std::is_move_assignable<Pointer>::value);
}

/**
 * @brief Appendix B: moving a std::unique_ptr moves the ownership, and leaves the source empty.
 */
TEST(Ownership, MovingLeavesTheSourceEmpty)
{
    std::unique_ptr<Base> first{std::make_unique<Derived>()};
    Base* const object{first.get()};
    std::unique_ptr<Base> second{std::move(first)};
    EXPECT_TRUE(nullptr == first);
    EXPECT_TRUE(object == second.get());
}

/**
 * @brief Appendix B: when a std::unique_ptr goes out of scope, the object is destroyed, through
 *        the interface's virtual destructor and exactly once.
 */
TEST(Ownership, UniquePtrDeletesTheObjectAutomatically)
{
    destroyed = 0;
    {
        std::unique_ptr<Base> object{std::make_unique<Derived>()};
        EXPECT_EQ(destroyed, 0);
    }
    EXPECT_EQ(destroyed, 1);
}

/**
 * @brief Appendix B: std::make_unique<T>() returns a std::unique_ptr<T>, which converts to a
 *        std::unique_ptr to any base class: how a factory returns a driver as its interface.
 */
TEST(Ownership, MakeUniqueReturnsAConvertibleUniquePtr)
{
    EXPECT_TRUE(
        (std::is_same<decltype(std::make_unique<Derived>()), std::unique_ptr<Derived>>::value));
    EXPECT_TRUE((std::is_convertible<std::unique_ptr<Derived>, std::unique_ptr<Base>>::value));
    EXPECT_FALSE((std::is_convertible<std::unique_ptr<Base>, std::unique_ptr<Derived>>::value));
}
