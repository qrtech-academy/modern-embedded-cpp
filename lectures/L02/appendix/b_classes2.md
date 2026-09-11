# Appendix B

## Advanced discussion on classes

### The keyword `explicit`
Consider the simple class `Led` below:

```cpp
#include <cstdint>

class Led final
{
public:
    Led(const std::uint8_t pin) noexcept
        : myPin{pin}
        , myState{false}
    {}

    void setEnabled(const bool state) noexcept { myState = state; }
    [[nodiscard]] bool isEnabled() const noexcept { return myState; }

private:
    const std::uint8_t myPin;
    bool myState;
};
```

Using the constructor above, we can create a `Led` instance and pass it as a copy (*pass-by-value*) to a function `ledPrint`, as shown below. The output is printed using `std::printf()` from `<cstdio>`:

```cpp
void ledPrint(const Led led) noexcept
{
    const char* state{led.isEnabled() ? "on" : "off"};
    std::printf("The LED is %s!\n", state);
}

int main()
{
    Led led1{8U};
    led1.setEnabled(true);
    ledPrint(led1);
    return 0;
}
```

The output appears as shown below:

```
The LED is on!
```

We can also create a `Led` instance through copy initialization, as shown below. Note that in this case we have not created a `Led` instance — instead, a pin number is passed (by mistake). This argument is implicitly converted into a `Led` instance:

```cpp
void ledPrint(const Led led) noexcept
{
    const char* state{led.isEnabled() ? "on" : "off"};
    std::printf("The LED is %s!\n", state);
}

int main()
{
    ledPrint(8U);
    return 0;
}
```

The output appears as shown below:

```
The LED is off!
```

**Note**: In this example, `led` is passed as a copy (pass-by-value) to clearly demonstrate how an implicit conversion can occur through the constructor. For larger classes, a const reference is often used instead to avoid unnecessary copies.

What happens in this case is that:
* The compiler implicitly constructs a `Led` instance given from an unsigned integer used as the pin number.
* The compiler expected a `Led` instance, but since it received an unsigned integer instead, which matches the class constructor, it implicitly invoked this constructor and thereby created a `Led` instance.
* This might be seen as efficient or elegant, but it can also lead to hard-to-detect bugs and is therefore not recommended.
* It is therefore good practice to ensure that implicit conversion of `Led` instances is not allowed. This can be achieved by adding the keyword `explicit` before the constructor, as shown below:

```cpp
#include <cstdint>

class Led final
{
public:
    explicit Led(const std::uint8_t pin) noexcept
        : myPin{pin}
        , myState{false}
    {}

    void setEnabled(const bool state) noexcept { myState = state; }
    [[nodiscard]] bool isEnabled() const noexcept { return myState; }

private:
    const std::uint8_t myPin;
    bool myState;
};
```

After marking the constructor `explicit`, the previous code example would have produced a compilation error.

---

### `static` Members
So far, every method and member variable we have seen belongs to a specific object: `myPin` and
`myState` each have a different value for every `Gpio` instance, and `read()` operates on one
particular instance's state. The keyword `static` lets a member instead belong to the *class
itself*, shared by every instance rather than duplicated per object.

In this section we continue using the class `Gpio`.

---

#### Static methods
A static method is called through the class name, using the scope resolution operator `::`,
rather than through an object:

```cpp
Gpio::instanceCount();
```

Because a static method has no associated object, it has no `this` pointer and therefore cannot
access non-static (instance) members such as `myState`. It can only access other `static` members.

We add a static method `instanceCount()` that reports how many `Gpio` instances currently exist:

```cpp
/**
 * @brief Get the number of currently active GPIO instances.
 *
 * @return Number of active GPIO instances.
 */
[[nodiscard]] static std::uint8_t instanceCount() noexcept { return ourInstanceCount; }
```

**Note**: A static method can technically also be called through an object, for example
`led1.instanceCount()`, but this is discouraged; it misleadingly suggests that the result depends
on `led1`, when it does not.

---

#### Static data members
A static data member is shared by all instances of the class, instead of each object storing its
own copy. We use the prefix `our` for private static members, the same way `my` is used for
private instance members; both exist to avoid name collisions with methods.

**`static constexpr`** is used for a compile-time constant shared by the class, for example the
maximum number of `Gpio` instances supported by the target MCU:

```cpp
/** Maximum number of GPIO instances supported by this MCU. */
static constexpr std::uint8_t MaxInstances{40U};
```

**Note**: Unlike regular `camelCase` members and methods, a `static constexpr` member is named
starting with a capital letter, for example `MaxInstances` rather than `maxInstances`. Some C++
style guides instead use a `k` prefix for this (`kMaxInstances`); this course uses a leading
capital letter.

Since C++17, a `static constexpr` data member is implicitly `inline`. This means `MaxInstances`
needs no separate definition in a source file. Unlike the plain `static` member shown next, it can
be fully declared and initialized directly inside the class.

A **plain `static`** member is used instead when the shared value must change at runtime, for
example a counter tracking how many `Gpio` instances currently exist:

```cpp
private:
    static std::uint8_t ourInstanceCount;
```

Unlike `static constexpr`, a plain `static` member must be defined exactly once in a source
file, or the linker fails:

```cpp
std::uint8_t Gpio::ourInstanceCount{0U};
```

We update the constructor and destructor to keep `ourInstanceCount` accurate:

```cpp
Gpio::Gpio(const std::uint8_t pin, const Direction direction, const bool initialState) noexcept
    : myPin{pin}
    , myDirection{direction}
    , myState{initialState}
{
    ++ourInstanceCount;
}

Gpio::~Gpio() noexcept
{
    --ourInstanceCount;
}
```

**Note**: This is more than a bookkeeping exercise. Recall from
[Appendix A](./a_classes1.md#6-copy-and-move-operations) that a `Gpio` object represents a unique
physical pin — `ourInstanceCount` combined with `MaxInstances`
gives the constructor a way to actually enforce that limit, refusing to create more `Gpio`
instances than the MCU has physical pins for.

We can now query the shared instance count without needing any particular `Gpio` object:

```cpp
int main()
{
    Gpio led{13U, Direction::Output};
    std::printf("Active GPIO instances: %u\n", Gpio::instanceCount());
    return 0;
}
```

```
Active GPIO instances: 1
```

---

### Copy and move operations
In addition to regular constructors, C++ also provides special member functions for copying and moving. These are very important in modern C++, and it is useful to become familiar with them when learning about classes.

The most common ones are:
* **Copy constructor** – Creates a new object as a copy of another object.
* **Copy assignment operator** – Assigns an already existing object from another object.
* **Move constructor** – Creates a new object by moving resources from another object.
* **Move assignment operator** – Assigns an already existing object by moving resources from another object.

In this section we continue using the class `Gpio`. This allows the same class to be used throughout the chapter while demonstrating the syntax for copy and move operations.

The following implementations are mainly shown to illustrate the syntax. For a class such as `Gpio`, it is usually better to delete these operations entirely.

We must keep in mind that the member variables `myPin` and `myDirection` are declared as `const`. This means that these values can only be set when the object is created and cannot be changed later. This affects how copy and move operations can be implemented.

---

#### Copy constructor
The copy constructor is used to create a new object as a copy of another object.

```cpp
// Create led1.
Gpio led1{13U, Direction::Output, false};

// Create copy of led1.
Gpio led2{led1};
```

An explicit copy constructor can be implemented as follows:

```cpp
/**
 * @brief Copy constructor.
 *
 * @param[in] other GPIO instance to copy.
 */
Gpio(const Gpio& other) noexcept
    : myPin{other.myPin}
    , myDirection{other.myDirection}
    , myState{other.myState}
{}
```

Here we initialize the member variables of the new instance with the values from the object `other`.

---

#### Move constructor
The move constructor is used to create a new object by moving resources from another object.

For a simple class such as `Gpio`, where we only store small built-in types, there are actually no heavy resources to move. Despite this, it is still useful to demonstrate the syntax.

```cpp
#include <utility>

// Create led1.
Gpio led1{13U, Direction::Output};

// Move led1 into led2.
Gpio led2{std::move(led1)};
```

**Note**: To invoke the move constructor, the function `std::move()` from `<utility>` is used.

An explicit move constructor can be implemented as follows:

```cpp
/**
 * @brief Move constructor.
 *
 * @param[in, out] other GPIO instance to move from.
 */
Gpio(Gpio&& other) noexcept
    : myPin{other.myPin}
    , myDirection{other.myDirection}
    , myState{other.myState}
{
    // Clear resources allocated by `other`
    other.myState = false;
}
```
**Notes**:
* `other` is an **rvalue reference**, written as `Gpio&&`. This allows the object's resources to be moved instead of copied.
* We cannot modify `other.myPin` or `other.myDirection`, because these member variables are declared as `const`.
* For this class, moving is in practice identical to copying, since no resources are owned dynamically.
* If `other` had contained other resources, such as dynamically allocated memory, ownership of these would have been transferred to the new object, and `other` would have been left without them (for example with its pointer set to `nullptr`).

---

#### Copy assignment operator
A copy assignment operator is used when an already existing object should be assigned from another object.

```cpp
// Create led1 and led2.
Gpio led1{13U, Direction::Output};
Gpio led2{12U, Direction::Output};

// Make led2 a copy of led1.
led2 = led1;
```

For the class `Gpio`, however, this is problematic. During assignment, an already existing object must be overwritten, but `myPin` and `myDirection` are constant and may not be changed after initialization.

An otherwise simplified operator could have looked like this:

```cpp
/**
 * @brief Copy assignment operator.
 *
 * @param[in] other GPIO instance to copy from.
 *
 * @return Reference to this instance.
 */
Gpio& operator=(const Gpio& other) noexcept
{
    if (this != &other)
    {
        myState = other.myState;
    }
    return *this;
}
```

Note that we **cannot** write:

```cpp
myPin       = other.myPin;
myDirection = other.myDirection;
```

since these are constant.

---

#### Move assignment operator
Similarly, there is a move assignment operator, which is used when an object is assigned from a temporary object or via `std::move` from `<utility>`:

```cpp
#include <utility>

Gpio led1{13U, Direction::Output};
Gpio led2{12U, Direction::Output};

led2 = std::move(led1);
```

A simplified implementation could look like this:

```cpp
/**
 * @brief Move assignment operator.
 *
 * @param[in, out] other GPIO instance to move from.
 *
 * @return Reference to this instance.
 */
Gpio& operator=(Gpio&& other) noexcept
{
    if (this != &other)
    {
        myState       = other.myState;
        other.myState = false;
    }
    return *this;
}
```
Here as well, however, we cannot move `myPin` or `myDirection`, since these are constant.

---

### Recommendation
In many real embedded systems, copying and moving are often completely disallowed, since an object frequently represents a unique hardware resource.

The deleted copy and move operations are normally placed at the bottom of the public section:

```cpp
Gpio(const Gpio&)            = delete; // No copy constructor.
Gpio(Gpio&&)                 = delete; // No move constructor.
Gpio& operator=(const Gpio&) = delete; // No copy assignment.
Gpio& operator=(Gpio&&)      = delete; // No move assignment.
```

In this way, it is ensured that each `Gpio` object represents exactly one physical GPIO pin and cannot be duplicated by mistake.

---
