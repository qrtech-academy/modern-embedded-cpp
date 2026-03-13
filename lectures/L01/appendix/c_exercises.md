
# Appendix C

## Exercises
These exercises practice concepts from [Appendix B](./b_from_c_to_cpp.md).

---

## Exercise Set 1 – Default Arguments and Functions

### Exercise 1.1 – Debug Logger
Create a namespace called `debug` containing a function:

```cpp
void log(const char* message, std::uint8_t level = 0U);
```

Tasks:
1. Implement the function so it prints the message using `std::printf` from `<cstdio>`.
2. Include the log level in the output.
3. Mark the function `noexcept`.

Example usage:

```cpp
debug::log("System started");
debug::log("Sensor failure", 2U);
```

Example output:

```text
System started, log level = 0
Sensor failure, log level = 2
```

---

### Exercise 1.2 – System Delay
In the same program as in Exercise 1.1, create a function that simulates a simple software delay:

```cpp
void delay_ms(std::uint32_t ms = 1U);
```

Tasks:
1. Place the function inside namespace `system`.   
2. Mark the function `noexcept`.  
3. Inside the function, define a `constexpr` constant named `maxCount` and assign it the value `10000000UL`.  
4. Implement a delay using nested loops:
    * The outer loop should iterate `ms` times.
    * The inner loop should iterate up to `maxCount`.
    * To reduce the risk of the compiler optimizing the loop away, add a volatile dummy variable `volatile std::uint32_t dummy{}` inside the function and increment it inside the inner loop.
5. Use this function to simulate a software delay of `100 ms` between the debug prints in exercise 1.1.

---

## Exercise Set 2 – Struct Driver

### Exercise 2.1 – Software Timer
In this exercise you will implement a simple software timer driver `driver::Timer` in a header file `driver/timer.h`.

Add the following lines at the top of the file:

```cpp
/**
 * @brief Timer driver implementation.
 */
#pragma once

#include <cstdint>
#include <cstdio>
```

The driver shall simulate a timer that counts milliseconds and generates a timeout when a configured timeout value is reached.

The struct shall:
* Use private member variables with the prefix `my`.
* Have a constructor.
* Have a destructor.

Use `std::printf` from `<cstdio>` for terminal prints.

---

### a) Private member variables
Add three private member variables:
* The first member variable shall:
  * Store the timeout in milliseconds.
  * Have the type `const std::uint16_t`.
  * Be named `myTimeout_ms`.

* The second member variable shall:
  * Store the internal counter in milliseconds.
  * Have the type `std::uint16_t`.
  * Be named `myCounter_ms`.

* The third member variable shall:
  * Indicate whether the timer is running.
  * Have the type `bool`.
  * Be named `myRunning`.

### b) Constructor
Add a constructor that:
* Takes two input arguments:
  * The timeout in milliseconds.
  * The initial running state, which shall be set to `false` by default.
* Initializes:
  * `myTimeout_ms` with the provided timeout value.
  * `myCounter_ms` with `0U`.
  * `myRunning` with `false`.
* Prints that the timer is being created.
* Calls the public method `start()` if the initial running state is `true`.

Example:

```text
Creating timer!
```

### c) Destructor
Add a destructor that:
* Calls the public method `stop()`.
* Prints that the timer is being destroyed.

Example:

```text
Destroying timer!
```

### d) Public methods
Add the following public methods:
* `timeout_ms()` shall:
    * Return `myTimeout_ms`.
* `isRunning()` shall:
    * Return `myRunning`.
* `start()` shall:
  * Set `myRunning` to `true`.
  * Print `Starting timer!`.
* `stop()` shall:
  * Set `myRunning` to `false`.
  * Print `Stopping timer!`.
* `toggle()` shall:
  * Toggle `myRunning`.
  * Print that the timer was toggled and whether it is now running or stopped:
    * `Toggling timer: running!` when enabled.
    * `Toggling timer: stopped!` when disabled.
* `tick()` shall:
  * Increment `myCounter_ms` if `myRunning` is `true`.
  * Otherwise do nothing.
* `timeout()` shall:
  * Return `true` if `myCounter_ms >= myTimeout_ms`, otherwise `false`.
  * If `myCounter_ms >= myTimeout_ms`:
    * `myCounter_ms` shall be reset to `0U`.

### e) Create and use a timer instance
In `main()`:
* Create a timer instance:
  * Name the instance `timer`.
  * Set the timeout to `1000 ms`.
  * Set the timer to running at startup.
* Create a loop that runs for `5000` iterations:
  * Call `tick()` each iteration.
  * Call `timeout()` to check whether the timer has expired.
  * Print `Timeout after x ms!`, where `x` is the configured timeout, whenever the timer times out.

Since the timer timeout is `1000 ms` and the loop runs for `5000` iterations, the timer should generate five timeouts.

Example output:

```text
Creating timer!
Starting timer!
Timeout after 1000 ms!
Timeout after 1000 ms!
Timeout after 1000 ms!
Timeout after 1000 ms!
Timeout after 1000 ms!
Stopping timer!
Destroying timer!
```

---

## Exercise Set 3 – References

### Exercise 3.1 – Assign value
In a new program, implement the following function in an anonymous namespace:

```cpp
constexpr void assign(std::uint8_t& byte) noexcept;
```

Tasks:
1. Assign the value `0xFFU` to `byte`.
2. Test the function using:

```cpp
std::uint8_t num{};
assign(num);
std::printf("num = %u\n", static_cast<unsigned>(num));
```

Expected output:

```text
num = 255
```

---

### Exercise 3.2 – Swap Values
Implement the following function in the same anonymous namespace as in Exercise 3.1:

```cpp
constexpr void swap(std::uint32_t& a, std::uint32_t& b) noexcept;
```

Tasks:
1. Swap the values using a temporary variable `temp`.
2. Test the function with two variables.

Example output:

```text
Before swap: a = 3, b = 10
After swap: a = 10, b = 3
```

---

## Exercise Set 4 – Bit Manipulation Templates

### Exercise 4.1 – Clear Bit
In a new file, create a function template inside an anonymous namespace that clears a bit in a register:

```cpp
template<typename T>
constexpr void clear(T& reg, std::uint8_t bit) noexcept;
```

Tasks:
1. Use `static_assert` in combination with `std::is_integral<T>::value` from `<type_traits>` to ensure that `T` is an integral type.
2. Use the error message `Cannot perform bit operation with non-integral type!` in the `static_assert`.
3. Clear the selected bit in the register.
4. Print the result in binary using C++ I/O functionality:
    * Output stream `std::cout` from `<iostream>` for printing.
    * `std::bitset<N>` from `<bitset>` to generate a bitset representation.

Example usage:

```cpp
std::uint8_t reg{0xFFU};
clear(reg, 2U);
std::cout << "Register content: " << std::bitset<8>(reg) << "\n";
```

Expected output:

```text
Register content: 11111011
```

---

### Exercise 4.2 – Toggle Bit
Create a function template for toggling one or several bits in a register using a parameter pack in the same anonymous namespace as in Exercise 4.1:

```cpp
template<typename T, typename... Bits>
void toggle(T& reg, const Bits... bits) noexcept;
```

Tasks:
1. Use a parameter pack.
2. Ensure the type is integral as in Exercise 4.1.
3. Iterate over the bits (for example using `{bits...}`).
4. Toggle all specified bits in the register.
5. Print the result in binary using C++ I/O functionality:
    * Output stream `std::cout` from `<iostream>` for printing.
    * `std::bitset<N>` from `<bitset>` to generate a bitset representation.

Example usage:

```cpp
std::uint8_t reg{0xFFU};
clear(reg, 2U);
toggle(reg, 0U, 2U, 4U, 6U);
std::cout << "Register content: " << std::bitset<8>(reg) << "\n";
```

Expected output:

```text
Register content: 10100101
```

---
