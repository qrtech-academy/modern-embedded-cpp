# Appendix B

## Driver factory with smart pointers (Abstract Factory)
In this appendix we further develop the factory example from 
[Appendix A](./a_factory_raw_pointers.md).

We replace raw pointers with smart pointers and use a simplified form of the **Abstract Factory pattern**.

The purpose is to:
* Avoid manual memory management.
* Clarify ownership.
* Make the system safer and more robust.

---

### What is Abstract Factory?
**Abstract Factory** is a design pattern where:
* A common factory interface defines how objects are created.
* Multiple concrete factories implement this interface.
* Each factory creates drivers for a specific platform or environment.

In our case:

| Factory | What it creates |
|--------|----------------|
| `Esp32s3` | Real hardware drivers (for ESP32-S3) |
| `Stub` | Simulated drivers for testing |

The system logic only uses the factory interface and does not know which concrete factory is used.

---

### Why do we switch to smart pointers?
In the previous example with raw pointers, the factory created objects using the `new` operator:

```cpp
return new gpio::Esp32s3(pin);
```

The system logic had to delete the objects manually using the `delete` operator:

```cpp
delete myLed;
```

Using raw pointers introduces several problems:
* It is easy to forget to call `delete`, which leads to memory leaks.
* Ownership of the object is unclear.
* Manual memory management makes programs harder to write correctly.

A modern C++ solution is to use `std::unique_ptr` from the `<memory>` header instead of raw pointers.

A `std::unique_ptr`:
* Represents exclusive ownership.
* Deletes the object automatically.
* Cannot be copied, only moved.

The `std::unique_ptr` returned by the factory transfers ownership of the created object to the caller.

When the `std::unique_ptr` goes out of scope, the object is automatically destroyed.

If we use smart pointers, the factory can instead create objects by calling the `std::make_unique()` function from `<memory>`:

```cpp
return std::make_unique<gpio::Esp32s3>(pin);
```

`std::make_unique()`:
* Creates the object.
* Returns a `std::unique_ptr`.
* Ensures safe and clear construction.

---

### Exceptions and `noexcept`
In C++, memory allocation can fail. When we use `new` or `std::make_unique()`, an exception (`std::bad_alloc`) may be thrown if the system cannot allocate memory.

In embedded systems, exceptions are often not used, or a predictable behavior is preferred when errors occur. Therefore `noexcept` is still often used in driver and factory interfaces.

Example:

```cpp
virtual std::unique_ptr<gpio::Interface> gpio(std::uint8_t pin) noexcept = 0;
```

If a function marked with `noexcept` nevertheless throws an exception, the program terminates immediately.

This provides clear and predictable error behavior. In many embedded systems this is desirable because:
* Running out of memory is often a critical error.
* The system cannot safely continue anyway.

For this reason, `noexcept` is used in our driver and factory interfaces. If a memory error still occurs, the program will terminate, providing clear and predictable error behavior.

---

### Example implementation using Abstract Factory
Here we use a simplified form of the Abstract Factory pattern.

#### Step 1: Factory interface

```cpp
#pragma once

#include <cstdint>
#include <memory>

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

    [[nodiscard]] virtual std::unique_ptr<gpio::Interface> gpio(std::uint8_t pin) noexcept = 0;
};
} // namespace driver::factory
```

The factory now returns a `std::unique_ptr` instead of a raw pointer.

Note that `gpio()` is marked `[[nodiscard]]`: discarding the returned `std::unique_ptr` would destroy
the newly created object immediately, since nothing would be left holding onto it. This is exactly
the "factory function returning an owning resource" case mentioned back in L01.

---

### Step 2: Real factory (for ESP32-S3)

```cpp
#pragma once

#include <memory>

#include "driver/factory/interface.hpp"
#include "driver/gpio/esp32s3.hpp"

namespace driver::factory
{
class Esp32s3 final : public Interface
{
public:
    Esp32s3() noexcept = default;
    ~Esp32s3() noexcept override = default;

    [[nodiscard]] std::unique_ptr<gpio::Interface> gpio(std::uint8_t pin) noexcept override
    {
        return std::make_unique<gpio::Esp32s3>(pin);
    }

    Esp32s3(const Esp32s3&)            = delete;
    Esp32s3(Esp32s3&&)                 = delete;
    Esp32s3& operator=(const Esp32s3&) = delete;
    Esp32s3& operator=(Esp32s3&&)      = delete;
};
} // namespace driver::factory
```

---

#### Step 3: Stub factory

```cpp
#pragma once

#include <cstdint>
#include <memory>

#include "driver/factory/interface.hpp"
#include "driver/gpio/stub.hpp"

namespace driver::factory
{
class Stub final : public Interface
{
public:
    Stub() noexcept = default;
    ~Stub() noexcept override = default;

    [[nodiscard]] std::unique_ptr<gpio::Interface> gpio(std::uint8_t pin) noexcept override
    {
        (void)(pin);
        return std::make_unique<gpio::Stub>();
    }

    Stub(const Stub&)            = delete;
    Stub(Stub&&)                 = delete;
    Stub& operator=(const Stub&) = delete;
    Stub& operator=(Stub&&)      = delete;
};
} // namespace driver::factory
```

---

#### Step 4: System logic with `unique_ptr`

```cpp
#pragma once

#include <cstdint>
#include <memory>

#include "driver/factory/interface.hpp"

namespace system::logic
{
class Logic final
{
public:
    explicit Logic(driver::factory::Interface& factory,
                   std::uint8_t ledPin,
                   std::uint8_t buttonPin) noexcept 
        : myLed{factory.gpio(ledPin)}
        , myButton{factory.gpio(buttonPin)}
    {}

    void run() noexcept
    {
        bool buttonPrev{false};

        while (1)
        {
            // Read the current button input.
            const bool buttonCurrent{myButton->read()};

            // Toggle the LED on button press (rising edge).
            if (buttonCurrent && !buttonPrev) 
            { 
                myLed->toggle(); 
            }
            // Store the current button input for next comparison.
            buttonPrev = buttonCurrent;
        }
    }

    Logic()                        = delete; // No default constructor.
    Logic(const Logic&)            = delete; // No copy constructor.
    Logic(Logic&&)                 = delete; // No move constructor.
    Logic& operator=(const Logic&) = delete; // No copy assignment.
    Logic& operator=(Logic&&)      = delete; // No move assignment.

private:
    std::unique_ptr<driver::gpio::Interface> myLed;
    std::unique_ptr<driver::gpio::Interface> myButton;
};
} // namespace system::logic
```

Note:
* No destructor is needed.
* The memory is released automatically.

---

#### Step 5: main
Just as in the factory example with raw pointers, we create a factory in the `main` function and 
then pass it to the system logic to create objects.

Below is an example of how a factory can be used to run the system logic on ESP32-S3 by
passing an instance of `driver::factory::Esp32s3` to the system logic and then calling `run()`:

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

As shown in the example factory with raw pointers, it is sufficient to switch to the stub factory `driver::factory::Stub` in order to run with stub drivers:

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

### Summary
In this version we use:
* **Interfaces** for all drivers.
* **Abstract Factory** to create the correct type of drivers.
* **`std::unique_ptr`** for safe ownership.

Advantages:
* No manual `delete`.
* No memory leaks.
* Clear ownership.
* The same system logic works:
  * On real hardware.
  * In simulation.

---
