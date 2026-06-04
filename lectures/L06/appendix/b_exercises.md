# Appendix B

## Exercises
These exercises reinforce the concepts from [Appendix A](./a_threads_mutex_atomic.md).

The purpose is to practice:
* Creating and joining threads using `std::thread`.
* Protecting shared data using `std::mutex` and `std::lock_guard`.
* Using `std::atomic` for simple thread-safe stop flags and counters.
* Understanding when a mutex is required instead of an atomic variable.

Create the following directory structure:

```text
Makefile
include/
    app/
        logic/
            logic.h
    driver/
        counter/
            interface.h
            stub.h
source/
    app/
        logic/
            logic.cpp
    driver/
        counter/
            stub.cpp
    main.cpp
```

---

# Exercise Set 1 – Threads and atomic stop flag

## Exercise 1.1 – Creating a worker thread
In this exercise you will create a simple worker function that runs in a separate thread.

### Tasks
Create a function named `workerThread()`.

The function shall:
* Take no parameters.
* Return `void`.
* Print `"Worker thread running"` five times.
* Sleep for `500 ms` between each print.

Use:

```cpp
#include <chrono>
#include <thread>

std::this_thread::sleep_for(std::chrono::milliseconds(500U));
```

---

## Exercise 1.2 – Starting and joining the thread
In `main.cpp`:
* Create a `std::thread` that runs `workerThread()`.
* Print `"Main thread running"` from `main()`.
* Call `join()` on the worker thread before returning from `main()`.

### Reflection
* What happens if `join()` is not called?
* Which thread prints first?
* Is the print order guaranteed?

---

## Exercise 1.3 – Atomic stop flag
In this exercise you will modify the worker thread so that it runs until a stop flag is set.

### Tasks
Create an atomic boolean variable in `main()`:

```cpp
std::atomic<bool> stopFlag{false};
```

Modify `workerThread()` so that it:
* Takes a `const std::atomic<bool>&` named `stopFlag`.
* Runs as long as `stopFlag.load()` is `false`.
* Prints `"Worker thread running"` once every `500 ms`.

In `main()`:
* Start the worker thread.
* Let the main thread sleep for `3000 ms`.
* Set the stop flag using `stopFlag.store(true)`.
* Join the worker thread.

### Reflection
* Why is `std::atomic<bool>` suitable for this stop flag?
* Would a normal `bool` be safe here?
* Why is `load()` used when reading the flag?
* Why is `store()` used when writing the flag?

---

# Exercise Set 2 – Shared counter with data race

## Exercise 2.1 – Shared counter without synchronization

In this exercise you will intentionally create a data race.

### Tasks
Create a shared counter variable in `main()`:

```cpp
std::uint32_t counter{};
```

Create a function named `incrementCounter()`.
The function shall:
* Take a reference to the counter.
* Take a number of iterations.
* Increment the counter once per iteration.

Example function signature:

```cpp
void incrementCounter(std::uint32_t& counter, std::uint32_t iterations) noexcept;
```

In `main()`:
* Create two threads that both call `incrementCounter()`.
* Let each thread increment the counter `100000U` times.
* Join both threads.
* Print the final counter value.

### Reflection
* What final value do you expect?
* Do you always get the expected value?
* Why is this program unsafe?
* Which condition makes this a data race?

---

## Exercise 2.2 – Protecting the counter with a mutex
In this exercise you will fix the data race using `std::mutex`.

### Tasks
Create a mutex in `main()`:

```cpp
std::mutex mutex{};
```

Modify `incrementCounter()` so that it:
* Takes a reference to the mutex.
* Locks the mutex before incrementing the counter.
* Uses `std::lock_guard<std::mutex>`.
* Unlocks automatically when the lock guard goes out of scope.

Example function signature:

```cpp
void incrementCounter(std::uint32_t& counter,
                      std::mutex& mutex,
                      std::uint32_t iterations) noexcept;
```

Inside the loop:

```cpp
std::lock_guard<std::mutex> lock{mutex};
++counter;
```

### Reflection
* What final counter value do you get now?
* Why does the mutex solve the problem?
* Why is `std::lock_guard` safer than manually calling `lock()` and `unlock()`?

---

## Exercise 2.3 – Atomic counter
In this exercise you will replace the mutex-protected counter with an atomic counter.

### Tasks
Replace the counter with:

```cpp
std::atomic<std::uint32_t> counter{};
```

Modify `incrementCounter()` so that it:
* Takes a reference to the atomic counter.
* Increments it once per iteration.

Example function signature:

```cpp
void incrementCounter(std::atomic<std::uint32_t>& counter, std::uint32_t iterations) noexcept;
```

Inside the loop:

```cpp
++counter;
```

### Reflection
* What final counter value do you get?
* Why is `std::atomic<std::uint32_t>` sufficient in this exercise?
* Would an atomic counter still be sufficient if several related variables had to be updated together?

---

# Exercise Set 3 – Shared data protected by mutex

## Exercise 3.1 – Shared memory structure
In this exercise you will create shared data that must be protected by a mutex.

### Tasks
Create a structure named `SharedMem`:

```cpp
struct SharedMem
{
    std::uint16_t data{};
    bool newData{false};
};
```

The structure represents data shared between a transmitter thread and a receiver thread.

---

## Exercise 3.2 – Transmitter thread
Create a function named `txThread()`.

The function shall:
* Take a reference to `SharedMem`.
* Take a reference to `std::mutex`.
* Take a reference to `const std::atomic<bool>` named `stopFlag`.
* Run until the stop flag is set.
* Produce an incrementing `std::uint16_t` value.
* Store the value in `shared.data`.
* Set `shared.newData` to `true`.
* Sleep for `1000 ms` between transmissions.

Example function signature:

```cpp
void txThread(SharedMem& shared, std::mutex& mutex, const std::atomic<bool>& stopFlag) noexcept;
```

The update of `shared.data` and `shared.newData` shall be protected using:

```cpp
std::lock_guard<std::mutex> lock{mutex};
```

---

## Exercise 3.3 – Receiver thread
Create a function named `rxThread()`.

The function shall:
* Take a reference to `SharedMem`.
* Take a reference to `std::mutex`.
* Take a reference to `const std::atomic<bool>` named `stopFlag`.
* Run until the stop flag is set.
* Check whether new data is available.
* Print the received data if `shared.newData` is `true`.
* Set `shared.newData` to `false` after consuming the data.
* Sleep for `100 ms` between receive attempts.

Example function signature:

```cpp
void rxThread(SharedMem& shared, std::mutex& mutex, const std::atomic<bool>& stopFlag) noexcept;
```

The check and update of `shared.data` and `shared.newData` shall be protected by the same mutex.

---

## Exercise 3.4 – Running TX and RX threads
In `main.cpp`:
* Create one `SharedMem` object.
* Create one `std::mutex`.
* Create one atomic stop flag.
* Start one TX thread.
* Start one RX thread.
* Let the main thread sleep for `10000 ms`.
* Set the stop flag.
* Join both threads.

### Reflection
* Why must `data` and `newData` be protected by the same mutex?
* Why is `newData` not atomic in this design?
* What could happen if TX updated `data` and `newData` without locking the mutex?
* Why is the stop flag atomic instead of mutex-protected?

---

# Exercise Set 4 – Thread-safe stub driver

## Exercise 4.1 – Counter driver interface
In this exercise you will design an interface named `driver::counter::Interface`.

The interface represents a generic counter driver.

### Tasks
Design a class named `driver::counter::Interface`.

All methods except the destructor shall be pure virtual (`= 0`).

---

### a) Destructor
Add a destructor that is:
* Virtual.
* Marked `noexcept`.
* Implemented using `= default`.

---

### b) Incrementing the counter
Add a pure virtual method `increment()` that:
* Increments the counter by one.
* Does not return a value.
* Cannot throw exceptions.

---

### c) Reading the counter
Add a pure virtual method `value()` that:
* Returns the current counter value as `std::uint32_t`.
* Does not modify the object.
* Cannot throw exceptions.

---

### d) Resetting the counter
Add a pure virtual method `reset()` that:
* Resets the counter to zero.
* Does not return a value.
* Cannot throw exceptions.

---

## Exercise 4.2 – Thread-safe stub counter
In this exercise you will implement `driver::counter::Stub`.

The class shall:
* Inherit from `driver::counter::Interface`.
* Be marked `final`.
* Be safe to use from multiple threads.

### a) Member variables
Add two private member variables:
* `myValue`:
    * Type: `std::uint32_t`.
    * Stores the counter value.
* `myMutex`:
    * Type: `mutable std::mutex`.
    * Protects access to `myValue`.

The mutex must be marked `mutable` because `value()` is a `const` method but still needs to lock the mutex.

---

### b) Constructor
Add a default constructor that:
* Initializes the counter to zero.
* Is marked `noexcept`.

---

### c) Destructor
Add a destructor that is:
* Public.
* Marked `noexcept`.
* Declared with `override`.
* Implemented using `= default`.

---

### d) Implement interface methods
Implement all methods required by the interface:
* `increment()` shall:
    * Lock the mutex using `std::lock_guard<std::mutex>`.
    * Increment `myValue`.
* `value()` shall:
    * Lock the mutex using `std::lock_guard<std::mutex>`.
    * Return `myValue`.
* `reset()` shall:
    * Lock the mutex using `std::lock_guard<std::mutex>`.
    * Set `myValue` to zero.

---

### e) Disable copy and move semantics
Delete the following functions in the public section of the class:
* Copy constructor.
* Move constructor.
* Copy assignment operator.
* Move assignment operator.

---

## Exercise 4.3 – Testing the thread-safe stub
In `main.cpp`:
* Create one `driver::counter::Stub`.
* Start two threads.
* Let both threads call `increment()` many times.
* Join both threads.
* Print the final counter value using `value()`.

### Reflection
* Why does `value()` need to lock the mutex even though it only reads the value?
* Why is `myMutex` marked `mutable`?
* What would happen if `increment()` did not lock the mutex?
* Could `std::atomic<std::uint32_t>` be used instead in this specific class?
* Why might a mutex still be preferable in a more complex driver?

---

# Exercise Set 5 – Atomic state and mutex-protected data

## Exercise 5.1 – Driver state
In this exercise you will extend the counter stub with an initialization state.

### Tasks
Add a private member variable to `driver::counter::Stub`:

```cpp
std::atomic<bool> myInitialized;
```

Initialize it to `true` in the constructor.

---

## Exercise 5.2 – Initialization methods
Add two public methods:

```cpp
void setInitialized(bool initialized) noexcept;
bool isInitialized() const noexcept;
```

The methods shall:
* Use `store()` when writing the initialization state.
* Use `load()` when reading the initialization state.

---

## Exercise 5.3 – Guarding counter operations
Modify `increment()`, `value()`, and `reset()`:
* `increment()` shall do nothing if the driver is not initialized.
* `value()` shall return `0U` if the driver is not initialized.
* `reset()` shall do nothing if the driver is not initialized.

The counter value itself shall still be protected by the mutex.

### Reflection
* Why is `myInitialized` suitable as an atomic variable?
* Why is `myValue` still protected by a mutex?
* Would it be safe to make both variables atomic if future requirements added more related state?
* What is the main design difference between a thread-safe flag and thread-safe shared data?

---
