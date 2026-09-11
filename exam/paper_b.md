# Modern Embedded C++ - Written Examination, Paper B

**Time:** 3 hours. **Closed book.** No compiler and no reference material. **Total: 100 marks.**

This paper exists to test your own skills and knowledge. It is not a qualification and it gates
nothing. It draws on all six lectures, so **it is meant to be taken once the course is over**.

---

## Rubric

**C++17** is assumed throughout. Standard headers may be assumed included unless a question asks you
to name them.

**Code is marked on semantics, not on syntax.** A missing semicolon, a forgotten `#include` or a
brace in the wrong column costs nothing. A missing `virtual` on a destructor costs everything.

**Where a question says "in the course's style"**, it means the conventions used in the appendices:

* `camelCase` for functions and methods, type names with a leading capital.
* Private member variables prefixed `my`, private static members prefixed `our`.
* `static constexpr` members named with a leading capital, e.g. `MaxInstances`.
* Brace initialization, `noexcept` on driver methods, `[[nodiscard]]` on queries and repeated on
  overriding methods.
* Copy and move operations deleted on a class that represents a unique hardware resource.

**Where a question says "state the consequence"**, naming the defect earns half the marks and
saying what it does to the running program earns the other half. "This is wrong" scores nothing.

**Where a question asks you to write a class**, documentation comments are not required and are not
marked. Write the code.

### Marks

| Question | 1   | 2   | 3   | 4   | 5   | 6   | 7   | 8   |
| -------- | --- | --- | --- | --- | --- | --- | --- | --- |
| Marks    | 12  | 12  | 12  | 13  | 13  | 13  | 12  | 13  |

---

## Question 1 - The vocabulary (12 marks)

**(a)** State what difference the extension `.h` or `.hpp` makes to the compiler, and what actually
decides whether a file is compiled as C or as C++. State what each extension signals by convention,
and say which of the two a vendor's register-definition header should carry, with the reason.
(3 marks)

**(b)** State what an anonymous namespace does and name the C construct it replaces. Then state the
purpose of a nested namespace such as `driver::gpio`, and write it in both the pre-C++17 form and
the compact modern form. (3 marks)

**(c)** Give three reasons references are usually preferred over pointers when a function must
modify its argument. Then name one thing a pointer can do that a reference cannot, and give an
example from the course where that matters. (3 marks)

**(d)** Complete the function template below so that it sets every bit in the pack, using a C++17
fold expression rather than a loop. State what the compiler generates for the call
`set(reg, 1U, 2U, 3U)`. (3 marks)

```cpp
template<typename T, typename... Bits>
constexpr void set(T& reg, const Bits... bits) noexcept
{
    static_assert(std::is_integral<T>::value,
        "Failed to set bit in register: T must be of integral type!");
    // Your code here.
}
```

---

## Question 2 - The same driver, twice (12 marks)

Below is a PWM driver as it would be written in C.

```c
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    uint8_t pin;
    uint16_t period_ms;
    bool enabled;
} pwm_t;

void pwm_init(pwm_t* self, const uint8_t pin, const uint16_t period_ms)
{
    if (NULL == self) { return; }
    self->pin       = pin;
    self->period_ms = period_ms;
    self->enabled   = false;
}

void pwm_set_enabled(pwm_t* self, const bool enabled)
{
    if (NULL == self) { return; }
    self->enabled = enabled;
}

bool pwm_is_enabled(const pwm_t* self)
{
    return NULL != self ? self->enabled : false;
}
```

**(a)** Rewrite it as a single-header C++ driver in the course's style. It shall:

* live in namespace `driver::pwm`, as a class named `Pwm` that cannot be inherited from;
* replace `pwm_init` with a constructor that cannot be used for implicit conversions and whose
  period defaults to `20U`;
* keep the pin and the period unchangeable after construction;
* expose `pin()`, `period_ms()`, `isEnabled()` and `setEnabled()`, with the query methods marked so
  that ignoring their result warns;
* be impossible to copy or move.

(6 marks)

**(b)** Name **four** distinct failure modes that exist in the C version and cannot occur in your
C++ version, and state for each what removes it. (4 marks)

**(c)** Your header and the vendor's C register header sit in the same project. State which
extension each should carry, and what must be added to the vendor's header before your driver can
call into it. (2 marks)

---

## Question 3 - Enumerations, files, and lifetime (12 marks)

**(a)** Write the enumeration class `Direction` from L02 in namespace `driver::gpio`, with the
enumerators `Input`, `InputPullup` and `Output`, a fixed underlying type, and whatever is needed to
support the validity check below. Then write that check.

```cpp
[[nodiscard]] constexpr bool isDirectionValid(Direction direction) noexcept;
```

Give three reasons to prefer an enumeration class over a plain `enum`, and state what fixing the
underlying type buys on an embedded target. (5 marks)

**(b)** A class is split into `driver/gpio/gpio.hpp` and `driver/gpio/gpio.cpp`. For each of the
following, state whether it belongs in the header, in the source file, or in both, and why:

`explicit`, a default argument, `[[nodiscard]]`, the trailing `const` on a query method, `noexcept`,
`= default` and `= delete`. (4 marks)

**(c)** State when the constructor runs and when the destructor runs for each of:

1. a local object inside a function;
2. an object at namespace scope;
3. an object created with `new`.

For the second, name the hazard this creates in an embedded program. (3 marks)

---

## Question 4 - What the compiler writes for you (13 marks)

**(a)** The `Gpio` class from L02 declares its pin as `const std::uint8_t myPin;`. A copy
*constructor* that makes a complete copy can be written for this class; a copy *assignment
operator* that does so cannot. Explain why, and state what the compiler does about the copy
assignment operator when you do not declare one.
(4 marks)

**(b)** A class `Buffer` owns a heap block through two members:

```cpp
std::uint8_t* myData;
std::size_t mySize;
```

Write its move constructor. Then state the one thing a move *assignment operator* must do that a
move *constructor* need not, and explain why the constructor is exempt. (4 marks)

**(c)** State what `= default` and `= delete` each mean. Give the block of operations a driver class
representing a unique hardware resource should delete, and the design reason.

Then answer this: writing `~Gpio() noexcept = default;` is often described as costing nothing,
because the compiler would have generated the same destructor anyway. State what it nevertheless
changes about the rest of the class. (5 marks)

---

## Question 5 - Designing against an abstraction (13 marks)

**(a)** Write the interface `driver::serial::Interface` in the course's style. It shall support
initializing the port with a baud rate, writing a null-terminated string, and querying whether the
port has been initialized.

Then state, for four of the conventions the appendix applies to an interface, what each is and
why it is there. (5 marks)

**(b)** State the difference between `private` and `protected` members. Name the three kinds of
inheritance in C++, state what each does to the access level of inherited members, and state which
one a concrete driver uses to implement an interface, and why. (4 marks)

**(c)** A colleague writes:

```cpp
void sendGreeting(driver::serial::Interface serial) noexcept
{
    serial.write("Hello!\n");
}
```

State what happens when this is compiled, and why. Then state what *would* have happened had the
base class not been abstract, and name the mechanism. Give the corrected signature, and state one
reason not to make it `const`. (4 marks)

---

## Question 6 - Factories (13 marks)

**(a)** Draw or describe the dependency graph of the four layers in the L04 example: system logic,
factory interface, concrete factories, concrete drivers. State which dependency is the one that
makes the system testable without hardware, and why. (3 marks)

**(b)** Write the stub factory `driver::factory::Stub` for the smart-pointer version of the factory
interface

```cpp
[[nodiscard]] virtual std::unique_ptr<gpio::Interface> gpio(std::uint8_t pin) noexcept = 0;
```

including whatever is needed to keep the compiler quiet about the unused pin. (4 marks)

**(c)** Give four differences between a raw `gpio::Interface*` and a
`std::unique_ptr<gpio::Interface>` as the return type of `gpio()`. State where `std::move` becomes
necessary once the logic class stores one, and what the source pointer holds afterwards. (4 marks)

**(d)** Name one cost the factory pattern imposes on an embedded system, and describe the
alternative shown in L04 that avoids it entirely. (2 marks)

---

## Question 7 - A container with its size in the type (12 marks)

**(a)** Write a class template `container::Array<T, Size>` in namespace `container`, in a single
header, providing:

* a default constructor;
* `push(const T& element)`, returning `false` if the array is already full;
* `size()` and `capacity()`;
* `operator[]` in both a modifiable and a read-only form;
* a compile-time check rejecting a size of zero, with a message.

The class shall not allocate memory. (6 marks)

**(b)** State why this class cannot have its method definitions placed in `array.cpp`, and what the
build would report if you tried. Then state how many distinct types the following three declarations
produce, with the reason, and what that means for the size of the binary. (4 marks)

```cpp
container::Array<std::uint8_t, 8U> a{};
container::Array<std::uint8_t, 16U> b{};
container::Array<std::uint16_t, 8U> c{};
```

**(c)** State one advantage `container::Array<T, Size>` has over the heap-based
`container::Vector<T>` from the appendix on a target where dynamic allocation is forbidden, and one
thing you give up by putting the capacity in the type. (2 marks)

---

## Question 8 - Sharing state between threads (13 marks)

**(a)** State the three conditions that together constitute a data race. State what the standard
says the program then does, and why that is a stronger statement than "the value may be wrong".
(3 marks)

**(b)** The TX and RX threads in L06 share this structure under a mutex:

```cpp
struct SharedMem
{
    std::uint16_t data{};
    bool newData{false};
};
```

A colleague deletes the mutex and writes instead:

```cpp
struct SharedMem
{
    std::atomic<std::uint16_t> data{};
    std::atomic<bool> newData{false};
};
```

State whether the data race is gone. State whether the program is correct, and justify your answer
with a specific interleaving. Then state the two things a mutex provides that a pair of atomics does
not. (4 marks)

**(c)** Name the two launch policies of `std::async` and state what each does. State what happens
when no policy is passed, and describe the failure this produces in code written as "start the work,
do something else, collect the result later". (3 marks)

**(d)** Describe priority inversion as a sequence of events involving three threads. State why
`std::mutex` cannot prevent it, and name what does. (3 marks)

---
