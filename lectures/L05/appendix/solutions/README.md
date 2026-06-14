# Exercise solutions

Reference implementations for the exercises in [Appendix D](../d_exercises.md).

**Note!** Try to solve the exercises yourself before looking at the solutions.

## Solutions
* [function_template/main.cpp](./function_template/main.cpp):
    * Implementation of the bit utility function templates `clear()` and `toggle()`.
* [class_template/include/driver/timer/timer.h](./class_template/include/driver/timer/timer.h):
    * Class template `driver::timer::Timer` with a primary stub implementation and a specialization for `driver::timer::Type::Stm32`.
* [class_template/source/main.cpp](./class_template/source/main.cpp):
    * Demonstration program for the timer driver template.

---

## Function Template Solution
The function template solution implements two generic bit operations:
* `clear(T& reg, std::uint8_t bit)`
* `toggle(T& reg, const Bits... bits)`

Both templates use `static_assert()` to verify that the register type is arithmetic.

The solution demonstrates how parameter packs and compile-time type constraints can make bit operations reusable for different integral register types.

---

## Class Template Solution
The class template solution demonstrates compile-time selection of timer implementations using template specialization.

---

### What is implemented
* `driver::timer::Type` enum class with values `Stm32` and `Stub`.
* Primary template `template<Type T = Type::Stub> class Timer final` for the stub timer implementation.
* Full specialization `template<> class Timer<Type::Stm32> final` for the STM32 timer implementation.

---

### Shared public interface
Both timer variants provide the same public API:
* `timeout_ms()`
* `isRunning()`
* `isInitialized()`
* `start()`
* `stop()`
* `toggle()`
* `tick()`
* `hasTimedOut()`

---

### Main demo program
The solution demo uses:
* A stub timer with a `500 ms` timeout.
* An STM32 timer with a `1500 ms` timeout.

Both timers are run during `2000 ms`. A message is printed every time a timer times out.

This shows how the same API can work for both stub and STM32 timer implementations without runtime polymorphism (i.e. interfaces).

---

### Answers to reflection questions
**a) Why is the timer type selected at compile time?**
* The timer type is selected at compile time so the compiler generates concrete code for the chosen implementation.  
* This avoids runtime overhead from virtual dispatch and allows the compiler to optimize each concrete timer class.

**b) What is a template specialization?**
* A template specialization is a custom implementation of a template for a specific template argument.  
* In this exercise, the primary `Timer` template provides the stub timer, and the specialization provides the STM32 timer.

**c) What advantages does this design provide compared to using inheritance and virtual functions?**  
* It avoids runtime polymorphism overhead, such as vtable lookups and virtual function calls.  
* Each implementation is compiled separately, allowing more aggressive optimization.  
* The same public interface is preserved while behavior is selected at compile time.

**d) When would inheritance be a better choice?**
* Inheritance is better when the concrete implementation must be selected at runtime.  
* It is useful for plugin-style architectures or when many interchangeable implementations must be handled through a common base pointer/reference.

**e) What happens if the specialization for `Type::Stm32` is removed and a `Timer<Type::Stm32>` instance is created?** 
* If the specialization is removed, the compiler will use the primary template implementation for `Timer<Type::Stm32>`.  
* That means the instance would behave like the stub timer rather than a true STM32 timer.

**f) Why might a template specialization be preferred over a virtual interface in a small microcontroller system?**  
* Because it removes runtime dispatch overhead and eliminates the need for a vtable.  
* It keeps code paths explicit and allows the compiler to optimize the code for the target hardware.  
* This is often better for resource-constrained microcontrollers where performance and code size matter.

---
