# Appendix A

## Classes in C++

### Background
To maintain good structure in larger programs, there is an increasing need to group related data and related behavior into a single unit instead of spreading everything across multiple separate variables and functions. For example, it may be beneficial to represent a GPIO pin through a class that stores the pin number, direction, and current state, while the class also contains functions for writing, reading, and toggling the pin.

In programming, the most common data structures used to store related data as a single unit are so-called structs and classes:
* **Structs** are mainly used in C (the concept of classes does not exist at all in C).
* **Classes** are used in many modern programming languages, such as C++, Java, and Python.
* In C++, both structs and classes are used.
* Classes can be seen as a further development of structs, which are relatively simple. In terms of structure, however, these data structures are very similar.

Languages that support classes are said to support object orientation, or OOP *(Object Oriented Programming).* C is considered a procedural language because it is based on programming via functions. Through structs, however, some object-oriented principles can be mimicked, which will be demonstrated below.

Some object-oriented languages, such as Java and C#, are purely object-oriented, meaning that classes must be used. Other programming languages, such as C++ and Python, are said to be multi-paradigm languages, since it is possible to program both in an object-oriented way (as in Java) and in a procedural way (as in C), or a combination of both.

---

### Terminology
As mentioned earlier, classes are used instead of structs in most conventional programming languages created after C.

In C++, however, structs are almost identical to classes; the only formal difference is that class members are private by default, whereas they are public by default in structs. Structs in C++ can therefore be used in the same way as classes.

To reduce confusion in the rest of this section, however, we will use the following **convention in this course material**:
* **Struct** refers to simple data structures without member functions, for example:
    * Pure C-style structs (sometimes called POD – *Plain Old Data*).
    * Structs containing constants (`static constexpr`).
* **Class** refers to a more advanced variant of the data structure we have seen so far.

---

### Differences between classes and structs
Classes can be seen as a further development of structs. Therefore, classes and structs are similar in terms of their fundamental structure, but classes offer much more functionality and flexibility. In this section, we will look at the most basic functionality that classes provide.

Some of the most significant differences between classes and structs are:
* **Classes support encapsulation,** which means that you can choose what is accessible/visible outside the class. This allows secret data or information that is not relevant outside the class (such as implementation details) to be hidden.
* **Classes can contain functions,** which means we do not need to implement associated functions using struct pointers, as shown previously.
* **Classes offer functionality for inheritance,** which means a new class can obtain the contents of a base class. This can significantly reduce the amount of code required.
* **Classes can be implemented using so-called class templates** to make one or more data types selectable, similar to the function templates we saw earlier. An example of a class template in the standard library is *std::vector*, which allows us to choose what should be stored in a given vector.

---

### Basic structure of a class
We implement a simple example of a GPIO driver in C++ using a class named `Gpio`. The class is placed in the namespace `driver::gpio`, which is common in larger C++ projects to organize code and avoid name collisions.

We will use `camelCase` instead of `snake_case`, which means that:
* We use capital letters instead of underscores in function and type names. So instead of a function name such as `print_numbers()`, we will name this function `printNumbers()`.
* The same pattern is followed for types such as structs, classes, and enumerations. One difference, however, is that type names begin with a capital letter, for example `Gpio` instead of `gpio`, `Direction` instead of `direction`, and so on.

---

#### 1. Add include directives
We begin by including the standard header `<cstdint>`, so that we have access to data types such as `std::uint8_t`:

```cpp
#include <cstdint>
```

---

#### 2. Create namespace and enumeration class
Next, we create the namespace `driver::gpio` and the enumeration class `Direction` to describe the direction of a GPIO pin. We set the underlying data type of the enumeration to `std::uint8_t`, which makes the type compact and ensures that no negative values can be represented (which can simplify validation):

```cpp
namespace driver::gpio
{
/**
 * @brief GPIO direction configuration.
 */
enum class Direction : std::uint8_t
{
    Input,       ///< Input without pull-up.
    InputPullup, ///< Input with pull-up enabled.
    Output,      ///< Output.
    Count,       ///< Number of supported directions.
};
} // namespace driver::gpio
```

The enumeration class above is used to specify whether a pin should be:
* `Input`
* `InputPullup`
* `Output`

We can also easily check whether a given enumerator `direction` is valid by ensuring that it has a numeric value that is less than `Direction::Count`:

```cpp
[[nodiscard]] constexpr bool isDirectionValid(const Direction direction) noexcept
{
    return static_cast<std::uint8_t>(Direction::Count) > static_cast<std::uint8_t>(direction);
}
```

The primary reason we choose to implement `Direction` as an enumeration class instead of a traditional enumeration is that we do not need to use prefixes to avoid name collisions; instead, this is built into the language.

Note above that the enumerators are now named
* `Input`,
* `InputPullup`, and
* `Output`

instead of, for example,
* `GPIO_DIRECTION_INPUT`,
* `GPIO_DIRECTION_INPUT_PULLUP`, and
* `GPIO_DIRECTION_OUTPUT`.

With the enumeration class above, we instead write
* `Direction::Input`,
* `Direction::InputPullup`, and
* `Direction::Output`.

In general, it is recommended to use enumeration classes instead of traditional enumerations when programming in modern C++, that is, C++11 and later.

---

#### 3. Define the class `Gpio`
Next, we define the class `Gpio` using the keyword `class`:

```cpp
namespace driver::gpio
{
/**
 * @brief GPIO driver.
 */
class Gpio final
{
public:

private:
};
} // namespace driver::gpio
```

We have added two keywords to the class, `public` and `private`:
* Everything that falls under the keyword `public` belongs to the public section:
    * The contents of this section are visible and accessible outside the class.
    * Everything that we want the "user" of the class to be able to see and use should be declared here.
    * For example, this may include methods for writing to the pin, reading its state, and toggling the output.
* Everything that falls under the keyword `private` belongs to the private section:
    * The contents of this section are not visible or accessible outside the class:
    * Here we place everything that we want to be inaccessible to the "user" of the class.
    * For example, this may include internal data or implementation details.

In our case, we place all member variables in the private section. We can also place methods that should only be used internally within the class here. Hiding information in this way is called **encapsulation**.

Common practice for a class is the following:
* Member variables are kept private.
* The public section appears first in the class so that the "user" can easily see what methods and other features are available if needed.
* Implementation details are placed below in the private part of the class, and the idea is that the user should not even need (or want) to read this far.

Usually, only the public part of the class is documented; the rest of the class consists of implementation details that are only of interest to the developer of the class and are therefore not documented. Since this is a pedagogical example, however, all code is documented.

We have also added the keyword `final` after the class name. This means that the class may not be used as a base class. In other words, no other class may inherit from `Gpio`. In embedded contexts, this is often a reasonable design choice for small, concrete drivers where you want to keep the design simple and clear.

---

#### 4. Add member variables
Under the private section we add the member variables, that is, the variables in the class that store the pin number, direction, and current state.  
Note that:
* We use the prefix `my` to avoid name collisions with methods that we will add later.
* We mark member variables that should never change after initialization as `const`.

```cpp
namespace driver::gpio
{
/**
 * @brief GPIO driver.
 */
class Gpio final
{
public:

private:
    /** GPIO pin number. */
    const std::uint8_t myPin;

    /** GPIO direction. */
    const Direction myDirection;

    /** GPIO state. */
    bool myState;
};
} // namespace driver::gpio
```

The member variables are private so that the "user" cannot read and write them arbitrarily. Instead, we later provide methods such as `write()`, `read()`, and `toggle()` to control how the object is used.

---

#### 5. Add constructors and destructors
Before adding the remaining methods, we add constructors, which can be seen as initialization routines that are automatically called when an object of the class is created.

First, we delete the so-called default constructor so that it is not possible to create a GPIO object without providing a pin number and direction:

```cpp
public:
    Gpio() = delete; // No default constructor.
```

This constructor is called the default constructor because it has no input arguments. Therefore, it is not possible to create an empty object as shown below:

```cpp
// Won't compile, since the default constructor is deleted.
driver::gpio::Gpio gpio{};
```

Normally, deleted functions, such as the default constructor above, are placed at the bottom of the public section.

We can also add a destructor, which can be seen as a function that is automatically called just before an instance of the given class is destroyed.

The destructor looks like the default constructor, with the difference that the tilde `~` is placed before the name of the class. If we do not need to do anything special before the object is destroyed, we can omit the destructor; the compiler will then create a default destructor for us. If we still want to create a destructor for clarity, we can explicitly set the destructor to default using the keyword `default`.

```cpp
/**
 * @brief Destructor.
 */
~Gpio() noexcept = default;
```

In modern C++, the keyword `default` is often recommended, which will be used in this example. However, it would also be possible to implement a default destructor as shown below:

```cpp
/**
 * @brief Destructor.
 */
~Gpio() noexcept {}
```

We want the user to specify the pin number, direction, and optionally the initial state directly when a GPIO object is created:
* We therefore create a constructor that takes the pin number, direction, and an initial state as input arguments.
* We mark the constructor with `explicit` so that it is not used for unintended implicit conversions.
* See [Appendix B](./b_classes2.md#the-keyword-explicit) for additional information about the keyword `explicit`.

```cpp
/**
 * @brief Create a new GPIO object.
 *
 * @param[in] pin GPIO pin number.
 * @param[in] direction GPIO direction.
 * @param[in] initialState Initial GPIO state (default = false).
 */
explicit Gpio(const std::uint8_t pin, const Direction direction, 
              const bool initialState = false) noexcept
{}
```
In the constructor above, we initialize the member variables so that:
* `myPin` is assigned the pin number stored in the input argument `pin`.
* `myDirection` is assigned the value stored in the input argument `direction`.
* `myState` is assigned the value stored in the input argument `initialState`. By default, `initialState` is set to `false`.

This is achieved by adding an initialization section between the function header and the function body (marked with curly braces {}). Note that we begin the initialization section with a colon, initialize using curly braces, and separate the initialization of each member variable with a comma.

```cpp
/**
 * @brief Create a new GPIO object.
 *
 * @param[in] pin GPIO pin number.
 * @param[in] direction GPIO direction.
 * @param[in] initialState Initial GPIO state (default = false).
 */
explicit Gpio(const std::uint8_t pin, const Direction direction, 
              const bool initialState = false) noexcept
    : myPin{pin}
    , myDirection{direction}
    , myState{initialState}
{}
```
If we needed to perform additional actions after initializing the member variables, we could place this in the constructor’s function body. In this case, however, we only want to initialize the member variables, so we do not need to add anything further.

The constructor shown above can now be used to initialize GPIO objects. Below is an example showing how we initialize the object `led` to represent an output on pin 13 with an initial low state:

```cpp
driver::gpio::Gpio led{13U, driver::gpio::Direction::Output, false};
```

Since the default value for the input argument that stores the initial state is `false`, we can also omit this argument:

```cpp
driver::gpio::Gpio led{13U, driver::gpio::Direction::Output};
```

We can also use the constructor to initialize an object `button` to represent an input with pull-up on pin 2:

```cpp
driver::gpio::Gpio button{2U, driver::gpio::Direction::InputPullup};
```

---

#### 6. Copy and move operations
In addition to regular constructors, C++ also provides special member functions for copying and moving. These are very important in modern C++, and it is useful to be familiar with them already when learning about classes.

The most common operations are shown below:

| Operation                          | Description                                                                             | Signature for class `Gpio`       |
| ---------------------------------- | --------------------------------------------------------------------------------------- | ------------------------------------ |
| **Copy constructor**               | Creates a new object as a copy of another object.                                      | `Gpio(const Gpio& other)`            |
| **Move constructor**               | Creates a new object by moving resources from another object.                          | `Gpio(Gpio&& other)`                 |
| **Copy assignment operator**       | Assigns an already existing object from another object.                                | `Gpio& operator=(const Gpio& other)` |
| **Move assignment operator**       | Assigns an already existing object by moving resources from another object.            | `Gpio& operator=(Gpio&& other)`      |

In many real embedded systems, copying and moving are often completely disallowed, since an object frequently represents a unique hardware resource. In this way, it is ensured that each `Gpio` object represents exactly one physical GPIO pin and cannot be duplicated by mistake.

For the class `Gpio`, copying and moving are deleted as shown below, normally placed at the bottom of the public section of the class.

```cpp
Gpio(const Gpio&)            = delete; // No copy constructor.
Gpio(Gpio&&)                 = delete; // No move constructor.
Gpio& operator=(const Gpio&) = delete; // No copy assignment.
Gpio& operator=(Gpio&&)      = delete; // No move assignment.
```

See [Appendix B](./b_classes2.md#copy-and-move-operations) for more information about copy and move operations.

---
#### 7. Add public methods
For a GPIO object, it is more natural to expose functionality in the form of behaviors rather than traditional getter and setter methods. We therefore add the following public methods:
* `write(bool state)` to write a new state to the pin.
* `read()` to read the current state of the pin.
* `toggle()` to toggle the state of the pin.

Below is the method `write()`, which is used to write a new state to the GPIO instance:

```cpp
/**
 * @brief Write a new state to the GPIO pin.
 *
 * @param[in] state The state to write.
 */
void write(const bool state) noexcept
{
    if (myDirection == Direction::Output)
    {
        myState = state;
    }
}
```
In the example above, we only allow writing if the pin is configured as an output. This is a simple way to illustrate that the class can protect its internal data and control how the object may be used.

Below is the method `read()`, which returns the current state of the pin:

```cpp
/**
 * @brief Read the current GPIO state.
 *
 * @return Current GPIO state.
 */
[[nodiscard]] bool read() const noexcept { return myState; }
```

Note that we write the keyword `const` directly after the function header, since we ensure that the class contents can only be read. This means that we cannot accidentally modify any of the class members.

We could use the method above to read and print the state of an LED that was previously implemented in software through the object `led`. Here, `std::printf()` from `<cstdio>` is used to print the state:

```cpp
int main()
{
    driver::gpio::Gpio led{13U, driver::gpio::Direction::Output};
    std::printf("State:\t%s\n", led.read() ? "High" : "Low");
    return 0;
}
```

The output becomes the following:

```
State:  Low
```

We then add the method `toggle()` to allow toggling the state of the pin:

```cpp
/**
 * @brief Toggle the GPIO state.
 */
void toggle() noexcept
{
    if (myDirection == Direction::Output)
    {
        myState = !myState;
    }
}
```

If, for example, we call `toggle()`, we ensure that the member variable `myState` switches between `false` and `true`, but only if the pin is configured as an output.

We can first call `write(true)` to set the LED pin high, and then call `toggle()` to switch it back to a low level. We print the state both before and after the calls:

```cpp
int main()
{
    driver::gpio::Gpio led{13U, driver::gpio::Direction::Output};
    std::printf("Old state:\t%s\n", led.read() ? "High" : "Low");
    led.write(true);
    std::printf("After write:\t%s\n", led.read() ? "High" : "Low");
    led.toggle();
    std::printf("After toggle:\t%s\n", led.read() ? "High" : "Low");
    return 0;
}
```

The output becomes the following:

```
Old state:      Low
After write:    High
After toggle:   Low
```

After adding the methods, the class `Gpio` looks like this:

```cpp
namespace driver::gpio
{
/**
 * @brief GPIO driver.
 */
class Gpio final
{
public:
    /**
     * @brief Create a new GPIO object.
     *
     * @param[in] pin GPIO pin number.
     * @param[in] direction GPIO direction.
     * @param[in] initialState Initial GPIO state (default = false).
     */
    explicit Gpio(const std::uint8_t pin, const Direction direction, 
                  const bool initialState = false) noexcept
        : myPin{pin}
        , myDirection{direction}
        , myState{initialState}
    {}

    /**
     * @brief Destructor.
     */
    ~Gpio() noexcept = default;

    /**
     * @brief Write a new state to the GPIO pin.
     *
     * @param[in] state The state to write.
     */
    void write(const bool state) noexcept
    {
        if (myDirection == Direction::Output)
        {
            myState = state;
        }
    }

    /**
     * @brief Read the current GPIO state.
     *
     * @return Current GPIO state.
     */
    [[nodiscard]] bool read() const noexcept { return myState; }

    /**
     * @brief Toggle the GPIO state.
     */
    void toggle() noexcept
    {
        if (myDirection == Direction::Output)
        {
            myState = !myState;
        }
    }

    Gpio()                       = delete; // No default constructor.
    Gpio(const Gpio&)            = delete; // No copy constructor.
    Gpio(Gpio&&)                 = delete; // No move constructor.
    Gpio& operator=(const Gpio&) = delete; // No copy assignment.
    Gpio& operator=(Gpio&&)      = delete; // No move assignment.

private:
    /** GPIO pin number. */
    const std::uint8_t myPin;

    /** GPIO direction. */
    const Direction myDirection;

    /** GPIO state. */
    bool myState;
};
} // namespace driver::gpio
```

---
#### 8. Splitting a class into header and source files
Objects of many different classes are often used in a program. These classes are also often larger than the class we have seen here. It is therefore impractical to implement all classes in a single file.

Typically, a header file dedicated to a specific class is created, including public enumeration classes. For example:
* The class `Gpio` and the enumeration class `Direction` in the namespace `driver::gpio` should be implemented via the header files `gpio.hpp` and `direction.hpp` in the directory `driver/gpio`. The file paths would then be:
    * `driver/gpio/direction.hpp`
    * `driver/gpio/gpio.hpp`

At the top of each header file, we place the directive `#pragma once`, which ensures that we do not accidentally define multiple copies of the contents of the header file if it is included in multiple files, in the same way that header guards work in C. It prevents the same header file from being included multiple times in the same translation unit (a source file together with all included header files):

```cpp
#pragma once
```

We also include `<cstdint>` to gain access to data types such as `std::uint8_t`:

```cpp
#include <cstdint>
```

**File `driver/gpio/direction.hpp`:**  
We implement the enumeration class `Direction` in this file:

```cpp
/**
 * @file GPIO direction configurations.
 */
#pragma once

#include <cstdint>

namespace driver::gpio
{
/**
 * @brief GPIO direction configuration.
 */
enum class Direction : std::uint8_t
{
    Input,       ///< Input without pull-up.
    InputPullup, ///< Input with pull-up enabled.
    Output,      ///< Output.
    Count,       ///< Number of supported directions.
};
} // namespace driver::gpio
```

**File `driver/gpio/gpio.hpp`:**  
We implement the class `Gpio` in this file. Since the enumeration class `Direction` is used directly in this file, `driver/gpio/direction.hpp` is included:

```cpp
/**
 * @file GPIO driver implementation.
 */
#pragma once

#include <cstdint>

#include "driver/gpio/direction.hpp"

namespace driver::gpio
{
/**
 * @brief GPIO driver.
 */
class Gpio final
{
public:
    /**
     * @brief Create a new GPIO object.
     *
     * @param[in] pin GPIO pin number.
     * @param[in] direction GPIO direction.
     * @param[in] initialState Initial GPIO state (default = false).
     */
    explicit Gpio(const std::uint8_t pin, const Direction direction, 
                  const bool initialState = false) noexcept
        : myPin{pin}
        , myDirection{direction}
        , myState{initialState}
    {}

    /**
     * @brief Destructor.
     */
    ~Gpio() noexcept = default;

    /**
     * @brief Write a new state to the GPIO pin.
     *
     * @param[in] state The state to write.
     */
    void write(const bool state) noexcept
    {
        if (myDirection == Direction::Output)
        {
            myState = state;
        }
    }

    /**
     * @brief Read the current GPIO state.
     *
     * @return Current GPIO state.
     */
    [[nodiscard]] bool read() const noexcept { return myState; }

    /**
     * @brief Toggle the GPIO state.
     */
    void toggle() noexcept
    {
        if (myDirection == Direction::Output)
        {
            myState = !myState;
        }
    }

    Gpio()                       = delete; // No default constructor.
    Gpio(const Gpio&)            = delete; // No copy constructor.
    Gpio(Gpio&&)                 = delete; // No move constructor.
    Gpio& operator=(const Gpio&) = delete; // No copy assignment.
    Gpio& operator=(Gpio&&)      = delete; // No move assignment.

private:
    /** GPIO pin number. */
    const std::uint8_t myPin;

    /** GPIO direction. */
    const Direction myDirection;

    /** GPIO state. */
    bool myState;
};
} // namespace driver::gpio
```
**Method definitions in the file `driver/gpio/gpio.cpp`:**  
Class method definitions are often placed in a separate source file to keep the header file more readable. For example, to keep the file `driver/gpio/gpio.hpp` readable, we can place the method definitions in a source file named `driver/gpio/gpio.cpp`.

At the top of this source file, we include the following header files:
* `<cstdint>` to gain access to data types such as `std::uint8_t`.
* `driver/gpio/gpio.hpp` to gain access to the class definition.
* `driver/gpio/direction.hpp` to gain access to the enumeration class `Direction`.

**Note**: Both `<cstdint>` and `driver/gpio/direction.hpp` are included indirectly via `driver/gpio/gpio.hpp`, but to follow good practice we should not rely on indirect dependencies.

```cpp
#include <cstdint>

#include "driver/gpio/direction.hpp"
#include "driver/gpio/gpio.hpp"
```
We then place the method definitions from the class `Gpio` in this file.  
We therefore copy all methods from the class `Gpio` into this file. It is worth noting that:

* For the compiler to understand that each method belongs to the class `Gpio`, we must use the prefix `Gpio`. For example, the method `write()` must be defined as `Gpio::write()`.
* Default values for input arguments should only be written in the method declaration in the header file.
* Keywords placed before method names, such as `explicit`, should not be included in the method definition.
* Attributes such as `[[nodiscard]]` should also only be written on the method declaration in the header file — the definition in the source file does not need to repeat it.
* Constructors, destructors, and similar functions marked with `default` or `delete` do not need to be placed in the source file.

We begin with the method `write()`. In the header file `driver/gpio/gpio.hpp`, we keep the method header together with its documentation:

```cpp
/**
 * @brief Write a new state to the GPIO pin.
 *
 * @param[in] state The state to write.
 */
void write(const bool state) noexcept;
```

In the source file `driver/gpio/gpio.cpp`, we then place the method definition:

```cpp
// -----------------------------------------------------------------------------
void Gpio::write(const bool state) noexcept
{
    if (myDirection == Direction::Output)
    {
        myState = state;
    }
}
```
We implement the definitions for the remaining methods in the source file `driver/gpio/gpio.cpp`. We place a line of hyphens between each method to visually separate them now that the method documentation is not included.

**File `driver/gpio/gpio.cpp`**
After adding all method definitions, the file `driver/gpio/gpio.cpp` looks like this:

```cpp
/**
 * @file GPIO driver implementation details.
 */
#include <cstdint>

#include "driver/gpio/direction.hpp"
#include "driver/gpio/gpio.hpp"

namespace driver::gpio
{
// -----------------------------------------------------------------------------
Gpio::Gpio(const std::uint8_t pin, const Direction direction, const bool initialState) noexcept
    : myPin{pin}
    , myDirection{direction}
    , myState{initialState}
{}

// -----------------------------------------------------------------------------
void Gpio::write(const bool state) noexcept
{
    if (myDirection == Direction::Output)
    {
        myState = state;
    }
}

// -----------------------------------------------------------------------------
bool Gpio::read() const noexcept { return myState; }

// -----------------------------------------------------------------------------
void Gpio::toggle() noexcept
{
    if (myDirection == Direction::Output)
    {
        myState = !myState;
    }
}
} // namespace driver::gpio
```

**File `driver/gpio/gpio.hpp`**  
The readability of the header file `driver/gpio/gpio.hpp` now increases, since we have removed a large part of the implementation details by moving the method definitions:
* However, we must still keep the method declarations and the definitions of the member variables inside the class.
* We remove the keyword `const` from parameters passed *by value* to improve readability; marking these as `const` only has an effect in the function definition.

```cpp
/**
 * @file GPIO driver implementation.
 */
#pragma once

#include <cstdint>

#include "driver/gpio/direction.hpp"

namespace driver::gpio
{
/**
 * @brief GPIO driver.
 */
class Gpio final
{
public:
    /**
     * @brief Create a new GPIO object.
     *
     * @param[in] pin GPIO pin number.
     * @param[in] direction GPIO direction.
     * @param[in] initialState Initial GPIO state (default = false).
     */
    explicit Gpio(std::uint8_t pin, Direction direction, bool initialState = false) noexcept;

    /**
     * @brief Destructor.
     */
    ~Gpio() noexcept = default;

    /**
     * @brief Write a new state to the GPIO pin.
     *
     * @param[in] state The state to write.
     */
    void write(bool state) noexcept;

    /**
     * @brief Read the current GPIO state.
     *
     * @return Current GPIO state.
     */
    [[nodiscard]] bool read() const noexcept;

    /**
     * @brief Toggle the GPIO state.
     */
    void toggle() noexcept;

    Gpio()                       = delete; // No default constructor.
    Gpio(const Gpio&)            = delete; // No copy constructor.
    Gpio(Gpio&&)                 = delete; // No move constructor.
    Gpio& operator=(const Gpio&) = delete; // No copy assignment.
    Gpio& operator=(Gpio&&)      = delete; // No move assignment.

private:
    /** GPIO pin number. */
    const std::uint8_t myPin;

    /** GPIO direction. */
    const Direction myDirection;

    /** GPIO state. */
    bool myState;
};
} // namespace driver::gpio
```

---

### Summary
After going through this appendix, you as the reader should have enough knowledge to create a small class.

You should be able to:
* Control what is visible and not visible outside a class through encapsulation.
* Create constructors to initialize objects of the class in question.
* Understand what `explicit` is used for.
* Understand what `final` is used for.
* Recognize and write a destructor.
* Recognize the copy constructor, copy assignment operator, move constructor, and move assignment operator.
* Use `= default` and `= delete` to control which operations should be allowed.
* Add public methods that control how an object is used.
* Create enumeration classes.
* Place code in a namespace to structure projects and avoid name collisions.

---
