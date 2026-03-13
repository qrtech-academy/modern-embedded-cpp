# Appendix B

## From C to Modern Embedded C++

### 1. Why Use C++ in Embedded Systems?
Embedded systems increasingly use C++ because it enables:
* Better abstraction (via classes and interfaces).
* Stronger type safety through stricter compile-time checks.
* Compile-time guarantees (e.g. `static_assert`, templates).
* Improved modularity and separation of concerns.
* Reusable and testable drivers.

Importantly, C++ provides these benefits while still allowing low-level hardware access and zero-cost abstractions.

Unlike many desktop C++ applications, embedded systems often:
* Disable exceptions (and mark functions `noexcept`).
* Avoid dynamic allocation (`malloc/free` or `new/delete`), or limit it to system initialization.
* Prefer deterministic execution, meaning that the execution time of critical code paths must be predictable and bounded.

---

### 2. Key Differences Between C and C++

| Feature                   | C       | C++      |
|---------------------------|---------|----------|
| Namespaces                | No      | Yes      |
| Classes                   | No      | Yes      |
| Templates                 | No      | Yes      |
| References                | No      | Yes      |
| Compile‑time polymorphism | No      | Yes      |
| Standard containers       | Limited | Rich STL |
| RAII                      | No      | Yes      |

**Note**: RAII *(Resource Acquisition Is Initialization)* means that a resource is acquired when an object is created and automatically released when the object goes out of scope.

---

### 3. Namespaces
Namespaces are used to avoid symbol collisions in large software systems.
They can be compared to folders on a computer:
* Just as different folders can contain files with the same name, different namespaces can contain functions, classes, variables, and other entities with the same name.
* For example, two different namespaces may both contain a function called `init()` without causing a conflict.

A namespace is defined using the `namespace` keyword, followed by a name and a block of code enclosed in braces.

Below is a simple example of a namespace named `driver`:

```cpp
namespace driver
{

} // namespace driver
```

Anything declared inside this block becomes part of the `driver` namespace.

For example, the namespace may contain a function called `init()`:

```cpp
namespace driver
{
void init() 
{
    // Driver initialization code.
}
} // namespace driver
```

To call this function from outside the namespace, we prefix it with the namespace name followed by the scope resolution operator `::`:

```cpp
int main()
{
    driver::init();
    return 0;
}
```

**Note**:
* Most functionality from the C++ standard library is accessed through namespace `std`.
* Therefore, it is common to write `std::` before standard library types and functions, for example `std::uint8_t`, `std::uint16_t`, `std::size_t`, and `std::printf`.
* C library functionality is also available in C++, but it is usually included through C++ headers such as `<cstdint>`, `<cstdio>`, and `<cstring>`.
* In modern C++, these headers are generally preferred over the older C-style forms `<stdint.h>`, `<stdio.h>`, and `<string.h>`.

#### Anonymous namespaces
In C++, an anonymous namespace can be used to restrict symbols (variables, functions, types, and compile-time constants) to a single translation unit (i.e. a single source file).

This is similar to using the keyword static for functions or variables in C.

Anonymous namespaces are typically used in .cpp files to hide internal helper functions or variables that should not be visible outside the file.

Example:

```cpp
namespace
{
// Content only visible within this source file.
void helper() {}
} // namespace
```

#### Nested namespaces
Namespaces can also be nested, similar to how folders can contain subfolders:
* Namespaces are often used to reflect the logical structure of the project.
* For example, assume a project has the following directory structure:

```text
include/
    driver/
        gpio.h
        timer.h
source/
    driver/
        gpio.cpp
        timer.cpp
    main.cpp
```

A common convention would be:
* The content of `driver/gpio.h` and `driver/gpio.cpp` belongs to the namespace `driver::gpio`.
* The content of `driver/timer.h` and `driver/timer.cpp` belongs to the namespace `driver::timer`.

This makes it easy to see which module a function belongs to.

#### Nested namespace syntax
Modern C++ allows nested namespaces to be declared in a compact form:

```cpp
namespace driver::gpio
{
void init() 
{
    // GPIO initialization code.
}
} // namespace driver::gpio
```

Before C++17, the same namespace would typically be written as:

```cpp
namespace driver
{
namespace gpio
{
void init() 
{
    // GPIO initialization code.
}
} // namespace gpio
} // namespace driver
```

Both forms are equivalent, but the compact form is preferred in modern C++.

#### Using functions from nested namespaces
A function defined in namespace `driver::gpio` can be called using its fully qualified name:

```cpp
int main()
{
    driver::gpio::init();
    return 0;
}
```

This makes it clear which module the function belongs to and prevents naming conflicts in larger projects.

**In short**:
In embedded systems, namespaces are commonly used to organize drivers, hardware abstraction layers (HAL), and communication protocols.

---

### 4. `constexpr`
The keyword `constexpr` indicates that a value or expression (including simple functions) can be evaluated at compile time rather than at runtime. This allows the compiler to compute values in advance, which can improve performance and ensure that such values are usable in constant-expression contexts.

In embedded systems, `constexpr` is particularly useful for defining hardware-related constants such as clock frequencies, register offsets, buffer sizes, and bit masks.

For example:

``` cpp
/** CPU frequency in Hz. */
constexpr std::uint32_t cpuFrequency{16000000U};

/** Baud rate in bps (bits per second). */
constexpr std::uint32_t baudrate{115200U};

/** Buffer length in bytes. */
constexpr std::size_t bufLen{20U};
```

**Note**: Brace initialization `{}` is used instead of `=` above:
* This is advantageous, since `{}` can be used to initialize everything.
* An exception is when using the `auto` keyword prior to C++17; then `auto` should be used in combination with `=` to avoid creating so called initializer lists.    

Because these values are known at compile time, the compiler can replace their usage directly with the computed value in the generated machine code.

Compared to macros (`#define`), `constexpr` has several advantages:
* It is type-safe.
* It obeys normal C++ scoping rules (functions, local blocks, and namespaces).
* It can be used in constant expressions.

For example, `constexpr` values can be used for:
* Array sizes.
* Template parameters.
* Compile-time calculations.

In modern C++, `constexpr` is generally preferred over `#define` for defining constants.

`constexpr` can be used for functions when the values are known at compile-time:

```cpp
constexpr int add(const int x, const int y) { return x + y; }
```

---


### 5. `noexcept`
The keyword `noexcept` indicates that a function is guaranteed not to throw exceptions.

For reference, an example of throwing an exception is shown below.
In this example, an exception is thrown if the value of `val` exceeds `100U`:

```cpp
#include <cstdint>
#include <stdexcept>

int main()
{
    constexpr std::uint8_t limit{100U};
    std::uint8_t val{};
    
    while (1)
    {
        if (limit < val++)
        {
            throw std::out_of_range("Value exceeded limit!");
        }
    }
    return 0;
}
```

Exceptions are a mechanism used in many C++ programs to signal runtime errors:
* When an exception is thrown, the program begins stack unwinding, meaning that the call stack is traversed until a matching exception handler (i.e. a catch block) is found.
* If a function marked `noexcept` attempts to throw an exception, the program will terminate immediately.

In many embedded systems, exceptions are disabled entirely. This is typically done to:
* Reduce binary size.
* Simplify the runtime environment.
* Ensure deterministic behavior.

In such systems, errors are typically handled using C-style mechanisms such as:
* Return codes.
* Status flags.
* Error callbacks.

Because of this, it is common practice in embedded C++ to mark functions that are not expected to fail with the `noexcept` keyword.

Doing so has several benefits:
* It documents that the function cannot throw exceptions.
* It enables additional compiler optimizations.
* It avoids the need for exception-related stack handling.

Example:

```cpp
namespace driver::gpio
{
bool read() noexcept { return false; }
} // namespace driver::gpio
```

**Important:**
* In embedded drivers, functions that perform simple hardware operations (such as reading or writing registers) are typically marked `noexcept`, since they are not expected to throw exceptions.
* Functions that may perform dynamic memory allocation, such as resizing a vector, should not be marked `noexcept`, since memory allocation may fail.

---

### 6. Default Arguments
C++ allows functions to specify default values for parameters:
* If the caller does not provide a value for a parameter, the default value is used automatically.
* This can simplify APIs by allowing common cases to use fewer arguments.

For example, consider the function `log()` below:

``` cpp
namespace debug
{
void log(const char* message, std::uint8_t level = 0U)
{
    // Log message with the specified level.
}
} // namespace debug
```

The parameter `level` has a default value of `0U`. This means the function can be called in two different ways:
* If the log level is not important, the function can be called with only the message:

``` cpp
debug::log("System started");
```

In this case, the value `0U` will automatically be used for `level`.

* If a specific log level is desired, the caller can provide it explicitly:

``` cpp
debug::log("Driver failure", 2U);
```

Default arguments are commonly used in embedded software for functions that have sensible default behavior, such as:
* Logging systems.
* Driver configuration functions.
* Initialization routines.

They allow APIs to remain flexible while keeping common usage simple.

#### Default argument placement
In C++, default arguments may only be omitted from right to left. This means that parameters with default values must be placed at the end of the parameter list.

A valid example is shown below:

```cpp
void printValue(const std::uint32_t value, const bool hex = false) noexcept;
```

This function can be called in either of the following ways:

```cpp
printValue(42U);
printValue(42U, true);
```

The following is not valid:

```cpp
void printValue(const std::uint32_t value = 0U, const bool hex) noexcept;
```

This is not allowed because the second parameter does not have a default value, even though the first one does.

If this were allowed, a call such as this:

```cpp
printValue(true);
```

would become unclear and potentially cause errors (`true` can be implicitly converted to `value` 
as `1`).

---

### 7. Modern C++ Structs
In C++, a `struct` can contain not only data members but also member functions, often referred to as methods.  
This allows related data and operations to be grouped together in a single type.

Consider the following `driver::Gpio` driver, implemented as a stub (for simulation):

```cpp
#include <cstdint>

namespace driver
{
/**
 * @brief GPIO stub driver.
 */
struct Gpio
{
    /** Pin the GPIO is connected to. */
    const std::uint8_t pin;

    /** GPIO state. */
    bool state;

    /**
     * @brief Set GPIO state.
     * 
     * @param[in] state GPIO state (true = enabled, false = disabled).
     */
    void write(const bool state) noexcept 
    { 
        // Note: The 'this' keyword is used here to refer to our member variable 'state',
        // since we have an input argument with the same name.
        this->state = state; 
    }

    /**
     * @brief Get GPIO state.
     *
     * @return True if the GPIO is enabled, false otherwise.
     *
     * @note The 'const' keyword is used after the method  name to set the GPIO instance to
     *       read-only in the scope of this method.
     */
    bool read() const noexcept { return state; }
};
} // namespace driver
```

The `driver::Gpio` struct contains:
* Two data members (`pin` and `state`).
* Two methods (`write` and `read`).

These functions operate on the data stored in the struct.

Instances of the struct can be created as follows:

``` cpp
// Initialize LED connected to pin 9, state set to false.
driver::Gpio led{9U, false};

// Initialize button connected to pin 13, simulate pressdown at startup.
driver::Gpio button{13U, true};
```

Note that unlike in C, the keyword `struct` is not required when creating an instance of a struct and can therefore be omitted.

The functions can then be called using the dot operator, just like the data members:

``` cpp
// Enable the LED if the button is pressed.
const bool buttonPressed{button.read()};
led.write(buttonPressed);
```

In traditional C, similar functionality would typically be implemented using a struct combined with separate functions that take a pointer to the struct, as shown below:

```c
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief GPIO stub driver.
 */
typedef struct
{
    /** Pin the GPIO is connected to. */
    const std::uint8_t pin;

    /** GPIO state. */
    bool state;
} gpio_t;

/**
 * @brief Set GPIO state.
 * 
 * @param[in, out] self Pointer to the GPIO.
 * @param[in] state GPIO state (true = enabled, false = disabled).
 */
void gpio_write(gpio_t* self, const bool state)
{
    if (NULL == self) { return; }
    self->state = state;
}

/**
 * @brief Get GPIO state.
 *
 * @param[in] self Pointer to the GPIO.
 *
 * @return True if the GPIO is enabled, false otherwise.
 */
bool gpio_read(const gpio_t* self)
{
    return NULL != self ? self->state : false; 
}
```

Instances of the struct can then be created and used as shown below:

``` cpp
// Initialize LED connected to pin 9, state set to false.
gpio_t led = {9U, false};

// Initialize button connected to pin 13, simulate pressdown at startup.
gpio_t button = {13U, true};

// Enable the LED if the button is pressed.
const bool button_pressed = gpio_read(&button);
gpio_write(&led, button_pressed);
```

#### Comparison

##### C-style approach

``` c
gpio_write(&led, true);
```

##### C++ approach

``` cpp
led.write(true);
```

The C++ syntax improves readability and keeps related functionality together, making the code easier to understand and maintain.

#### Constructor and destructor
It's also possible to add initialization and cleanup methods. These methods are known as:
* Constructor: 
    * Has the same name as the struct.
    * Called automatically when an object is created.
    * Can be used to automatically initialize an object on creation (for instance pin configuration).
    * For example, a constructor for the `driver::Gpio` struct above can be implemented as follows:

```cpp
/**
 * @brief Constructor.
 *
 * @param[in] pin Pin the GPIO is connected to.
 * @param[in] initialState Initial state (default = disabled).
 */
Gpio(const std::uint8_t pin, const bool initialState = false) noexcept
    : pin{pin}
    , state{initialState}
{
    std::printf("Initializing GPIO at pin %u!\n", pin);
    // Configure pin here!
}
```

* Destructor:
    * Has the same name as the struct, but with prefix `~`.
    * Called automatically when an object is destroyed.
    * For example, this happens when an object goes out of scope or when `delete` is used.
    * Can be used to automatically release allocated resources or undo hardware-related setup.
    * Often used to release peripherals, disable interrupts, or restore hardware state in embedded systems.

```cpp
/**
 * @brief Destructor.
 */
~Gpio() noexcept
{
    std::printf("Releasing resources reserved for GPIO at pin %u!\n", pin);
    // Release allocated resources here.
}
```

After adding the constructor and destructor, the struct `driver::Gpio` looks like this:

```cpp
#include <cstdint>
#include <cstdio>

namespace driver
{
/**
 * @brief GPIO stub driver.
 */
struct Gpio
{
    /** Pin the GPIO is connected to. */
    const std::uint8_t pin;

    /** GPIO state. */
    bool state;

    /**
     * @brief Constructor.
     *
     * @param[in] pin Pin the GPIO is connected to.
     * @param[in] initialState Initial state (default = disabled).
     */
    Gpio(const std::uint8_t pin, const bool initialState = false) noexcept
        : pin{pin}
        , state{initialState}
    {
        std::printf("Initializing GPIO at pin %u!\n", pin);
        // Configure pin here!
    }
    
    /**
     * @brief Destructor.
     */
    ~Gpio() noexcept
    {
        std::printf("Releasing resources reserved for GPIO at pin %u!\n", pin);
        // Release allocated resources here.
    }

    /**
     * @brief Set GPIO state.
     * 
     * @param[in] state GPIO state (true = enabled, false = disabled).
     */
    void write(const bool state) noexcept
    {
        // Note: The 'this' keyword refers to the current object.
        // It is used here because the parameter name 'state' hides the member variable.
        this->state = state;
    }

    /**
     * @brief Get GPIO state.
     *
     * @return True if the GPIO is enabled, false otherwise.
     *
     * @note The 'const' keyword is used after the method name to set the GPIO instance to
     *       read-only in the scope of this method.
     */
    bool read() const noexcept { return state; }
};
} // namespace driver
```

Before a constructor is defined, the struct can be initialized using aggregate initialization. After a user-defined constructor is added, the same brace syntax may still be used, but it now calls the constructor instead.

For example, the following initialization:

```cpp
int main()
{
    driver::Gpio led{9U, false};
    led.write(true);
}
```

will generate the following output because the constructor and destructor are called automatically:

```text
Initializing GPIO at pin 9!
Releasing resources reserved for GPIO at pin 9!
```

This illustrates the idea behind **RAII** (*Resource Acquisition Is Initialization*):
* Initialization is performed during object creation.
* Cleanup is performed automatically when the object is destroyed.

#### Introduction to encapsulation
In C++, parts of a struct can be made private using the `private` keyword, as shown below for the `driver::Gpio` struct:
* The `public` keyword has been added for clarity; in a C++ struct, everything is public by default.
* The `private` keyword has been added to create a private segment.
* The member variables and methods have been removed for simplicity.

```cpp
namespace driver
{
struct Gpio
{
public:
    // Public segment - accessible outside the struct.
private:
    // Private segment - inaccessible outside the struct.
};
} // namespace driver
```

Using the `private` keyword has an important advantage:
* Symbols (member variables, constants, and methods) declared in the private segment are inaccessible outside the struct.
* This allows us to hide implementation details and data that should not be accessed directly by other parts of the program.
* In many designs, member variables are declared private. This ensures that the internal state of the object can only be modified through its member functions.

For instance, the member variables of the `driver::Gpio` struct are typically made private, which is shown below: 
* The private segment is typically placed at the bottom of the struct, while the public interface (constructors and methods) is placed first. This makes it easier to see how the struct should be used.
* Here we add the `my` prefix to make it clear that these are member variables and to reduce the need to use the `this` keyword (`this->state = state` was used earlier).

```cpp
namespace driver
{
struct Gpio
{
private:
    /** Pin the GPIO is connected to. */
    const std::uint8_t myPin;

    /** GPIO state. */
    bool myState;
};
} // namespace driver
```

**Note:** The snippet above only shows the private segment of `driver::Gpio`.

Then we update the struct:
* Member variables `pin` and `state` are replaced with `myPin` and `myState`.
* The `pin()` method is added so that users still can read the pin number.

```cpp
#include <cstdint>
#include <cstdio>

namespace driver
{
/**
 * @brief GPIO stub driver.
 */
struct Gpio
{
    /**
     * @brief Constructor.
     *
     * @param[in] pin Pin the GPIO is connected to.
     * @param[in] initialState Initial state (default = disabled).
     */
    Gpio(const std::uint8_t pin, const bool initialState = false) noexcept
        : myPin{pin}
        , myState{initialState}
    {
        std::printf("Initializing GPIO at pin %u!\n", myPin);
        // Configure pin here!
    }
    
    /**
     * @brief Destructor.
     */
    ~Gpio() noexcept
    {
        std::printf("Releasing resources reserved for GPIO at pin %u!\n", myPin);
        // Release allocated resources here.
    }

    /**
     * @brief Get GPIO pin number.
     *
     * @return Pin the GPIO is connected to.
     */
    std::uint8_t pin() const noexcept { return myPin; }

    /**
     * @brief Set GPIO state.
     * 
     * @param[in] state GPIO state (true = enabled, false = disabled).
     */
    void write(const bool state) noexcept { myState = state; }

    /**
     * @brief Get GPIO state.
     *
     * @return True if the GPIO is enabled, false otherwise.
     */
    bool read() const noexcept { return myState; }

private:
    /** Pin the GPIO is connected to. */
    const std::uint8_t myPin;

    /** GPIO state. */
    bool myState;
};
} // namespace driver
```

After making the member variables private, the compiler will generate an error if we try to access them directly:

```cpp
int main()
{
    driver::Gpio led{9U};

    // Allowed: pin() provides controlled read access to the pin number.
    std::printf("The LED is connected to pin %u!\n", led.pin());

    // Allowed: write() is part of the public interface.
    led.write(true);

    // Not allowed: myPin is private.
    // std::printf("Pin: %u!\n", led.myPin);

    // Not allowed: myState is private.
    // led.myState = true;
}
```

Later in the course we will introduce classes:
* In C++, structs and classes support almost the same language features, but classes use private as the default access level, while structs use public. 
* Structs are often used for simple data-oriented types, while classes are commonly used for more complex objects that may involve inheritance, polymorphism, or custom copy and move semantics.

---

### 8. References
References provide an alternative way to pass variables to functions.

A reference acts as an alias for another variable. This means that operations performed on the reference directly affect the original variable. References behave similarly to pointers but can be used with normal variable syntax.

References are declared using the `&` symbol.

Consider the following example:

``` cpp
void toggle(bool& state)
{
    state = !state;
}
```

The parameter `state` is a reference to a `bool`.

This means the function modifies the original variable that was passed to it.

Example usage:

``` cpp
bool state{false};

toggle(state);
```

After the function call, the value of `state` will be `true`.

In C, similar behavior would typically be implemented using pointers:

``` c
void toggle(bool* state)
{
    *state = !(*state);
}
```

### Comparison

#### C-style approach (with pointer)

``` c
toggle(&state);
```

#### C++ approach (with reference)

``` cpp
toggle(state);
```

While pointers are still used in C++, references are often preferred because they:
* Cannot be `nullptr` (`NULL` in C).
* Do not require the address operator (`&`) to pass addresses.
* Do not require dereferencing (`*`) to read the value a pointer is pointing at.
* Provide clearer syntax.

As a result, references are commonly used in modern C++ when a function needs to modify an existing variable. However, a reference cannot be reseated to refer to another object after initialization; pointers are still used when we need to change the address we refer to, for instance when implementing dynamic arrays.

---

### 9. The `auto` keyword
The keyword `auto` allows the compiler to automatically deduce the type of a variable from its initializer.

For example:

``` cpp
auto number  = 5;   // Deduced as int.
auto counter = 10U; // Deduced as unsigned int
auto voltage = 3.3; // Deduced as double.
```

This can improve readability and reduce the need to repeat long type names.

However, care must be taken when using `auto` with brace initialization `{}` in C++11/14. In some cases the compiler may deduce an `std::initializer_list` instead of the intended type.

For example:

``` cpp
auto a = 10; // Deduced as int.
auto b{10};  // Deduced std::initializer_list<int> prior to C++17.
```

Because of this, when using `auto` before C++17 it was generally recommended to use the assignment operator `=` instead of brace initialization `{}`.

In modern C++ (C++17 and later), the behavior was improved and brace initialization works more intuitively. However, a common rule of thumb is still:
* When using `auto`, prefer `=` instead of `{}` unless you explicitly want an `std::initializer_list`.

However, in this course the data type will usually be specified explicitly.  
The `auto` keyword will mainly be used in situations where the type would otherwise be verbose, for example when iterating over containers:

```cpp
// Enable all LEDs in a list.
for (const auto& led : leds)
{
    led.on();
}
```

Without `auto`, the type may become very long.

---

### 10. Function Templates - A First Look
**Note:** Here we only introduce the basic idea. Templates will be covered in more detail later in the course.

Templates allow functions and classes to operate on multiple data types without duplicating code.

In embedded systems, templates are often used to implement generic utilities such as bit manipulation helpers.

Consider the following example to set a bit `bit` in a given register `reg`:
* The data type of the register for which to write is set to `T`. The type is set at compile time when the function is instantiated.
* We constrain this function to only work for integral types with a `static_assert`. If this function is called with some other type, such as a floating point number, a compiler error with error message `Failed to set bit in register: T must be of integral type!` is generated.
* We check the data type using `std::is_integral<T>` from `<type_traits>`, specifically via its member variable `value`:
  * If `T` is of integral type, `std::is_integral<T>::value` is `true` and the assertion succeeds.
  * If `T` isn't of integral type, `std::is_integral<T>::value` is `false` and the assertion fails.
  * For more information about type traits, see [Additional information about type traits](#additional-information-about-type-traits).
* The value `1` is cast to type `T` via a `static_cast`, which is a safer alternative to regular C casts.

```cpp
#include <cstdint>
#include <type_traits>

/**
 * @brief Set bit in register.
 *
 * @tparam T The register type. Must be integral.
 *
 * @param[out] reg Register to write to.
 * @param[in] bit The bit to set.
 */
template<typename T>
constexpr void set(T& reg, const std::uint8_t bit) noexcept
{
    static_assert(std::is_integral<T>::value,
        "Failed to set bit in register: T must be of integral type!");
    reg |= (static_cast<T>(1U) << bit);
}
```

The function can be used to set bits in registers of arbitrary integral type.   
In the example below, bits 1-2 in an 8-bit register are set by instantiating the `set()` function twice:

```cpp
std::uint8_t reg{};
set(reg, 1U);
set(reg, 2U);
```

**Note**: Unlike C, functions in C++ can have the same name as long as their parameter lists differ. The compiler determines which version to use at each function call based on the types of the arguments. This is called function overloading.

#### Template instantiation and code size
Templates are instantiated separately for each data type used in the program.  
For example, if the function `set()` is used with both `std::uint8_t` and `std::uint32_t`:

```cpp
std::uint8_t reg1{};
std::uint32_t reg2{};

set(reg1, 1U);
set(reg2, 12U);
```

Then the compiler generates two separate versions of the function:

```cpp
set(std::uint8_t&, std::uint8_t)
set(std::uint32_t&, std::uint8_t)
```

Each version is compiled independently and included in the final binary:
* This behavior is one of the reasons templates are considered zero‑cost abstractions;
the generated code is specialized for the exact type being used, which allows the compiler to optimize the implementation.
* However, in embedded systems this also means that excessive template usage can increase the
binary size, since multiple versions of the same function may be generated.
* For this reason, templates should be used carefully in resource‑constrained systems,
especially when many different data types are involved.

#### Parameter packs
We can also implement parameter packs so that multiple bits can be set at once:

```cpp
#include <cstdint>
#include <type_traits>

template<typename T, typename... Bits>
constexpr void set(T& reg, const Bits... bits) noexcept
{
    static_assert(std::is_integral<T>::value,
        "Failed to set bit in register: T must be of integral type!");

    for (const auto& bit : {bits...})
    {
        reg |= (static_cast<T>(1U) << bit);
    }
}
```

In the example below, bits 1-5 in an 8-bit register are set by instantiating the `set()` function once:

```cpp
// Set bit 1-5 in a register.
std::uint8_t reg{};
set(reg, 1U, 2U, 3U, 4U, 5U);
```

The compiler will generate code similar to the following:

```cpp
reg |= (static_cast<T>(1U) << 1U);
reg |= (static_cast<T>(1U) << 2U);
reg |= (static_cast<T>(1U) << 3U);
reg |= (static_cast<T>(1U) << 4U);
reg |= (static_cast<T>(1U) << 5U);
```

##### Fold Expressions (C++17)
In the implementation above we iterate over the bits using a loop and a so-called **braced initializer list**.  
This approach is easy to read and resembles how similar logic might be written in C.

Since C++17, we can omit the loop by using a fold expression:

```cpp
(reg |= (static_cast<T>(1U) << bits), ...);
```

This expression expands the parameter pack `bits...` and performs the operation once for each element.
Because of this feature, the `set()` function can be implemented more compactly:


```cpp
template<typename T, typename... Bits>
constexpr void set(T& reg, const Bits... bits) noexcept
{
    static_assert(std::is_integral<T>::value,
        "Failed to set bit in register: T must be of integral type!");
    (reg |= (static_cast<T>(1U) << bits), ...);
}
```

---

### Additional information about type traits
Type traits allow programs to inspect and reason about types at compile time.

In the C++ standard library, many type traits are implemented as small structs containing a static constant boolean value (`static` means that the value belongs to the type itself rather than to an instance of the struct).

We can implement a simplified version of a trait that checks whether a type is unsigned:

```cpp
template<typename T>
struct isUnsigned
{
    static constexpr bool value{false};
};
```

**Note**: 
* The type trait is nothing more than a struct template containing a boolean constant `value`.
* By default, this boolean value is `false` for all types `T`.
* We can specialize the template for specific types `T` so that `value` becomes `true`, as shown below, where the types `std::uint8_t`, `std::uint16_t`, `std::uint32_t`, `std::uint64_t`, and `std::size_t` 
are treated as unsigned types:

```cpp
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

template<>
struct isUnsigned<std::uint64_t>
{
    static constexpr bool value{true};
};

template<>
struct isUnsigned<std::size_t>
{
    static constexpr bool value{true};
};
```

We can use our own type trait `isUnsigned<T>` to check if a type `T` is unsigned as shown below:

```cpp
static_assert(isUnsigned<T>::value, "T must be of unsigned type!");
```

Since C++17, it is common to also provide a variable template ending in `_v` that exposes the boolean value directly:

```cpp
template<typename T>
inline constexpr bool isUnsigned_v{isUnsigned<T>::value};
```

**Note**: `inline` allows the variable template to be defined in a header file and included in multiple translation units without causing multiple definition errors.

This variable template allows us to write:

```cpp
static_assert(isUnsigned_v<T>, "T must be of unsigned type!");
```

In this course, the traditional form `isUnsigned<T>::value` will be used for clarity.

---

### Summary
This appendix introduced the following modern C++ features commonly used in embedded software:
* Namespaces.
* `constexpr` and compile-time constants.
* `noexcept` and exception handling in embedded systems.
* Default arguments.
* Structs with member functions, constructors/destructors, and encapsulation.
* References.
* Function templates.

These features allow developers to write safer, clearer, and more maintainable embedded software while still maintaining full control over hardware and performance.

---
