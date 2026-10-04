# Appendix C

## Exercises
These exercises reinforce the concepts from [Appendix A](./a_inheritance.md) and [Appendix B](./b_interfaces.md).

> **How to check your work.** All four exercise sets build one program: write it in
> [`lectures/L03/exercises`](../exercises/README.md), in the directory structure below, and run
> `make test` from the repository root. Each set's tests switch on as soon as its files exist, so
> the interface can be checked before any driver is written, and a set you have not started is
> reported as `SKIP`. The tests compare what the program prints with the output below, character
> for character.

Create the following directory structure:

```text
Makefile
include/
    driver/
        serial/
            console.hpp
            interface.hpp
            stub.hpp
source/
    driver/
        serial/
            console.cpp
            stub.cpp
    main.cpp
```

**Note:** When a class overrides a method that is marked `[[nodiscard]]` in the interface, repeat
the attribute on the overriding method as well. The attribute is not inherited; see
[Appendix B](./b_interfaces.md#structure-of-an-interface).

---

# Exercise Set 1 – Serial interface

## Exercise 1.1 – Interface for serial communication
In this exercise you will design an interface `driver::serial::Interface` representing a generic serial communication driver.

### Tasks
In `driver/serial/interface.hpp`, design a class named `driver::serial::Interface` that represents a generic serial driver.

All methods except the destructor shall be purely virtual (`= 0`).

---

### a) Destructor
Add a destructor that is:
* Virtual.
* Marked `noexcept`.
* Implemented using `= default`.

---

### b) Initialization status
Add a pure virtual method `isInitialized()` that:
* Indicates whether the driver has been initialized (`true/false`).
* Does not modify the object (`const`).
* Cannot throw exceptions.
* Generates a warning if the return value is discarded (`[[nodiscard]]`).

---

### c) Transmitting data
Add a pure virtual method `write()` that:
* Transmits one byte of data.
* Takes a parameter of type `std::uint8_t`.
* Does not return a value.
* Cannot throw exceptions.

---

### d) Receiving data
Add a pure virtual method `read()` that:
* Attempts to read one byte from the serial driver.
* Takes no parameters.
* Returns a `std::optional<std::uint8_t>` from `<optional>`: the received byte, or `std::nullopt`
  if no byte was received.
* Cannot throw exceptions.
* Generates a warning if the return value is discarded (`[[nodiscard]]`): a byte that is read and
  then discarded is lost.

---

# Exercise Set 2 – Stub implementation

## Exercise 2.1 – Serial stub
In `driver/serial/stub.hpp`, implement a stub driver `driver::serial::Stub` for the serial interface.

A stub driver simulates hardware behaviour and is useful for testing without real hardware.

The class shall:
* Inherit from `driver::serial::Interface`.
* Be marked `final`.

Method definitions shall be placed in `driver/serial/stub.cpp`.

---

### a) Member variables
Add two private member variables:
* The first member variable shall:
    * Store the most recently transmitted byte, until it is read.
    * Have the type `std::optional<std::uint8_t>`.
    * Be named `myLastByte`.
    * Be empty when no byte is available to read.
* The second member variable shall:
    * Indicate whether the driver is initialized.
    * Have the type `bool`.
    * Be named `myInitialized`.

### b) Constructor
Add a default constructor that:
* Sets the driver as initialized.
* Leaves `myLastByte` empty, since no data is available to read.

### c) Disable copy and move semantics
Delete the following functions (in the public section of the class):
* Copy constructor.
* Move constructor.
* Copy assignment operator.
* Move assignment operator.

### d) Implement interface methods
Implement all methods required by the interface:
* The method `isInitialized()` shall:
    * Return the value stored in `myInitialized`.
* The method `write()` shall:
    * Do nothing if `myInitialized` is `false`.
    * Otherwise store the transmitted byte in `myLastByte`.
* The method `read()` shall:
    * Return the content of `myLastByte`, which is `std::nullopt` if no byte is available.
    * Leave `myLastByte` empty, so that a byte can only be read once.

### e) Initialization method
Add an additional public method `setInitialized()` that is not part of the interface.

This method shall:
* Be used to simulate the initialization state.
* Take the initialization state as input.
* Discard the stored byte, leaving `myLastByte` empty, when the driver is set to uninitialized.
* Not return a value.

---

# Exercise Set 3 – Singleton driver

## Exercise 3.1 – Console serial driver
In `driver/serial/console.hpp`, create a real driver implementation `driver::serial::Console` that writes transmitted bytes to the system console.

This driver shall follow the so-called singleton pattern, i.e. there is only one instance of the class.

The reason is that the system only has one console output device.

This class shall:
* Inherit from `driver::serial::Interface`.
* Be marked `final`.

Method definitions shall be placed in `driver/serial/console.cpp`.

### a) Private constructor
Add a private default constructor that prevents users from directly creating objects of the class.

### b) Private destructor
Add a private destructor, mark it `noexcept` and `default`.

### c) Prevent multiple instances
Ensure that only one instance of the class can exist.

Delete the following functions (in the public section of the class):
* Copy constructor.
* Move constructor.
* Copy assignment operator.
* Move assignment operator.

### d) Singleton access method
Add a static public method `instance()` that returns a reference to the unique instance of the class.

Requirements:
* The method shall return the console driver as a reference to `driver::serial::Interface`
(even though the instance itself is of type `driver::serial::Console`).
* The method cannot throw exceptions.
* In this method:
    * Create a local static instance of the driver (`static driver::serial::Console`).
    * Initialize the instance using the default constructor.

### e) Implement interface methods
Implement all methods required by the interface:
* The method `isInitialized()` shall:
    * Always return `true`, since the console driver is always available.
* The method `write()` shall:
    * Cast the given byte to a character and print it to the console using `std::printf()` from `<cstdio>`.
* The method `read()` shall:
    * Always return `std::nullopt`, since the console does not support reading.

---

# Exercise Set 4 – Using the interface

## Exercise 4.1 – Sending a message
**a)** In `source/main.cpp`, create a function `sendMessage()` that sends a message using the serial interface.

The function shall:
* Take a reference to `driver::serial::Interface`.
* Transmit the string `"Transmitting data with a serial driver!"`.
* Send the message one byte at a time.

**b)** In function `main`, test the console driver:
* Obtain the singleton instance of the console driver.
* Pass it to the `sendMessage()` function.
* Verify that the program prints the following:

```text
Transmitting data with a serial driver!
```

**c)** In function `main()`, test the stub driver:
* Create an instance of the stub driver and pass it to the function `sendMessage()`.
* Verify that the last transmitted byte `!` is returned by the `read()` method, and that a second
  call to `read()` returns `std::nullopt`.

---
