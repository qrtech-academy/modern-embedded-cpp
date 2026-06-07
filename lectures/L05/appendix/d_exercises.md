# Appendix D

## Exercises

## Exercise Set 1 – Function Templates

### Exercise 1.1 – Clear Bit
Create a function template to clear a bit in a register:

```cpp
/**
 * @brief Clear bit in a register.
 *
 * @tparam T Register type. Must be integral.
 *
 * @param[out] reg Destination register.
 * @param[in] bit Bit to clear.
 */
template<typename T>
constexpr void clear(T& reg, const std::uint8_t bit) noexcept;
```

#### Tasks:
**a)** Use `static_assert()` with `std::is_integral<T>::value` and the following error message:  

```text
Cannot perform bit operation with non-integral type!
```

**b)** Clear the selected bit.  
**c)** Try the `clear()` function with the following code:

```cpp
#include <bitset>
#include <cstdint>
#include <iostream>

int main()
{
    std::uint8_t reg{0xFFU};

    // Clear bit 2 => reg becomes 0b11111011.
    clear(reg, 2U);
    std::cout << "Register content after clearing bit 2: 0b" << std::bitset<8U>(reg) << "\n";
    return 0;
}
```

Expected output:

```text
Register content: 0b11111011
```

---

### Exercise 1.2 – Toggle Bits
Create the following function template to toggle one or several bits in a register:

```cpp
/**
 * @brief Toggle bits in a register.
 *
 * @tparam T Register type. Must be integral.
 * @tparam Bits Parameter pack of bits.
 *
 * @param[out] reg Destination register.
 * @param[in] bits Bits to toggle.
 */
template<typename T, typename... Bits>
constexpr void toggle(T& reg, const Bits... bits) noexcept;
```

Tasks:
**a)** Use a parameter pack.    
**b)** Ensure that `T` is an integral type using `static_assert()`.  
**c)** Iterate over the bits (for example using `{bits...}`).  
**d)** Toggle the specified bits.  
**e)** Try the `toggle()` function with the following code snippet:

```cpp
#include <bitset>
#include <cstdint>
#include <iostream>

int main()
{
    std::uint8_t reg{0xFFU};

    // Clear bit 2 => reg becomes 0b11111011.
    clear(reg, 2U);
    std::cout << "Register content after clearing bit 2: 0b" << std::bitset<8U>(reg) << "\n";

    // Toggle bits 0-3 => reg becomes 0b11110100.
    toggle(reg, 0U, 1U, 2U, 3U);
    std::cout << "Register content after toggling bits 0-3: 0b" << std::bitset<8U>(reg) << "\n";
    return 0;
}
```

Expected output:

```text
Register content after clearing bit 2: 0b11111011
Register content after toggling bits 0-3: 0b11110100
```

---

## Exercise Set 2 – Function Template Concepts

### Exercise 2.1 – Template Constraints
Tasks:
**a)** Explain why `static_assert()` is useful.  
**b)** What happens if the constraint fails?  
**c)** Why use compile-time checks instead of runtime?  

---

## Exercise Set 3 – Class Template Specialization

### Exercise 3.1 – Timer Driver Template
In this exercise, you will create a timer driver that supports multiple implementations using template specialization in a file `driver/timer.h`.

The goal is to select the timer implementation at compile time based on a template parameter.

The timer type shall be selected using the following enumeration class:

```cpp
namespace driver::timer
{
/**
 * @brief Enumeration of supported timers.
 */
enum class Type : std::uint8_t
{
    Stm32, ///< STM32 timer.
    Stub,  ///< Stub timer.
};
} // namespace driver::timer
```

The timer shall be declared according to the following structure:

```cpp
namespace driver::timer
{
/**
 * @brief Timer implementation.
 *
 * @tparam T Timer type.
 */
template<Type T>
class Timer final;

} // namespace driver::timer
```

The primary template shall represent a stub timer implementation. A specialization shall later be created for `Type::Stm32`.

---

#### Part I – Stub Timer

**a)** Implement the primary template as a stub timer:

```cpp
/**
 * @brief Timer implementation.
 *
 * @tparam T Timer type (default = stub timer).
 */
template<Type T = Type::Stub>
class Timer final
{
};
```

---

**b)** Add four private member variables:
* The first member variable shall:
   * Be named `myTimeout_ms`.
   * Represent the timeout duration in milliseconds.
   * Have the type `std::uint16_t`.
   * Be readable only after initialization.
* The second member variable shall:
   * Be named `myCounter_ms`.
   * Represent the internal timer counter.
   * Have the type `std::uint16_t`.
* The third member variable shall:
   * Be named `myRunning`.
   * Represent whether the timer is active or not (`true/false`).
   * Be `true` when the timer is running.
* The fourth member variable shall:
   * Be named `myInitialized`.
   * Represent whether the timer is initialized or not (`true/false`).
   * Be `true` when the timer is initialized.

---

**c)** Add a constructor that:
* Takes a timeout value in milliseconds (`std::uint16_t`).
* Uses an initialization list to initialize the member variables.
* Initializes the counter to `0`.
* Initializes the timer as stopped.
* Marks the timer as initialized if the timeout value is valid (`> 0`).
* Is marked `explicit` and `noexcept`.
* If the timeout is successful, prints:

```text
Created stub timer with timeout 1000 ms!
```

where `1000` shall be replaced by the configured timeout value.

* If the timeout is 0, prints:

```text
Failed to initialize stub timer: invalid timeout 0 ms!
```

---

**d)** Add a destructor that:
* Is marked `noexcept`.
* Stops the timer if it is currently running.
* Prints `Destroying stub timer!`.

---

**e)** Define the following methods:
* The method `timeout_ms()` shall:
   * Return the configured timeout in milliseconds.
   * Take no parameters.
   * Be marked `const` and `noexcept`.
* The method `isRunning()` shall:
   * Return `true` if the timer is running; otherwise `false`.
   * Take no parameters.
   * Be marked `const` and `noexcept`.
* The method `isInitialized()` shall:
   * Return `true` if the timer is initialized; otherwise `false`.
   * Take no parameters.
   * Be marked `const` and `noexcept`.
* The method `start()` shall:
   * Start the timer if the timer is initialized.
   * Print `Starting stub timer!`.
   * Take no parameters.
   * Return no value.
   * Be marked `noexcept`.
* The method `stop()` shall:
   * Stop the timer if the timer is initialized.
   * Print `Stopping stub timer!`.
   * Take no parameters.
   * Return no value.
   * Be marked `noexcept`.
* The method `toggle()` shall:
   * Toggle the timer if the timer is initialized.
   * Print `Toggling stub timer!`.
   * Take no parameters.
   * Return no value.
   * Be marked `noexcept`.
* The method `tick()` shall:
   * Simulate that 1 millisecond has passed.
   * Increment the counter if the timer is running; otherwise do nothing.
   * Return no value.
   * Be marked `noexcept`.
* The method `hasTimedOut()` shall:
   * Return `true` when the counter has reached the timeout value; otherwise `false`.
   * Reset the counter to `0` when a timeout occurs.
   * Be marked `noexcept`.

---

**f)** Delete the following methods:
* The default constructor.
* The copy constructor.
* The move constructor.
* The copy assignment operator.
* The move assignment operator.

---

#### Part II – STM32 Timer

**a)** Create a specialization for microcontroller `STM32`:

```cpp
/**
 * @brief STM32 timer implementation.
 */
template<>
class Timer<Type::Stm32> final
{
};
```

---

**b)** Implement the STM32 Timer with same public interface as the primary template:
* Implement the same methods, but adjust the prints, for instance:

```text
Starting STM32 timer!
Stopping STM32 timer!
Destroying STM32 timer!
```

* Delete the default constructor as was done for the stub implementation.
* Prohibit copy and move semantics as was done for the stub implementation.

---

#### Part III – Testing the Timers
Test the timers using the following test program in `main.cpp`:

```cpp
#include <chrono>
#include <cstdint>
#include <thread>

#include "driver/timer/timer.h"

using namespace driver;

namespace
{
// -----------------------------------------------------------------------------
template<timer::Type T>
constexpr const char* timerType() noexcept
{
    if constexpr (T == timer::Type::Stub) { return "Stub"; }
    else if (T == timer::Type::Stm32) { return "STM32"; }
    return "Unknown";
}

// -----------------------------------------------------------------------------
template<timer::Type T>
void tickTimer(timer::Timer<T>& timer) noexcept
{
    constexpr const char* type{timerType<T>()};
    timer.tick();

    if (timer.hasTimedOut())
    {
        std::printf("%s timer has timed out after %u ms!\n", type, timer.timeout_ms());
    }
}

// -----------------------------------------------------------------------------
void delay_ms(const std::uint16_t ms) noexcept
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
} // namespace

// -----------------------------------------------------------------------------
int main()
{
    timer::Timer timer1{500U};
    timer::Timer<timer::Type::Stm32> timer2{1500U};
    timer1.start();
    timer2.start();

    constexpr std::uint16_t iterationCount{2000U};

    for (std::uint16_t i{}; i < iterationCount; ++i)
    {
        tickTimer(timer1);
        tickTimer(timer2);
        delay_ms(1U);
    }
    return 0;
}
```

Expected output:

```text
Created stub timer with timeout 500 ms!
Created STM32 timer with timeout 1500 ms!
Starting stub timer!
Starting STM32 timer!
Stub timer has timed out after 500 ms!
Stub timer has timed out after 500 ms!
Stub timer has timed out after 500 ms!
STM32 timer has timed out after 1500 ms!
Stub timer has timed out after 500 ms!
Stopping STM32 timer!
Destroying STM32 timer!
Stopping stub timer!
Destroying stub timer!
```

---

### Exercise 3.2 – Reflection
**a)** Why is the timer type selected at compile time?  
**b)** What is a template specialization?  
**c)** What advantages does this design provide compared to using inheritance and virtual functions?  
**d)** When would inheritance be a better choice?  
**e)** What happens if the specialization for `Type::Stm32` is removed and a `Timer<Type::Stm32>` instance is created?  
**f)** Why might a template specialization be preferred over a virtual interface in a small microcontroller system?  

---
