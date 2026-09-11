# Appendix A

## A simple driver factory with raw pointers
This appendix shows a simplified example of how a factory can be used to create drivers in an embedded system.

The purpose is to demonstrate:
* What a factory is.
* Why we use it.
* How it makes the system logic independent of hardware.

---

### Raw pointers and dynamic memory allocation
In this example, so-called raw pointers are used to clarify what happens. Raw pointers are obtained when we use the operators `new` and `delete` to allocate and free objects respectively:
* These operators serve a similar function to `malloc()` and `free()` in C,
but in C++ `new` also calls the object’s constructor and `delete` its destructor.
* Just as when using `malloc()` and `free()`, we are responsible for freeing allocated resources ourselves.
* In modern C++, smart pointers are used instead, which ensure that resources are released via the pointer’s destructor when it goes out of scope, preventing memory leaks.
* Smart pointers will be discussed in [Appendix B](./b_factory_smart_pointers.md), where these raw pointers are replaced with `std::unique_ptr`.

---

### Background
Assume that we have implemented the following drivers as classes:
* GPIO:
    * `driver::gpio::Interface` with the following concrete implementations:
        * `driver::gpio::Esp32s3`: GPIO driver for the ESP32-S3 microcontroller.
        * `driver::gpio::Stub`: GPIO stub driver.

Assume that we have created the following class `system::logic::Logic` to handle the system logic:
* The system logic uses an LED and a button, where the LED is toggled on the rising edge of the push button.
* Interfaces are used so that the hardware used can be selected generically, i.e. the concrete types are chosen by the user.
* The method `run()` is used to execute the system logic continuously.

```cpp
namespace system::logic
{
class Logic final
{
public:
    explicit Logic(driver::gpio::Interface& led,
                   driver::gpio::Interface& button) noexcept
        : myLed{led}
        , myButton{button}
    {}

    ~Logic() noexcept 
    { 
        myLed.write(false); 
    }

    void run() noexcept
    {
        bool buttonPrev{false};

        while (1)
        {
            const bool buttonCurrent{myButton.read()};
            if (buttonCurrent && !buttonPrev) 
            { 
                myLed.toggle(); 
            }
            buttonPrev = buttonCurrent;
        }
    }

    Logic()                        = delete;
    Logic(const Logic&)            = delete;
    Logic(Logic&&)                 = delete;
    Logic& operator=(const Logic&) = delete;
    Logic& operator=(Logic&&)      = delete;

private:
    driver::gpio::Interface& myLed;
    driver::gpio::Interface& myButton;
};
} // namespace system::logic
```

The example above can be implemented by creating drivers of any concrete types before the system logic is created, as shown below:

```cpp
int main()
{
    constexpr std::uint8_t ledPin{2U};
    constexpr std::uint8_t buttonPin{3U};

    // Create ESP32-S3 drivers (ignoring data direction for simplicity).
    driver::gpio::Esp32s3 led{ledPin};
    driver::gpio::Esp32s3 button{buttonPin};

    // Create and run system logic.
    system::logic::Logic logic{led, button};
    logic.run();
}
```

The method above is very efficient when dynamic memory allocation should be limited.

However, it is often desirable to create multiple drivers of the same type, for example drivers for a specific microcontroller. For this reason, it is common to implement classes that create such instances.  
Such a class is called a **factory**, and its sole purpose is to create objects.

Assume that the following factories have been implemented:
* `driver::factory::Interface` with the following concrete implementations:
    * `driver::factory::Esp32s3`: Creates ESP32-S3 instances.
    * `driver::factory::Stub`: Creates stub instances.

Assume that the constructor for `system::logic::Logic` had taken a reference to an arbitrary factory:

```cpp
explicit Logic(driver::factory::Interface& factory, 
               const std::uint8_t ledPin, 
               const std::uint8_t buttonPin) noexcept;
```

Assuming that the member variables had been implemented as pointers instead of references:

```cpp
driver::gpio::Interface* myLed;
driver::gpio::Interface* myButton;
```

and the factory had created raw pointers, the instances could be created as shown below:

```cpp
explicit Logic(driver::factory::Interface& factory, 
               const std::uint8_t ledPin, 
               const std::uint8_t buttonPin) noexcept
        : myLed{factory.gpio(ledPin)}
        , myButton{factory.gpio(buttonPin)}
    {}
```

As an example, `Esp32s3` instances could be created by passing a `driver::factory::Esp32s3` to the system logic when it is created:

```cpp
    constexpr std::uint8_t ledPin{2U};
    constexpr std::uint8_t buttonPin{3U};

    // Create ESP32-S3 factory.
    driver::factory::Esp32s3 factory{};

    // Create and run system logic.
    system::logic::Logic logic{factory, ledPin, buttonPin};
    logic.run();
```

This moves the creation of drivers away from the system logic.  
Instead, the factory is responsible for selecting and creating the correct concrete implementation.  
The system logic can therefore operate on interfaces without knowing the underlying hardware.

---

### Dynamic memory allocation in the factory
When instances are created in this way, they normally need to be allocated dynamically,
unless the factory instead returns a reference to a singleton class.

This can be done using either:
* Raw pointers (`new` and `delete`).
* Smart pointers (`std::make_unique()`).

Instead of allocating an instance of `driver::gpio::Esp32s3` dynamically ourselves:

```cpp
auto* gpio = new driver::gpio::Esp32s3(pin);
```

we let a factory perform this task:

```cpp
auto* gpio = factory.gpio(pin);
```

This means that:
* The system logic does not know which concrete class is used.
* It only knows that it receives an object that satisfies a certain interface.

---

### Why do we use a factory?
In many embedded systems, a factory plays a role similar to a hardware abstraction layer (HAL) or a board support package (BSP); it centralizes the creation of hardware-specific objects.

In an embedded system we often want to be able to:

| Situation | What we want to do |
|----------|--------------------|
| Real hardware | Use real drivers |
| Testing/simulation | Use stubs |
| New platform | Replace drivers |

If the system logic creates drivers itself:

```cpp
auto* led = new driver::gpio::Esp32s3(pin);
```

then:
* The logic is tied to ESP32-S3.
* The system cannot be tested without hardware.

With a factory:

```cpp
auto* led = factory.gpio(pin);
```

we can switch between:

```cpp
driver::factory::Esp32s3 factory{};
```

and:

```cpp
driver::factory::Stub factory{};
```

without changing the system logic.

This is called *dependency injection via factory*.

---

### Overview of the architecture in a typical embedded system
Assume that we have an embedded system where the software is divided into four layers:

```text
system::logic::Logic
        |
        v
driver::factory::Interface
        |
        +------------+
        |            |
        v            v
factory::Esp32s3   factory::Stub
        |            |
        v            v
gpio::Esp32s3      gpio::Stub
```

The system logic therefore does not know whether it is running on real hardware or on stubs.  
It only knows about the interfaces.

---

### Example implementation using a factory

#### Step 1: Interface for GPIO
We create a simple GPIO interface in a file `driver/gpio/interface.hpp`, as shown below:

```cpp
#pragma once

namespace driver::gpio
{
class Interface
{
public:
    virtual ~Interface() noexcept = default;
    [[nodiscard]] virtual bool read() const noexcept = 0;
    virtual void write(bool state) noexcept = 0;
    virtual void toggle() noexcept = 0;
};
} // namespace driver::gpio
```

---

#### Step 2: Real driver
We create a GPIO driver for the ESP32-S3 in a file `driver/gpio/esp32s3.hpp`, as shown below:

```cpp
#pragma once

#include <cstdint>

#include "driver/gpio/interface.hpp"

namespace driver::gpio
{
class Esp32s3 final : public Interface
{
public:
    explicit Esp32s3(const std::uint8_t pin) noexcept
        : myPin{pin}
    {
        // ESP-specific initialization here!
    }

    ~Esp32s3() noexcept override
    {
        // ESP-specific cleanup here!
    }

    [[nodiscard]] bool read() const noexcept override
    {
        // ESP-specific code here!
        return false;
    }

    void write(const bool state) noexcept override
    {
        // ESP-specific code here!
        (void) (state);
    }

    void toggle() noexcept override
    {
        // ESP-specific code here!
    }

    Esp32s3()                          = delete;
    Esp32s3(const Esp32s3&)            = delete;
    Esp32s3(Esp32s3&&)                 = delete;
    Esp32s3& operator=(const Esp32s3&) = delete;
    Esp32s3& operator=(Esp32s3&&)      = delete;

private:
    const std::uint8_t myPin;
};
} // namespace driver::gpio
```

---

#### Step 3: Stub
We create a GPIO stub in a file `driver/gpio/stub.hpp`, as shown below:

```cpp
#pragma once

#include <cstdint>

#include "driver/gpio/interface.hpp"

namespace driver::gpio
{
class Stub final : public Interface
{
public:
    Stub() noexcept
        : myState{false}
    {}
    ~Stub() noexcept override = default;

    [[nodiscard]] bool read() const noexcept override { return myState; }
    void write(const bool state) noexcept override { myState = state; }
    void toggle() noexcept override { myState = !myState; }

    Stub(const Stub&)            = delete;
    Stub(Stub&&)                 = delete;
    Stub& operator=(const Stub&) = delete;
    Stub& operator=(Stub&&)      = delete;

private:
    bool myState;
};
} // namespace driver::gpio
```

---

#### Step 4: Factory interface
We create a factory interface in a file `driver/factory/interface.hpp` in order to implement separate factories:
* A factory for creating instances for ESP32-S3.
* A factory for creating stub instances.

```cpp
#pragma once

#include <cstdint>

namespace driver
{
/** GPIO driver interface. */
namespace gpio { class Interface; }
} // namespace driver

namespace driver::factory
{
class Interface
{
public:
    virtual ~Interface() noexcept = default;
    [[nodiscard]] virtual gpio::Interface* gpio(std::uint8_t pin) noexcept = 0;
};
} // namespace driver::factory
```

Note that `gpio()` is marked `[[nodiscard]]`: it allocates a new object with `new` and hands ownership
to the caller. Discarding the returned pointer would lose the only reference to that object, leaking
it — there would be no way to `delete` it later.

---

#### Step 5: Real factory
We then create an ESP32-S3 factory in a file `driver/factory/esp32s3.hpp` in order to construct ESP32 instances:

```cpp
#pragma once

#include <cstdint>

#include "driver/factory/interface.hpp"
#include "driver/gpio/esp32s3.hpp"

namespace driver::factory
{
class Esp32s3 final : public Interface
{
public:
    Esp32s3() noexcept = default;
    ~Esp32s3() noexcept override = default;

    [[nodiscard]] gpio::Interface* gpio(const std::uint8_t pin) noexcept override 
    { 
        return new gpio::Esp32s3(pin);
    }

    Esp32s3(const Esp32s3&)            = delete;
    Esp32s3(Esp32s3&&)                 = delete;
    Esp32s3& operator=(const Esp32s3&) = delete;
    Esp32s3& operator=(Esp32s3&&)      = delete;
};
} // namespace driver::factory
```

---

#### Step 6: Stub factory
We also create a stub factory in a file `driver/factory/stub.hpp` in order to construct stub instances:

```cpp
#pragma once

#include <cstdint>

#include "driver/factory/interface.hpp"
#include "driver/gpio/stub.hpp"

namespace driver::factory
{
class Stub final : public Interface
{
public:
    Stub() noexcept = default;
    ~Stub() noexcept override = default;

    [[nodiscard]] gpio::Interface* gpio(const std::uint8_t pin) noexcept override
    {
        // Ignore the pin number since it is not used by the stub.
        (void) (pin);
        return new gpio::Stub();
    }

    Stub(const Stub&)            = delete;
    Stub(Stub&&)                 = delete;
    Stub& operator=(const Stub&) = delete;
    Stub& operator=(Stub&&)      = delete;
};
} // namespace driver::factory
```

---

#### Step 7: System logic using a factory
We then create a logic class that uses a given factory to create instances, in a file `system/logic/logic.hpp`:
* In the constructor we allocate memory for the GPIO instances `myLed` and `myButton` via the factory.
* In the destructor we free the allocated resources, i.e. the GPIO instances — since we have allocated memory using raw pointers we are responsible for doing this ourselves.
* In the method `run()`, `myLed` is toggled on the rising edge of the push button.
* Because `myLed` and `myButton` are pointers, the arrow operator `->` is used to call their methods.
* The copy and move operations are deleted. This is particularly important here, because `Logic` owns two raw pointers and deletes them in its destructor. Had we not deleted them, the compiler would have generated a copy constructor that copies the two *pointer values*, so a copy would refer to the exact same two GPIO objects. Both objects would then delete them when destroyed, which means each GPIO object would be deleted twice; a **double deletion**, which is undefined behavior, would occur. The same applies to the copy assignment operator.

**Note:** This is a general rule, and it is sometimes referred to as the *rule of three*: a class that needs a destructor almost always needs its copy operations written or deleted as well. For a class such as `Logic`, which represents one system with one set of drivers, deleting them is the right choice.

In this example, the system logic owns the driver objects. The pointers returned by the factory are therefore owned by `Logic`, which means that the system logic is also responsible for deleting them in the destructor:

```cpp
#pragma once

#include <cstdint>

#include "driver/factory/interface.hpp"
#include "driver/gpio/interface.hpp"

namespace system::logic
{
class Logic final
{
public:
    explicit Logic(driver::factory::Interface& factory,
                   const std::uint8_t ledPin, const std::uint8_t buttonPin) noexcept
        : myLed{factory.gpio(ledPin)}
        , myButton{factory.gpio(buttonPin)}
    {}

    ~Logic() noexcept
    {
        delete myLed;
        delete myButton;
    }

    void run() noexcept
    {
        bool buttonPrev{false};

        while (1)
        {
            // Read the current button input.
            const bool buttonCurrent{myButton->read()};

            // Toggle the LED on pressdown (rising edge).
            if (buttonCurrent && !buttonPrev) 
            { 
                myLed->toggle(); 
            }
            // Store the current button input for next comparison.
            buttonPrev = buttonCurrent;
        }
    }

    // Deleting the copy operations is essential here: a copy would hold the same two pointers,
    // and both objects would delete the same GPIO instances (double deletion).
    Logic()                        = delete; // No default constructor.
    Logic(const Logic&)            = delete; // No copy constructor.
    Logic(Logic&&)                 = delete; // No move constructor.
    Logic& operator=(const Logic&) = delete; // No copy assignment.
    Logic& operator=(Logic&&)      = delete; // No move assignment.

private:
    driver::gpio::Interface* myLed;
    driver::gpio::Interface* myButton;
};
} // namespace system::logic
```

---

#### Step 8: main
In the function main, we create a factory and pass it to the system logic to create the objects.

Below is an example of how a factory could be used to run the system logic on an ESP32-S3 by passing an instance of `driver::factory::Esp32s3` to the system logic, followed by calling the method `run()`:

```cpp
#include <cstdint>

#include "driver/factory/esp32s3.hpp"
#include "system/logic/logic.hpp"

int main()
{
    constexpr std::uint8_t ledPin{2U};
    constexpr std::uint8_t buttonPin{3U};

    // Create system logic and initialize the system.
    driver::factory::Esp32s3 factory{};
    system::logic::Logic logic{factory, ledPin, buttonPin};

    // Run the system continuously.
    logic.run();
    return 0;
}
```

To run with stub drivers, it is sufficient to switch to the stub factory `driver::factory::Stub`, as shown below. This is well suited for testing and will be demonstrated in a later QAcademy course on software testing:

```cpp
#include <cstdint>

#include "driver/factory/stub.hpp"
#include "system/logic/logic.hpp"

int main()
{
    constexpr std::uint8_t ledPin{2U};
    constexpr std::uint8_t buttonPin{3U};

    // Create system logic and initialize the system.
    driver::factory::Stub factory{};
    system::logic::Logic logic{factory, ledPin, buttonPin};

    // Run the system continuously.
    logic.run();
    return 0;
}
```

---
