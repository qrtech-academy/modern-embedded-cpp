# Appendix C

## Exercises
These exercises reinforce the concepts from [Appendix A](a_classes1.md) and [Appendix B](./b_classes2.md).

---

# Exercise Set 1 - First class

## Exercise 1.1 - Simple LED class
In a new program, create a class `Led` in the namespace `driver::gpio`:

```cpp
namespace driver::gpio
{
class Led final
{
public:

private:
};
} // namespace driver::gpio
```

### Tasks
**a)** Add two private member variables:
* The first member variable shall:
    * Be named `myPin`.
    * Represent the pin number to which the LED is connected.
    * Have the type `std::uint8_t`.
    * Be readable only after initialization.
* The second member variable shall:
    * Be named `myState`.
    * Represent the state of the LED (`true/false`).
    * Be `true` when the LED is enabled.

**b)** Add a constructor that:
* Takes a pin number (`std::uint8_t`).
* Takes an initial state (`bool`) with the default value `false`.
* Uses an initialization list to initialize the member variables.
* Is marked `explicit` and `noexcept`.

**c)** Add a destructor and mark it `noexcept` and `default`.

**d)** Implement the following methods directly in the class:
* The method `isOn()` shall:
    * Return `true` if the LED is enabled, otherwise `false`.
    * Be marked `const`, `noexcept`, and `[[nodiscard]]`.
* The method `on()` shall:
    * Take no parameters.
    * Set the LED state to `true`.
    * Return no value.
    * Be marked `noexcept`.
* The method `off()` shall:
    * Take no parameters.
    * Set the LED state to `false`.
    * Return no value.
    * Be marked `noexcept`.
* The method `toggle()` shall:
    * Toggle the LED state.
    * Take no parameters.
    * Return no value.
    * Be marked `noexcept`.

**e)** Delete the following methods:
* The default constructor.
* The copy constructor.
* The move constructor.
* The copy assignment operator.
* The move assignment operator.

**f)** Create an instance of the class:

```cpp
constexpr std::uint8_t ledPin{13U};
driver::gpio::Led led{ledPin};
```

Test the class by:
* Printing the state.
* Calling `on()`.
* Printing again.
* Calling `toggle()`.
* Printing once more.

Use `std::printf()` from `<cstdio>` for the output.

Example output:

```text
Initial state: Off
After on(): On
After toggle(): Off
```

---

## Exercise 1.2 - Button class
In the same program, create a class `Button` in the namespace `driver::gpio`:

```cpp
namespace driver::gpio
{
class Button final
{
public:

private:
};
} // namespace driver::gpio
```

### Tasks
**a)** Add two private member variables:
* The first member variable shall:
    * Be named `myPin`.
    * Represent the pin number to which the button is connected.
    * Have the type `std::uint8_t`.
    * Be readable only after initialization.
* The second member variable shall:
    * Be named `myPressed`.
    * Represent the state of the button (`true/false`).
    * Be `true` when the button is pressed.

**b)** Add a constructor that:
* Takes a pin number (`std::uint8_t`).
* Uses an initialization list to initialize the member variables.
* Is marked `explicit` and `noexcept`.

**c)** Add a destructor and mark it `noexcept` and `default`.

**d)** Implement the following methods directly in the class:
* The method `pin()` shall:
    * Return the pin number.
    * Be marked `const`, `noexcept`, and `[[nodiscard]]`.
* The method `isPressed()` shall:
    * Return `true` if the button is pressed, otherwise `false`.
    * Be marked `const`, `noexcept`, and `[[nodiscard]]`.
* The method `setPressed()` shall:
    * Take an argument of type `bool`.
    * Update the internal state of the button.
    * Return no value.
    * Be marked `noexcept`.

**e)** Delete the following methods:
* The default constructor.
* The copy constructor.
* The move constructor.
* The copy assignment operator.
* The move assignment operator.

**f)** Create an instance:

```cpp
constexpr std::uint8_t buttonPin{2U};
driver::gpio::Button button{buttonPin};
```

Simulate a button press:

```cpp
button.setPressed(true);
```

Print the pin number and whether the button is pressed using `std::printf()` from `<cstdio>`.

Example output:

```text
Button pin: 2
Pressed: Yes
```

---
# Exercise Set 2 - Interaction between classes

## Exercise 2.1 - Control LED with a button
Use the classes from the previous exercises.

### Tasks
**a)** Create an LED on pin `13`.  
**b)** Create a button on pin `2`.  
**c)** If the button is not pressed, the LED should be off.  
**d)** If the button is pressed, the LED should turn on.  
**e)** Print the LED state.  

Example output:

```text
Button released -> LED: Off
Button pressed  -> LED: On
```

---

## Exercise 2.2 - Blink function
Create a loop that toggles the LED state.

### Tasks
**a)** Turn the LED off, then run a loop for six iterations.  
**b)** In each iteration:
* Call `toggle()`.
* Print the LED state.

Example output:

```text
Blink 1: On
Blink 2: Off
Blink 3: On
Blink 4: Off
Blink 5: On
Blink 6: Off
```

---

# Exercise Set 3 - Class split across multiple files

## Exercise 3.1 - Buzzer class
In this exercise, you will create a class `Buzzer` in the namespace `driver`.  
The class should be split across multiple files. Therefore, create the following directory structure:

```text
Makefile
include/
    driver/
        buzzer.hpp
source/
    driver/
        buzzer.cpp
    main.cpp
```

**Note!**
* All method declarations shall be written in `driver/buzzer.hpp`.
* All method definitions shall be implemented in `driver/buzzer.cpp`.

---

In the header file `driver/buzzer.hpp`, the class shall be declared according to the structure below:

```cpp
#pragma once

#include <cstdint>

namespace driver
{
class Buzzer final
{
public:

private:
};
} // namespace driver
```
### Tasks
**a)** Add two private member variables:
* The first member variable shall:
    * Represent the pin number to which the buzzer is connected.
    * Have the type `std::uint8_t`.
    * Be named `myPin`.
    * Be readable only after initialization.
* The second member variable shall:
    * Represent the state of the buzzer (`true`/`false`).
    * Be named `myEnabled`.
    * Be `true` when the buzzer is enabled.

**b)** Add a constructor in the header file that:
* Takes a pin number (`std::uint8_t`).
* Takes an initial state (`bool`) with the default value `false`.
* Is marked `explicit` and `noexcept`.

**c)** Add a destructor in the header file:
* Mark it `noexcept`.
* Implement it in `driver/buzzer.cpp` so that the output below is generated when the buzzer is destroyed:

```text
Releasing resources allocated for buzzer at pin 8!
```

**Note!** In the example output above, it is assumed that the buzzer is connected to pin 8.

**d)** Declare the following methods in the header file and implement them in `driver/buzzer.cpp`:
* The method `pin()` shall:
    * Return the pin number to which the buzzer is connected.
    * Be marked `const`, `noexcept`, and `[[nodiscard]]`.
* The method `isEnabled()` shall:
    * Return `true` if the buzzer is enabled, otherwise `false`.
    * Take no parameters.
    * Be marked `const`, `noexcept`, and `[[nodiscard]]`.
* The method `enable()` shall:
    * Set the buzzer state to `true`.
    * Take no parameters.
    * Return no value.
    * Be marked `noexcept`.
* The method `disable()` shall:
    * Set the buzzer state to `false`.
    * Take no parameters.
    * Return no value.
    * Be marked `noexcept`.
* The method `toggle()` shall:
    * Toggle the buzzer state.
    * Take no parameters.
    * Return no value.
    * Be marked `noexcept`.

**e)** Delete the following methods in the header file:
* The default constructor.
* The copy constructor.
* The move constructor.
* The copy assignment operator.
* The move assignment operator.

**f)** In `main.cpp`, create an instance of the class:


```cpp
constexpr std::uint8_t buzzerPin{8U};
driver::Buzzer buzzer{buzzerPin};
```

Test the class by:
* Printing the state.
* Calling `enable()`.
* Printing again.
* Calling `toggle()`.
* Printing again.

Use `std::printf()` from `<cstdio>` for the output.

Example output:

```text
Initial state: Disabled
After enable(): Enabled
After toggle(): Disabled
Releasing resources allocated for buzzer at pin 8!
```

---

# Exercise Set 4 - Timer class

## Exercise 4.1 - Timer
In this exercise, you will create a class `Timer` in the namespace `driver`.

The class should be split across multiple files. Therefore, create the following files in the same project as the previous exercise:

```text
include/driver/timer.hpp
source/driver/timer.cpp
```

**Note!**
* All method declarations shall be written in `driver/timer.hpp`.
* All method definitions shall be implemented in `driver/timer.cpp`.

In the header file, the class shall be declared according to the structure below:

```cpp
#pragma once

#include <cstdint>

namespace driver
{
class Timer final
{
public:

private:
};
} // namespace driver
```

### Tasks
**a)** Add four private member variables:
* The first member variable shall:
    * Be named `myTimeout_ms`.
    * Represent the timeout duration in milliseconds.
    * Have the type `std::uint32_t`.
    * Be readable only after initialization.
* The second member variable shall:
    * Be named `myCounter_ms`.
    * Represent the internal timer counter.
    * Have the type `std::uint32_t`.
* The third member variable shall:
    * Be named `myRunning`.
    * Represent whether the timer is active or not (`true/false`).
    * Be `true` when the timer is running.
* The fourth member variable shall:
    * Be named `myInitialized`.
    * Represent whether the timer is initialized or not (`true/false`).
    * Be `true` when the timer is initialized.

**b)** Add a constructor in the header file that:
* Takes a timeout value in milliseconds (`std::uint32_t`).
* Uses an initialization list to initialize the member variables.
* Initializes the counter to `0`.
* Initializes the timer as stopped.
* Marks the timer as initialized if the timeout value is valid (`> 0`).
* Is marked `explicit` and `noexcept`.

**c)** Add a destructor in the header file:
* Mark it `noexcept`.
* Implement it in `driver/timer.cpp` so that:
    * The timer is stopped if it was running.
    * If the timer was running and is stopped, the output below should be printed:

```text
Stopping timer before deletion!
```

**d)** Declare the following methods in the header file and implement them in `driver/timer.cpp`:
* The method `timeout_ms()` shall:
    * Return the configured timeout in milliseconds.
    * Take no parameters.
    * Be marked `const`, `noexcept`, and `[[nodiscard]]`.
* The method `isRunning()` shall:
    * Return `true` if the timer is running, otherwise `false`.
    * Take no parameters.
    * Be marked `const`, `noexcept`, and `[[nodiscard]]`.
* The method `isInitialized()` shall:
    * Return `true` if the timer is initialized, otherwise `false`.
    * Take no parameters.
    * Be marked `const`, `noexcept`, and `[[nodiscard]]`.
* The method `start()` shall:
    * Start the timer if the timer is initialized.
    * Take no parameters.
    * Return no value.
    * Be marked `noexcept`.
* The method `stop()` shall:
    * Stop the timer if the timer is initialized.
    * Take no parameters.
    * Return no value.
    * Be marked `noexcept`.
* The method `toggle()` shall:
    * Toggle the timer if the timer is initialized.
    * Take no parameters.
    * Return no value.
    * Be marked `noexcept`.
* The method `tick()` shall:
    * Simulate that 1 millisecond has passed.
    * Increment the counter if the timer is running; otherwise do nothing.
    * Return no value.
    * Be marked `noexcept`.
* The method `hasTimedOut()` shall:
    * Return `true` when the counter has reached the timeout value, otherwise `false`.
    * Reset the counter to `0` when a timeout occurs.
    * Be marked `noexcept` and `[[nodiscard]]`.

**e)** Delete the following methods:
* The default constructor.
* The copy constructor.
* The move constructor.
* The copy assignment operator.
* The move assignment operator.

**f)** In `main.cpp`, create an instance of the class:

```cpp
constexpr std::uint32_t timeout_ms{1000U};
driver::Timer timer{timeout_ms};
```
Test the class by:
* Starting the timer.
* Running a loop for 3000 iterations:
    * In each iteration:
        * Call `tick()`.
        * Check `hasTimedOut()`.
        * Print a message if a timeout has occurred:

```text
Timeout after 1000 ms!
```

**Note!** In the example output above, it is assumed that the timeout is set to 1000 milliseconds.

Use `std::printf()` from `<cstdio>` for the output above.

Example output:

```text
Timeout after 1000 ms!
Timeout after 1000 ms!
Timeout after 1000 ms!
Stopping timer before deletion!
```

---
