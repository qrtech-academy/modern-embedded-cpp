# Modern Embedded C++ - Written Examination, Paper A

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

**Where a question asks you to find defects**, the number of defects is stated. Listing more than
the stated number is not penalised, but only the stated number is marked, so put your strongest
answers first.

### Marks

| Question | 1   | 2   | 3   | 4   | 5   | 6   | 7   | 8   |
| -------- | --- | --- | --- | --- | --- | --- | --- | --- |
| Marks    | 12  | 12  | 12  | 13  | 13  | 13  | 12  | 13  |

---

## Question 1 - Crossing the line between C and C++ (12 marks)

**(a)** A silicon vendor ships a header `hal_gpio.h` and a precompiled library, both built with a C
compiler. Your application is C++ and calls `halGpioInit()`. The code compiles, and the linker then
reports:

```text
undefined reference to `halGpioInit()'
```

Explain the mechanism that produces this error. Write the minimal change to the vendor's header
that fixes it while keeping the header usable from C, and state why that change has to be guarded.
(4 marks)

**(b)** State three concrete advantages of `constexpr` over `#define` for a constant such as a clock
frequency, and give one thing a `constexpr` value can be used for that a runtime `const` variable
cannot. (3 marks)

**(c)** State what `noexcept` guarantees, what happens at runtime if a `noexcept` function throws
anyway, and give two reasons why exceptions are commonly disabled entirely in an embedded build.
(3 marks)

**(d)** L01 states that a function which may allocate dynamically should **not** be marked
`noexcept`. The factory interface in L04 returns `std::make_unique<...>` and **is** marked
`noexcept`. Reconcile the two, and state exactly which runtime behavior the L04 choice selects.
(2 marks)

---

## Question 2 - Object lifetime, and a driver that nearly works (12 marks)

**(a)** Consider the program below.

```cpp
#include <cstdint>
#include <cstdio>

namespace driver
{
struct Gpio
{
    explicit Gpio(const std::uint8_t pin) noexcept
        : myPin{pin}
    {
        std::printf("Init %u\n", myPin);
    }

    ~Gpio() noexcept { std::printf("Release %u\n", myPin); }

private:
    const std::uint8_t myPin;
};
} // namespace driver

int main()
{
    driver::Gpio led{9U};
    {
        driver::Gpio button{13U};
        driver::Gpio buzzer{7U};
        std::printf("Inner\n");
    }
    std::printf("Outer\n");
    return 0;
}
```

Write the exact output, in order. Then state the two rules about destructors that your answer
depends on. (4 marks)

**(b)** The software timer below compiles and passes a smoke test. It contains **four** defects
against the course's own rules. Identify each one, state the consequence, and give the corrected
line. (6 marks)

```cpp
namespace driver
{
struct Timer
{
    Timer(const std::uint16_t timeout_ms) noexcept
        : myCounter_ms{0U}
        , myTimeout_ms{timeout_ms}
        , myRunning{false}
    {}

    ~Timer() noexcept = default;

    void start() noexcept { myRunning = true; }
    void stop() noexcept { myRunning = false; }

    std::uint16_t timeout_ms() noexcept { return myTimeout_ms; }

    [[nodiscard]] bool elapsed() noexcept
    {
        if (!myRunning) { return false; }
        if (++myCounter_ms < myTimeout_ms) { return false; }
        myCounter_ms = 0U;
        return true;
    }

private:
    const std::uint16_t myTimeout_ms;
    std::uint16_t myCounter_ms;
    bool myRunning;
};
} // namespace driver
```

**(c)** State what RAII means, and give one embedded-specific example of hardware state that a
destructor should undo. (2 marks)

---

## Question 3 - Classes, and what belongs to the type (12 marks)

**(a)** State the only formal difference between a `struct` and a `class` in C++. Then state the
convention this course uses to decide which of the two to reach for. (2 marks)

**(b)** Consider the following, which compiles and runs:

```cpp
class Buzzer final
{
public:
    Buzzer(const std::uint16_t frequency_hz) noexcept
        : myFrequency_hz{frequency_hz}
    {}

    [[nodiscard]] std::uint16_t frequency_hz() const noexcept { return myFrequency_hz; }

private:
    const std::uint16_t myFrequency_hz;
};

void playAlarm(const Buzzer buzzer) noexcept
{
    std::printf("Alarm at %u Hz\n", buzzer.frequency_hz());
}

int main()
{
    playAlarm(4000U);
    return 0;
}
```

State what the compiler does at the call to `playAlarm`, why this is a hazard rather than a
convenience, and the one-word change that makes the call a compile error. (3 marks)

**(c)** A `Gpio` class is to gain two shared values: the maximum number of GPIO instances the MCU
supports, and a count of how many `Gpio` objects currently exist.

Declare both in the course's style, state where each is defined and why the two differ on that
point, and state what a `static` method cannot access and why. (4 marks)

**(d)** State what `final` does when applied to a class. Give one reason a driver class should
normally have it, and one concrete reason you might deliberately leave it off. (3 marks)

---

## Question 4 - The class that owns something (13 marks)

The class below is used to hold a receive buffer.

```cpp
namespace util
{
class Buffer final
{
public:
    explicit Buffer(const std::size_t size) noexcept
        : myData{new std::uint8_t[size]{}}
        , mySize{size}
    {}

    ~Buffer() noexcept { delete[] myData; }

    Buffer(const Buffer& other) noexcept
        : myData{other.myData}
        , mySize{other.mySize}
    {}

    Buffer& operator=(Buffer&& other) noexcept
    {
        myData = other.myData;
        mySize = other.mySize;
        return *this;
    }

    [[nodiscard]] std::size_t size() const noexcept { return mySize; }

private:
    std::uint8_t* myData;
    std::size_t mySize;
};
} // namespace util
```

**(a)** Name the four special member functions concerned with copying and moving, and write the
signature of each for `Buffer`. (3 marks)

**(b)** The copy constructor and the move assignment operator are both wrong. For each, state the
defect and describe precisely what happens to the heap when the following runs. (6 marks)

```cpp
util::Buffer a{64U};
util::Buffer b{a};

util::Buffer c{32U};
c = std::move(a);
```

**(c)** Rewrite the move assignment operator correctly. (2 marks)

**(d)** State what `std::move()` actually does. It is not what its name suggests. (2 marks)

---

## Question 5 - Interfaces, and the keyword that is missing (13 marks)

```cpp
namespace driver::sensor
{
/**
 * @brief Temperature sensor interface.
 */
class Interface
{
public:
    ~Interface() noexcept = default;

    virtual void start() noexcept                             = 0;
    [[nodiscard]] virtual std::uint16_t read() const noexcept = 0;
};

/**
 * @brief Temperature sensor driver for the BME280.
 */
class Bme280 final : public Interface
{
public:
    explicit Bme280(const std::uint8_t address) noexcept
        : myAddress{address}
        , mySamples{new std::uint16_t[SampleCount]{}}
    {}

    ~Bme280() noexcept { delete[] mySamples; }

    void start() noexcept override {}
    [[nodiscard]] std::uint16_t read() const noexcept override { return mySamples[0U]; }

private:
    /** Number of samples held by the driver. */
    static constexpr std::size_t SampleCount{16U};

    const std::uint8_t myAddress;
    std::uint16_t* mySamples;
};
} // namespace driver::sensor

void run(driver::sensor::Interface* sensor) noexcept
{
    sensor->start();
    delete sensor;
}
```

**(a)** The code above compiles without error. State exactly what happens when `run()` is called
with a pointer to a heap-allocated `Bme280`, and why. (4 marks)

**(b)** Give the single-word change that fixes it. Then state what the author could have written on
**one other line** so that the compiler would have rejected the broken version outright. (3 marks)

**(c)** A colleague implements the sensor for another chip and writes the read method as:

```cpp
[[nodiscard]] std::uint16_t read() noexcept override { return 0U; }
```

State what the compiler reports, and state what it would have reported had `override` been left off.
(3 marks)

**(d)** Give two advantages of programming against `driver::sensor::Interface` rather than against a
concrete driver type, and state what a call through the interface costs at runtime compared with a
direct call. (3 marks)

---

## Question 6 - The factory, and who owns what (13 marks)

**(a)** State the purpose of the factory pattern in an embedded system. Explain what it means that
the system logic depends only on interfaces, and name what has to change in a program to move it
from real drivers to stubs. (4 marks)

**(b)** The program below uses the raw-pointer factory from L04. It contains **two** ownership
defects. Identify each and state the consequence. (5 marks)

```cpp
int main()
{
    driver::factory::Esp32s3 factory{};

    // Spare status LED on pin 4.
    factory.gpio(4U);

    app::logic::Logic logic{factory, 2U, 3U};
    app::logic::Logic backup{logic};

    logic.run();
    return 0;
}
```

This version of `Logic` stores its two drivers as `driver::gpio::Interface*`, obtains them from the
factory in its constructor, and deletes them in its destructor. Its public section declares the
constructor, the destructor and `run()`, and nothing else.

**(c)** The factory is changed to return `std::unique_ptr<driver::gpio::Interface>`. State what
happens to `Logic`'s destructor, and explain why one of the two defects in (b) becomes a compile
error rather than a runtime fault. (4 marks)

---

## Question 7 - Templates (12 marks)

**(a)** A colleague puts a function template's declaration in `bit_util.hpp` and its definition in
`bit_util.cpp`, exactly as they would for an ordinary function. The build fails at link time with
`undefined reference`. Explain why, and give the two arrangements the course recommends instead.
(4 marks)

**(b)** The bit-clearing helper below has been in the codebase for a year and is used on 8-, 16- and
32-bit registers without trouble. Its first use on a 64-bit register corrupts the register.

```cpp
template<typename T>
constexpr void clear(T& reg, const std::uint8_t bit) noexcept
{
    static_assert(std::is_integral<T>::value,
        "Cannot perform bit operation with non-integral type!");
    reg &= ~(1U << bit);
}
```

Given `std::uint64_t reg{0xFFFFFFFFFFFFFFFFU}` and a call `clear(reg, 2U)`, give the exact value of
`reg` afterwards and explain how it got there. Name the second, separate failure this expression has
for any `bit` of 32 or more. Then give the corrected body. (5 marks)

**(c)** With the corrected template in place, the following appears in one translation unit:

```cpp
std::uint8_t reg1{};
std::uint32_t reg2{};
std::uint8_t reg3{};

clear(reg1, 0U);
clear(reg2, 17U);
clear(reg3, 3U);
```

State how many versions of `clear()` the compiler generates and name them. State what this implies
for a resource-constrained target, and name one situation where a template is the wrong tool. (3 marks)

---

## Question 8 - Two threads and a shared struct (13 marks)

```cpp
namespace
{
struct Shared
{
    std::uint16_t data{};
    bool newData{false};
};

Shared shared{};
std::mutex mutex{};
bool stop{false};

// -----------------------------------------------------------------------------
void txThread(const std::uint16_t count) noexcept
{
    for (std::uint16_t i{}; i < count; ++i)
    {
        shared.data    = i;
        shared.newData = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(10U));
    }
    stop = true;
}

// -----------------------------------------------------------------------------
void rxThread() noexcept
{
    while (!stop)
    {
        std::lock_guard<std::mutex> lock{mutex};
        if (shared.newData)
        {
            std::printf("RX: %u\n", shared.data);
            shared.newData = false;
        }
    }
}
} // namespace

// -----------------------------------------------------------------------------
int main()
{
    std::thread t1{txThread, 100U};
    std::thread t2{rxThread};
    t1.detach();
    return 0;
}
```

**(a)** The program contains a mutex, and it is still not thread-safe. Identify **three**
synchronization defects, and for each state the consequence. (6 marks)

**(b)** State what happens at the closing brace of `main()`, and why. Name the two legal ways to
end a `std::thread`'s lifetime and state which one this program should use. (3 marks)

**(c)** Rewrite `rxThread()` so that it waits on a `std::condition_variable` instead of spinning.
Give the predicate, state why `std::unique_lock` must be used rather than `std::lock_guard`, and
state what the writer of the stop flag must additionally do so that the receiver can never be
left asleep forever. (4 marks)

---
