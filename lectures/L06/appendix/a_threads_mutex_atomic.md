# Appendix A

## Threads, Mutexes, and Atomic Variables
This appendix introduces the fundamentals of concurrency in C++:
* `std::thread` – Threads for parallel execution of code in C++.
* `std::mutex` combined with `std::lock_guard` – Protection of shared data (critical sections).
* `std::atomic` – How to share individual variables between threads without a mutex.
* The practical differences between atomic variables and mutexes.

---

### Theory

#### 1. What Is a Thread?
* A thread is an execution flow.
* Multiple threads share the same address space.
* In C++, a thread is created using:

```cpp
std::thread t{function, argument};
```

**Note:** The `join()` method must be called to synchronize threads:

```cpp
std::thread t1{function1, argument1};
std::thread t2{function2, argument2};

t1.join();
t2.join();
```

Alternatively, `detach()` can be used to allow a thread to run independently of the other threads, but this is not recommended in the context of this course.

#### 2. What Is a Data Race?
A data race occurs when:
* Two threads access the same memory location.
* At least one access is a write operation.
* No synchronization mechanism is used.

Result:
* Leads to *undefined behavior* – even if the program appears to work during testing.
* The program may work correctly until it suddenly does not.

#### 3. Mutexes and Critical Sections
A mutex is a lock that makes it easy to protect critical sections, i.e., regions containing resources shared between threads.

A mutex is implemented using the `std::mutex` type from the `<mutex>` header:

```cpp
std::mutex mutex{};
```

A mutex can be used to protect resources shared between threads:
* A given mutex is shared between the threads.
* Shared resources are protected by locking the mutex using `lock()`. This call blocks the current thread until the resource becomes available.
* Shared resources are made available to other threads by unlocking the mutex using `unlock()`.
* A mutex can protect multiple related variables simultaneously.

The example below demonstrates how a mutex is shared between two functions running in separate threads. Each thread temporarily gains exclusive access to the shared resources:
* The threads take turns using `std::cout` for terminal output.
* A mutex is used to protect `std::cout` in each thread:
    * A thread reserves `std::cout` as soon as it becomes available by locking the mutex.
    * While `std::cout` is reserved, other threads must wait.
    * Once access is granted, the thread performs its output operation.
    * After the output is complete, the mutex is unlocked to make `std::cout` available to the other thread.

**Note:** These examples intentionally omit a stop mechanism and are used for illustration purposes only.

```cpp
std::mutex mutex{};

// Function running in thread 1.
void thread1Func()
{
    while (1)
    {
        // Lock mutex before entering critical region.
        mutex.lock();
        {
            // Critical region - Feel free to access shared resources here.
            // Note: Terminal print is a resource shared between the threads.
            std::cout << "Thread1 currently has access to the terminal!\n";
        }
        // Unlock the mutex to make shared data available for the other thread.
        mutex.unlock();
    }
}

// Function running in thread 2.
void thread2Func()
{
    while (1)
    {
        // Lock mutex before entering critical region.
        mutex.lock();
        {
            // Critical region - Feel free to access shared resources here.
            // Note: Terminal print is a resource shared between the threads.
            std::cout << "Thread2 currently has access to the terminal!\n";
        }
        // Unlock the mutex to make shared data available for other threads.
        mutex.unlock();
    }
}
```

We can also use `std::lock_guard<T>` from `<mutex>` to automatically lock and unlock a mutex:

```cpp
std::lock_guard<std::mutex> lock{mutex};
```

This works as follows:
* The mutex is automatically locked when the lock guard is created.
* The mutex is automatically unlocked when the lock guard is destroyed, i.e., when it goes out of scope.

#### 4. `std::atomic`
Individual variables shared between threads, such as stop flags, can be made atomic instead of being protected by a mutex. This simplifies the implementation because no mutex needs to be created, locked, or unlocked:
* Atomic variables are implemented using the `std::atomic` type from the `<atomic>` header.
* For example, an atomic boolean variable named `stopFlag` can be implemented as:

```cpp
std::atomic<bool> stopFlag{false};
```

To modify and read the stop flag, the methods `store()` and `load()` can be used:

```cpp
// Set the stop flag.
stopFlag.store(true);

// Check the stop flag.
const bool stop{stopFlag.load()};
```

Atomic stop flags are commonly used to stop multiple threads simultaneously.

**Note:**
* An atomic variable only protects a single variable.
* If multiple variables must be updated together, a mutex is required, even if each individual variable is atomic.
* A mutex also provides memory synchronization between threads, ensuring that writes performed within a critical section become visible to other threads once the lock is released.

---

### Example: Mutex with Lock Guard and Atomic Stop Flag
The example in the original appendix demonstrates a typical embedded scenario:
* A transmitter thread (TX) periodically produces messages.
* A receiver thread (RX) reads and consumes new data.
* A third thread stops program execution after a specified timeout.


``` cpp
#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>

namespace
{
/**
 * @brief Shared memory used by TX and RX threads.
 *
 *        Access must be protected by mutex to avoid data races.
 */
struct SharedMem
{
    /** Shared data. */
    std::uint16_t data{};

    /** Indicate that new data is available (protected by a mutex, hence not atomic). */
    bool newData{false};
};

/** Mutex to protect critical regions. */
std::mutex mutex{};

// -----------------------------------------------------------------------------
void wait_ms(const std::size_t ms) noexcept
{
    // Sleep the calling thread for the given duration.
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// -----------------------------------------------------------------------------
void txThread(SharedMem& shared, const std::atomic<bool>& stop,
              const std::size_t txInterval_ms) noexcept
{
    std::uint16_t counter{};

    // Run the TX loop as long as the stop flag isn't set.
    while (!stop.load())
    {
        // Enter critical section, lock the mutex to protect shared memory.
        {
            std::lock_guard<std::mutex> lock{mutex};

            // Produce new data and mark it as available.
            shared.data    = counter++;
            shared.newData = true;
            std::cout << "TX: Produced " << shared.data << "!\n";
        }
        wait_ms(txInterval_ms);
    }
}

// -----------------------------------------------------------------------------
void rxThread(SharedMem& shared, const std::atomic<bool>& stop,
              const std::size_t rxInterval_ms) noexcept
{
    // Run the RX loop as long as the stop flag isn't set.
    while (!stop.load())
    {
        // Enter critical section, lock the mutex to protect shared memory.
        {
            std::lock_guard<std::mutex> lock{mutex};

            // Consume new data if available.
            if (shared.newData)
            {
                std::cout << "RX: Consumed " << shared.data << "!\n";
                shared.newData = false;
            }
        }
        wait_ms(rxInterval_ms);
    }
}

// -----------------------------------------------------------------------------
void stopThread(std::atomic<bool>& stop, const std::size_t timeout_ms) noexcept
{
    // Wait for timeout, then signal all threads to stop.
    wait_ms(timeout_ms);
    stop.store(true);
}
} // namespace

// -----------------------------------------------------------------------------
int main()
{
    constexpr std::size_t txInterval_ms{1000U};
    constexpr std::size_t rxInterval_ms{100U};
    constexpr std::size_t threadTimeout_ms{10000U};

    // Shared memory structure.
    SharedMem sharedMem{};

    // Atomic stop flag shared between threads.
    // Atomic is sufficient here, since it protects a single variable.
    std::atomic<bool> stopFlag{false};

    // Create and run threads during ten seconds to simulate communication.
    std::thread t1{txThread, std::ref(sharedMem), std::cref(stopFlag), txInterval_ms};
    std::thread t2{rxThread, std::ref(sharedMem), std::cref(stopFlag), rxInterval_ms};
    std::thread t3{stopThread, std::ref(stopFlag), threadTimeout_ms};

    // Synchronize the threads.
    t1.join();
    t2.join();
    t3.join();
    return 0;
}
```

---

### Why Is a Mutex Needed?
`SharedMem` contains resources shared between the TX and RX threads that belong together:
* `data` – Data shared between the threads.
* `newData` – Indicates that new data is available.

If TX and RX execute simultaneously without a mutex, RX may read the data while it is being updated.  
A mutex ensures that both fields are updated completely before another thread is allowed to read them.

---

### Rule of Thumb
* `std::atomic` is suitable for individual variables, such as stop flags.
* `std::mutex` is required when multiple resources, such as variables, are shared between threads.

---

## Summary
In this appendix, we have seen:
* How to create threads using `std::thread`.
* How to protect shared data using `std::mutex`.
* How to use `std::atomic` correctly.
* The difference between atomic variables and mutexes.

These concepts form the foundation of safe parallel programming in modern C++.

---