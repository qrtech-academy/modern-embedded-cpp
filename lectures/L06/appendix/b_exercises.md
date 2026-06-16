# Appendix B

## Exercises
These exercises reinforce the concepts from [Appendix A](./a_threads_mutex_atomic.md).

---

# Exercise Set 1 – Threads and atomic stop flag
Create a file named `exercise1.cpp` to work in for this exercise set.

## Exercise 1.1 – Creating a worker thread
In this exercise you will create a simple worker function that runs in a separate thread.

### Tasks
In `exercise1.cpp`, create a function named `workerThread()`.

The function shall:
* Take a `std::uint8_t` named `printCount`: The number of times to print.
* Take a `std::uint16_t` named `printSpeed_ms`: The delay in milliseconds between each print.
* Return `void`.
* Print `"Worker thread running!"` `printCount` times.
* Sleep for `printSpeed_ms` milliseconds between each print using `std::this_thread::sleep_for()` with `std::chrono::milliseconds`.

---

## Exercise 1.2 – Starting and joining the thread
In `exercise1.cpp`:
* Create a `std::thread` that runs `workerThread()`, passing `5U` as `printCount` and `500U` as `printSpeed_ms`.
* Print `"Main thread running!"` from `main()`.
* Call `join()` on the worker thread before returning from `main()`.

### Reflection
* What happens if `join()` is not called?
* Which thread prints first, the main thread or the worker thread?
* Is the print order guaranteed?

---

## Exercise 1.3 – Atomic stop flag
In this exercise you will modify the worker thread so that it runs until a stop flag is set.

### Tasks
Create an atomic boolean variable in `main()`:

```cpp
std::atomic<bool> stop{false};
```

Modify `workerThread()` so that it:
* Replaces the `printCount` parameter with a `const std::atomic<bool>&` named `stop`, placed after `printSpeed_ms`.
* Runs as long as `stop.load()` is `false`.
* Prints `"Worker thread running"` once every `printSpeed_ms` milliseconds.

In `main()`:
* Start the worker thread.
* Let the main thread sleep for `3000 ms`.
* Set the stop flag using `stop.store(true)`.
* Join the worker thread.

### Reflection
* Why is `std::atomic<bool>` suitable for this stop flag?
* Would a normal `bool` be safe here?
* Why is `load()` used when reading the flag?
* Why is `store()` used when writing the flag?

---

# Exercise Set 2 – Shared counter with data race
Create a file named `exercise2.cpp` to work in for this exercise set.

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
* Take a number of iterations (`std::uint32_t`).
* Increment the counter once per iteration.
* Be marked `noexcept`.

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

### Reflection
* What final counter value do you get?
* Why is `std::atomic<std::uint32_t>` sufficient in this exercise?
* Would an atomic counter still be sufficient if several related variables had to be updated together?

---

# Exercise Set 3 – Shared data protected by mutex
Create a file named `exercise3.cpp` to work in for this exercise set.

## Exercise 3.1 – Shared memory structure
In this exercise you will create shared data that must be protected by a mutex.

### Tasks
In `exercise3.cpp`, create a structure named `SharedMem` with the following members:
* `data`: A 16-bit unsigned value.
* `newData`: True if new data is available, false otherwise.

The structure represents data shared between a transmitter thread and a receiver thread.

---

## Exercise 3.2 – Transmitter thread
In `exercise3.cpp`, create a function named `txThread()`.

The function shall:
* Take a reference to `SharedMem`.
* Take a reference to `std::mutex`.
* Take a reference to `const std::atomic<bool>` named `stop`.
* Run until the stop flag is set.
* Produce an incrementing `std::uint16_t` value.
* Store the value in `shared.data`.
* Set `shared.newData` to `true`.
* Sleep for `1000 ms` between transmissions.

The update of `shared.data` and `shared.newData` shall be protected using a lock guard.

---

## Exercise 3.3 – Receiver thread
In `exercise3.cpp`, create a function named `rxThread()`.

The function shall:
* Take a reference to `SharedMem`.
* Take a reference to `std::mutex`.
* Take a reference to `const std::atomic<bool>` named `stop`.
* Run until the stop flag is set.
* Check whether new data is available.
* Print the received data if `shared.newData` is `true`.
* Set `shared.newData` to `false` after consuming the data.
* Sleep for `100 ms` between receive attempts.

The check and update of `shared.data` and `shared.newData` shall be protected by the same mutex.

---

## Exercise 3.4 – Running TX and RX threads
In `exercise3.cpp`:
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
* Could `std::atomic<SharedMem>` be used instead of a mutex? Why or why not?

---

# Exercise Set 4 – Thread-safe stub driver
Create the following directory structure to work in for this exercise set:

```text
Makefile
include/
    driver/
        counter/
            interface.h
            stub.h
source/
    main.cpp
```

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

### b) Checking initialization state
Add a pure virtual method `isInitialized()` that:
* Returns `true` if the driver is initialized, `false` otherwise.
* Does not modify the object.
* Is marked `noexcept`.

---

### c) Reading the counter
Add a pure virtual method `value()` that:
* Returns the current counter value as `std::uint32_t`.
* Does not modify the object.
* Is marked `noexcept`.

---

### d) Incrementing the counter
Add a pure virtual method `increment()` that:
* Increments the counter by one.
* Does not return a value.
* Is marked `noexcept`.

---

### e) Resetting the counter
Add a pure virtual method `reset()` that:
* Resets the counter to zero.
* Does not return a value.
* Is marked `noexcept`.

---

## Exercise 4.2 – Thread-safe stub counter
In this exercise you will implement `driver::counter::Stub`.

The class shall:
* Inherit from `driver::counter::Interface`.
* Be marked `final`.
* Be safe to use from multiple threads (via a mutex).

### a) Member variables
Add two private member variables:
* `myMutex`:
    * Type: `std::mutex`.
    * Protects access to `myValue` below.
    * Must be marked `mutable` because `value()` is a `const` method but still needs to lock the mutex.
* `myValue`:
    * Type: `std::uint32_t`.
    * Stores the counter value.

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
* `isInitialized()` shall:
    * Return `true`.
* `value()` shall:
    * Lock the mutex using `std::lock_guard<std::mutex>`.
    * Return `myValue`.
* `increment()` shall:
    * Lock the mutex using `std::lock_guard<std::mutex>`.
    * Increment `myValue`.
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
Create a function named `counterThread()`.

The function shall:
* Take a reference to `driver::counter::Interface`.
* Take a `std::uint32_t` named `iterations`, which is the number of times to call `increment()`.
* Be marked `noexcept`.

In `main.cpp`:
* Create one `driver::counter::Stub`.
* Start two threads, both running `counterThread()` — the first with `100U` iterations, the second with `200U`.
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
Add a public method `setInitialized()` specific to the stub. The method shall:
* Take a `bool` named `initialized`.
* Write to `myInitialized` using `store()`.
* Be marked `noexcept`.

Override `isInitialized()` from the interface so that it:
* Reads `myInitialized` using `load()`.
* Returns the current initialization state instead of always returning `true`.

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
* What could happen if `setInitialized(false)` is called between the initialization check and the mutex lock in `increment()`? How would you fix this?

---

# Exercise Set 6 – Condition variable
Create a file named `exercise6.cpp` by copying `exercise3.cpp`. All modifications in this exercise set are made to that file.

## Exercise 6.1 – Replacing polling with a condition variable
In Exercise 3.3 the receiver thread checks for new data by polling in a loop with a `100 ms` sleep. In this exercise you will replace that pattern with a `std::condition_variable` so the receiver wakes up immediately when data is available.

### Tasks
Add a `std::condition_variable` named `cv` alongside the existing mutex.

Create a helper function named `hasNewData()` to use as the wake-up predicate. The function shall:
* Take a `const SharedMem&`.
* Take a `const std::atomic<bool>&` named `stop`.
* Return `true` if `shared.newData` is `true` or the stop flag is set, `false` otherwise.

Modify `txThread()` so that it calls `cv.notify_one()` after releasing the lock guard that protects the data update.

Modify `rxThread()` so that instead of sleeping it:
* Acquires a `std::unique_lock<std::mutex>`.
* Calls `cv.wait()` with the lock and a predicate that calls `hasNewData()`.

> **Note:** The predicate is passed as a lambda; the `[&]` syntax captures `shared` and `stop` by reference so the predicate can access both. Lambdas are covered in the associated appendix.

In `main()`, after setting the stop flag, call `cv.notify_all()` to wake the receiver thread so it can exit cleanly.

### Reflection
* Why is `std::unique_lock` required here instead of `std::lock_guard`?
* What is a spurious wake-up, and how does the predicate protect against it?
* Why must the stop flag also be checked inside the predicate?
* How does this design differ from the polling approach in Exercise 3.3?
* What would happen if `notify_all()` was not called after setting the stop flag?
* Why is `notify_all()` used here instead of `notify_one()`?

---

# Exercise Set 7 – std::async and std::future
Create a new `main.cpp` to work in for this exercise set.

## Exercise 7.1 – Offloading a computation
In this exercise you will use `std::async` to run a validation in the background while the
main thread continues working.

### Tasks
Create a function named `validateFirmware()`. The function shall:
* Take a `const std::uint8_t*` named `buf` and a `std::uint16_t` named `bufLen`.
* Return `false` if `buf` is `nullptr` or `bufLen` is `0`.
* Sleep for `2000 ms` to simulate a slow flash read before validating.
* Iterate over all bytes in `buf`.
* Return `false` if any byte has the value `0xFFU` (unprogrammed flash).
* Return `true` if the image is valid.
* Be marked `noexcept`.

In `main()`:
* Create a buffer with some test data.
* Launch `validateFirmware()` asynchronously using `std::async` with `std::launch::async`, storing the result in a `std::future<bool>`.
* Print `"Booting system while validating firmware..."` while validation is running.
* Retrieve the result using `future.get()` and print whether the firmware is valid or not.

### Reflection
* What does `std::launch::async` guarantee compared to omitting the launch policy?
* At what point does the main thread block?
* What would happen if `future.get()` was called immediately after `std::async`?

---

## Exercise 7.2 – Checking whether the result is ready
Modify `main()` so that instead of calling `future.get()` directly, it polls in a loop:
* Each iteration shall call `future.wait_for()` with `std::chrono::milliseconds(200U)` and compare the result against `std::future_status::ready`. 
* As long as the result is not ready, print `"Still waiting for firmware validation..."`. 
* Once the loop exits, retrieve the result using `future.get()`.

### Reflection
* How does `wait_for()` differ from `get()`?
* Is it safe to call `get()` after `wait_for()` returns `std::future_status::ready`?

---
