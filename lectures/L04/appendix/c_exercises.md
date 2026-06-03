# Appendix C

## Exercises
These exercises reinforce the concepts from [Appendix A](./a_factory_raw_pointers.md) and 
[Appendix B](./b_factory_smart_pointers.md).

Create the following directory structure:

```text
Makefile
include/
    app/
        logic/
            logic.h
    driver/
        factory/
            esp32s3.h
            interface.h
            stub.h
        serial/
            esp32s3.h
            interface.h
            stub.h
source/
    main.cpp
```

---

# Exercise Set 1 – Serial interface and drivers

## Exercise 1.1 – Serial interface
In this exercise you will design an interface `driver::serial::Interface` representing a generic serial driver.

### Tasks
Design a class named `driver::serial::Interface` that represents a generic serial communication driver.

All methods except the destructor shall be pure virtual (`= 0`).

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
* Takes a reference to a variable where the byte will be stored.
* Returns `true` if a byte was received, otherwise `false`.
* Cannot throw exceptions.

---

## Exercise 1.2 – Stub serial driver
In this exercise you will implement a stub driver `driver::serial::Stub`.

A stub driver simulates hardware behavior and is useful for testing without real hardware.

The class shall:
* Inherit from `driver::serial::Interface`.
* Be marked `final`.

### a) Member variables  
Add three private member variables:
* The first member variable shall:
    * Store the most recently transmitted byte.
    * Have the type `std::uint8_t`.
    * Be named `myLastByte`.
* The second member variable shall:
    * Indicate whether the driver is initialized.
    * Have the type `bool`.
    * Be named `myInitialized`.
* The third member variable shall:
    * Indicate whether a byte is available to read.
    * Have the type `bool`.
    * Be named `myHasData`.

### b) Constructor  
Add a default constructor that:
* Sets the driver as initialized.
* Indicates that no data is available to read.
* Is marked `noexcept`.

### c) Destructor  
Add a destructor that is:
* Public.
* Marked `noexcept`.
* Implemented using `= default`.

### d) Implement interface methods  
Implement all methods required by the interface:
* The method `isInitialized()` shall:
    * Return the value stored in `myInitialized`.
* The method `write()` shall:
    * Do nothing if `myInitialized` is `false`.
    * Otherwise:
        * Store the transmitted byte in `myLastByte`.
        * Set `myHasData` to `true`.
* The method `read()` shall:
    * Return `false` if `myInitialized` is `false` or `myHasData` is `false`.
    * Otherwise:
        * Copy the stored byte (`myLastByte`) into the output argument.
        * Set `myHasData` to `false`.
        * Return `true`.

### e) Disable copy and move semantics
Delete the following functions (in the public section of the class):
* Copy constructor.
* Move constructor.
* Copy assignment operator.
* Move assignment operator.

---

## Exercise 1.3 – Serial driver for ESP32-S3
In this exercise you will create a hardware-specific serial driver `driver::serial::Esp32s3`.

This class represents a serial driver for the ESP32-S3 microcontroller.

The class shall:
* Inherit from `driver::serial::Interface`.
* Be marked `final`.

### a) Member variables  
Add two private member variables:
* The first shall store the transmit pin number.
* The second shall store the receive pin number.
* Both shall have the type `std::uint8_t` and be marked `const`.
* They shall be named `myTxPin` and `myRxPin`.

### b) Constructor  
Add an explicit constructor that:
* Takes a transmit pin number.
* Takes a receive pin number.
* Stores them in the corresponding member variables.
* Is marked `noexcept`.

### c) Destructor  
Add a destructor that is:
* Public.
* Marked `noexcept`.
* Declared with `override`.

### d) Interface methods  
Declare and implement the required interface methods.

For this exercise, it is sufficient to use placeholder implementations:
* `isInitialized()` may always return `true`.
* `write()` shall print the transmitted byte in hexadecimal format. For example, transmitting byte `0xFF` on TX pin `17` shall print `Transmitting byte FF via TX pin 17`.
* `read()` may always return `false`.

### e) Disable copy and move semantics
Delete the following functions (in the public section of the class):
* Default constructor.
* Copy constructor.
* Move constructor.
* Copy assignment operator.
* Move assignment operator.

---

# Exercise Set 2 – Factory interface with raw pointers

## Exercise 2.1 – Factory interface
In this exercise you will design a factory interface `driver::factory::Interface`.

The purpose of this factory is to create serial drivers without exposing the concrete driver type to the application logic.

### Tasks
Design a class named `driver::factory::Interface`.

---

### a) Destructor  
Add a destructor that is:
* Virtual.
* Marked `noexcept`.
* Implemented using `= default`.

---

### b) Factory method  
Add a pure virtual method `serial()` that:
* Creates a serial driver.
* Takes a transmit pin number of type `std::uint8_t`.
* Takes a receive pin number of type `std::uint8_t`.
* Returns a pointer to `driver::serial::Interface`.
* Is marked `noexcept`.

---

## Exercise 2.2 – ESP32-S3 factory
In this exercise you will implement `driver::factory::Esp32s3`.

This factory shall create real drivers for ESP32-S3.

The class shall:
* Inherit from `driver::factory::Interface`.
* Be marked `final`.

### a) Constructor and destructor  
Add:
* A default constructor marked `noexcept`.
* A destructor marked `noexcept` and `override`.

### b) Factory method  
Implement the method `serial()` so that it:
* Takes a transmit pin number and a receive pin number as input.
* Dynamically allocates a `driver::serial::Esp32s3`.
* Returns the object as a pointer to `driver::serial::Interface`.

Use the operator `new`.

### c) Disable copy and move semantics
Delete the following functions (in the public section of the class):
* Copy constructor.
* Move constructor.
* Copy assignment operator.
* Move assignment operator.

---

## Exercise 2.3 – Stub factory
In this exercise you will implement `driver::factory::Stub`.

This factory shall create stub serial drivers.

The class shall:
* Inherit from `driver::factory::Interface`.
* Be marked `final`.

### a) Constructor and destructor  
Add:
* A default constructor marked `noexcept`.
* A destructor marked `noexcept` and `override`.

### b) Factory method  
Implement the method `serial()` so that it:
* Takes a transmit pin number and a receive pin number as input.
* Ignores both pin numbers.
* Dynamically allocates a `driver::serial::Stub`.
* Returns the object as a pointer to `driver::serial::Interface`.

Use the operator `new`.

### c) Disable copy and move semantics
Delete the following functions (in the public section of the class):
* Copy constructor.
* Move constructor.
* Copy assignment operator.
* Move assignment operator.

---

# Exercise Set 3 – Application logic with raw pointers

## Exercise 3.1 – Logic class using a factory
In this exercise you will implement a logic class `app::logic::Logic` that uses a factory to create its serial driver.

The application logic shall:
* Own one serial driver.
* Periodically transmit a message.
* Attempt to read one received byte.

### a) Member variable  
Add a private member variable `mySerial` of type `driver::serial::Interface*`.

### b) Constructor  
Add a constructor that:
* Takes a reference to `driver::factory::Interface`.
* Takes a transmit pin number of type `std::uint8_t`.
* Takes a receive pin number of type `std::uint8_t`.
* Is marked `noexcept`.

Inside the constructor:
* Use the factory to create the serial driver.

### c) Destructor  
Add a destructor that:
* Is marked `noexcept`.
* Deletes `mySerial`.

### d) `run()` method  
Add a method `run()` that:
* Is marked `noexcept`.
* Runs continuously.
* Sends an incrementing byte value (`0–255`). After reaching `255`, the value wraps around to `0`.
* Attempts to read one byte into a local variable and prints it if received.
* Repeats forever.
* Delays execution by `100 ms` at the end of each loop iteration:

```cpp
#include <chrono>
#include <thread>

constexpr std::uint8_t sleep_ms{100U};

std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
```

Use local variables of type `std::uint8_t` to store the transmitted and received bytes:

```cpp
std::uint8_t txByte{}, rxByte{};
```

### e) Disable copy and move semantics
Delete the following functions (in the public section of the class):
* Default constructor.
* Copy constructor.
* Move constructor.
* Copy assignment operator.
* Move assignment operator.

---

## Exercise 3.2 – Using the raw-pointer factory in `main`
In this exercise you will test your application using both factories.

### a) ESP32-S3 factory  
In `main.cpp`:
* Create an instance of `driver::factory::Esp32s3`.
* Create an instance of `app::logic::Logic` using that factory.
* Pass suitable pin numbers, for example `17U` and `18U`.
* Call `run()`.

### b) Stub factory  
Modify `main.cpp` so that:
* `driver::factory::Stub` is used instead of `driver::factory::Esp32s3`.

### c) Reflection  
Answer the following questions:
* What code in `Logic` had to change when switching from the ESP32-S3 factory to the stub factory?
* What code in `main()` had to change?

---

# Exercise Set 4 – Factory interface with smart pointers

## Exercise 4.1 – Updating the factory interface
In this exercise you will modernize the factory interface by replacing raw pointers with `std::unique_ptr`.

### Tasks
Update `driver::factory::Interface` so that the method `serial()`:
* Returns `std::unique_ptr<driver::serial::Interface>`.
* Still takes a transmit pin number of type `std::uint8_t`.
* Still takes a receive pin number of type `std::uint8_t`.
* Is marked `noexcept`.

Also:
* Include the header `<memory>` where needed.

---

## Exercise 4.2 – Updating the concrete factories
In this exercise you will update both concrete factories to use smart pointers.

### a) ESP32-S3 factory  
Modify `driver::factory::Esp32s3` so that:
* The method `serial()` returns `std::unique_ptr<driver::serial::Interface>`.
* The object is created using `std::make_unique<driver::serial::Esp32s3>(txPin, rxPin)`.

### b) Stub factory  
Modify `driver::factory::Stub` so that:
* The method `serial()` returns `std::unique_ptr<driver::serial::Interface>`.
* The object is created using `std::make_unique<driver::serial::Stub>()`.
* The pin numbers are ignored.

---

## Exercise 4.3 – Updating the logic class
In this exercise you will update `app::logic::Logic` to use smart pointers.

### a) Member variable  
Replace the raw pointer member `mySerial` with a `std::unique_ptr<driver::serial::Interface>`.

### b) Constructor  
Keep the constructor parameters the same as before, but initialize the member variable using the factory’s smart-pointer return value.

### c) Destructor  
Remove the destructor entirely.

### d) `run()` method  
Keep the behavior the same as before:
* Send the character `'A'`.
* Attempt to read one byte.
* Repeat forever.

Remember that member access through a smart pointer still uses the arrow operator `->`.

---
