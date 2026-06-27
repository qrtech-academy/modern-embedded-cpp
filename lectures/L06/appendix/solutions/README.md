# Exercise solutions

Reference implementations for the exercises in [Appendix B](../b_exercises.md).

**Note!** Try to solve the exercises yourself before looking at the solutions.

---

## Solutions
* **Exercise Set 1**:
    * [exercise1.cpp](./exercise1/exercise1.cpp):
        * Worker thread with atomic stop flag.
* **Exercise Set 2**:
    * [exercise2.cpp](./exercise2/exercise2.cpp):
        * Thread-safe atomic counter.
* **Exercise Set 3**:
    * [exercise3.cpp](./exercise3/exercise3.cpp):
        * Shared memory protected by mutex with TX and RX threads.
* **Exercise Set 4-5**:
    * [driver/counter/interface.h](./exercise4-5/include/driver/counter/interface.h):
        * Pure virtual interface `driver::counter::Interface`.
    * [driver/counter/stub.h](./exercise4-5/include/driver/counter/stub.h):
        * Thread-safe stub counter `driver::counter::Stub` with atomic initialization state.
    * [main.cpp](./exercise4-5/source/main.cpp):
        * Demonstration and test program.
* **Exercise Set 6**:
    * [exercise6.cpp](./exercise6/exercise6.cpp):
        * TX/RX threads with condition variable replacing polling.
* **Exercise Set 7**:
    * [exercise7.cpp](./exercise7/exercise7.cpp):
        * Asynchronous firmware validation using `std::async` and `std::future`.

---

## Exercise Set 1 – Threads and atomic stop flag
`workerThread()` takes a print speed and an atomic stop flag. It prints a message at the given interval until the stop flag is set. `main()` starts the thread, sleeps for a timeout, sets the flag, and joins.

---

## Exercise Set 2 – Shared counter with data race
`incrementCounter()` takes a counter and an iteration count. Two threads increment the same counter concurrently. The solution progresses through three versions:
* A plain `std::uint32_t` that demonstrates the data race.
* A mutex-protected version that fixes it.
* An `std::atomic<std::uint32_t>` that achieves the same result without a mutex.

---

## Exercise Set 3 – Shared data protected by mutex
`SharedMem` holds a `std::uint16_t` data value and a `newData` flag. `txThread()` produces incrementing values and sets `newData`; `rxThread()` polls for new data and consumes it. Both threads share one mutex that protects all accesses to `SharedMem`. `main()` runs both threads for a fixed timeout, then sets the stop flag and joins.

---

## Exercise Sets 4–5 – Thread-safe stub driver
`driver::counter::Interface` defines a pure virtual interface with `isInitialized()`, `value()`, `increment()`, and `reset()`.

`driver::counter::Stub` implements the interface:
* `myMutex` (`mutable std::mutex`) protects `myValue`.
* `myInitialized` (`std::atomic<bool>`) guards all counter operations — each method returns early if the driver is not initialized.
* Copy and move are deleted.

`main()` creates one `Stub`, starts two threads via `counterThread()` (100 and 200 increments respectively), joins them, and prints the final value — expected `300`.

---

## Exercise Set 6 – Condition variable
Built on Exercise Set 3. `rxThread()` no longer polls with a sleep; instead it acquires a `std::unique_lock` and calls `cv.wait()` with `hasNewData()` as the predicate. `txThread()` calls `cv.notify_one()` after releasing the lock. `main()` calls `cv.notify_all()` after setting the stop flag so the receiver wakes up and exits cleanly.

---

## Exercise Set 7 – std::async and std::future
`validateFirmware()` simulates a slow flash read by sleeping for `2000 ms`, then iterates over the buffer looking for `0xFFU`. It is launched asynchronously via `std::async(std::launch::async, ...)`. `main()` polls `future.wait_for(std::chrono::milliseconds(200U))` against `std::future_status::ready` in a loop, printing a status message each iteration, then retrieves the result with `future.get()`.

---

## Answers to reflection questions

### Exercise 1.2
**What happens if `join()` is not called?**
* The `std::thread` object is destroyed while still joinable, which calls `std::terminate()` and crashes the program.
* The worker thread may not finish executing before the process exits.

**Which thread prints first, the main thread or the worker thread?**
* Neither is guaranteed to print first.
* Both threads are ready to execute as soon as the worker thread is created, and the OS scheduler decides which runs first.

**Is the print order guaranteed?**
* No. Thread scheduling is non-deterministic, so the order may differ between runs and between platforms.

---

### Exercise 1.3
**Why is `std::atomic<bool>` suitable for this stop flag?**
* It guarantees that reads and writes are indivisible and visible across threads without a data race.
* No additional synchronization primitive is needed for a single boolean flag that is written by one thread and read by another.

**Would a normal `bool` be safe here?**
* No. Concurrent access to a plain `bool` from multiple threads without synchronization is undefined behavior.
* The compiler or CPU may also cache the value in a register, causing the worker thread to never see the updated value.

**Why is `load()` used when reading the flag?**
* `load()` makes the atomic read explicit and ensures the value is fetched from the atomic object with the correct memory ordering, not from a stale cached copy.

**Why is `store()` used when writing the flag?**
* `store()` makes the atomic write explicit and ensures the new value is published to all threads with the correct memory ordering.

---

### Exercise 2.1
**What final value do you expect?**
* `200000`, since each of the two threads increments the counter `100000` times.

**Do you always get the expected value?**
* No. Due to the data race the result is typically lower than `200000` and varies between runs.

**Why is this program unsafe?**
* Two threads read, modify, and write the same counter without any synchronization.
* The read-modify-write sequence is not atomic for a plain integer, so one thread can overwrite the other's increment.

**Which condition makes this a data race?**
* Two or more threads access the same memory location concurrently, at least one access is a write, and there is no synchronization between them.

---

### Exercise 2.2
**What final counter value do you get now?**
* Always `200000`.

**Why does the mutex solve the problem?**
* The mutex ensures that only one thread can be inside the critical section at a time, making the read-modify-write sequence effectively atomic.

**Why is `std::lock_guard` safer than manually calling `lock()` and `unlock()`?**
* `std::lock_guard` releases the lock automatically when it goes out of scope, even if an exception is thrown.
* Manual `lock()`/`unlock()` risks forgetting to unlock, or leaving the mutex locked if an exception occurs between the two calls.

---

### Exercise 2.3
**What final counter value do you get?**
* Always `200000`.

**Why is `std::atomic<std::uint32_t>` sufficient in this exercise?**
* Incrementing a single integer is one independent operation. `std::atomic` makes the increment indivisible and thread-safe without needing a mutex.

**Would an atomic counter still be sufficient if several related variables had to be updated together?**
* No. If multiple related variables must be updated as a consistent unit, an atomic type alone cannot protect the compound operation.
* A mutex is needed to ensure the update of all related variables appears atomic to other threads.

---

### Exercise 3.4
**Why must `data` and `newData` be protected by the same mutex?**
* They are logically related — RX must read `data` and `newData` together as a consistent snapshot.
* If they were protected separately, TX could update `data` after RX has already read `newData`, causing RX to act on a stale or mismatched value.

**Why is `newData` not atomic in this design?**
* `newData` must be checked and updated together with `data` as a consistent unit.
* Protecting `newData` with an atomic alone would not prevent TX from updating `data` between RX reading the flag and reading the value.
* The mutex protects both fields together, making the compound operation safe.

**What could happen if TX updated `data` and `newData` without locking the mutex?**
* RX could read `newData = true` before `data` has been written, or read a partially updated `data` value.
* This is a data race and results in undefined behavior.

**Why is the stop flag atomic instead of mutex-protected?**
* The stop flag is a single boolean written by one thread and read by others. No compound operation is needed.
* An atomic boolean is sufficient and avoids the overhead of locking the mutex on every loop iteration just to check a flag.

**Could `std::atomic<SharedMem>` be used instead of a mutex?**
* Not without additional complexity. RX's operation is a read-modify-write sequence: load the struct, check `newData`, clear `newData`, and store back.
* Even with `std::atomic<SharedMem>`, that sequence is not atomic as a whole. TX could store a new value between RX's load and RX's store, causing RX to silently overwrite TX's new data.
* Making it correct without a mutex would require a compare-exchange (CAS) retry loop in RX, which is significantly more complex than the mutex approach.
* The mutex is the right tool here because it makes the entire compound operation safe through mutual exclusion.

---

### Exercise 4.3
**Why does `value()` need to lock the mutex even though it only reads the value?**
* Another thread could be calling `increment()` or `reset()` concurrently.
* Reading `myValue` while it is being written is a data race. The mutex ensures the read is not interleaved with a write.

**Why is `myMutex` marked `mutable`?**
* `value()` is a `const` method and cannot modify member variables.
* Locking a mutex is an implementation detail, not a logical mutation of the object.
* `mutable` allows the mutex to be locked inside a `const` method without violating `const` correctness.

**What would happen if `increment()` did not lock the mutex?**
* Two threads could read the same value, both increment it, and write back the same result, losing one increment.
* The counter would end up lower than expected — identical to the data race in Exercise 2.1.

**Could `std::atomic<std::uint32_t>` be used instead in this specific class?**
* Yes. In this specific class each operation (increment, read, reset) acts on a single variable independently, so `std::atomic<std::uint32_t>` would be sufficient.

**Why might a mutex still be preferable in a more complex driver?**
* If future requirements add more member variables that must be updated together with `myValue`, a mutex can protect all of them as a unit.
* An atomic can only protect a single variable; compound multi-variable operations still require a mutex.

---

### Exercise 5.3
**Why is `myInitialized` suitable as an atomic variable?**
* It is a single boolean flag that is read and written independently.
* No other variable needs to be updated in the same atomic step, so `std::atomic<bool>` is sufficient.

**Why is `myValue` still protected by a mutex?**
* `myValue` is accessed by multiple operations (`increment`, `reset`, `value`) that must not interleave.
* A mutex protects all operations on `myValue` as a group, preventing data races.

**Would it be safe to make both variables atomic if future requirements added more related state?**
* No. If future state must be updated together with `myValue` as a consistent unit, atomic types alone cannot protect the compound update.
* A mutex would be needed to keep the update of all related variables atomic as a group.

**What is the main design difference between a thread-safe flag and thread-safe shared data?**
* A thread-safe flag is a single boolean that only needs to be observed independently — an atomic is sufficient.
* Thread-safe shared data involves multiple related fields that must be read or written together as a consistent snapshot, requiring a mutex to protect the compound operation.

**What could happen if `setInitialized(false)` is called between the initialization check and the mutex lock in `increment()`? How would you fix this?**
* `increment()` would pass the initialization check, then the driver would be marked uninitialized by another thread, and `increment()` would proceed to modify `myValue` — violating the intended guard.
* The fix is to check `myInitialized` inside the mutex lock so that the flag check and the counter update are protected together as an atomic unit.

---

### Exercise 6.1
**Why is `std::unique_lock` required here instead of `std::lock_guard`?**
* `cv.wait()` must unlock the mutex while the thread is sleeping and re-lock it when woken up.
* `std::lock_guard` has no way to release and re-acquire the lock — it only supports locking on construction and unlocking on destruction.
* `std::unique_lock` provides `lock()` and `unlock()`, which `cv.wait()` uses internally.

**What is a spurious wake-up, and how does the predicate protect against it?**
* A spurious wake-up is when `cv.wait()` returns even though no `notify_one()` or `notify_all()` was called, due to OS implementation details.
* The predicate re-checks the condition after every wake-up. If the condition is not met, `wait()` automatically re-enters the waiting state instead of proceeding.

**Why must the stop flag also be checked inside the predicate?**
* When the stop flag is set and `notify_all()` is called, the receiver wakes up.
* Without `stop.load()` in the predicate, the receiver would see `hasNewData() == false`, treat it as a spurious wake-up, and go back to sleep — never exiting cleanly.

**How does this design differ from the polling approach in Exercise 3.3?**
* In Exercise 3.3 the receiver wakes every `100 ms` regardless of whether data is available, wasting CPU cycles and adding up to `100 ms` of latency.
* With a condition variable the receiver sleeps until TX explicitly wakes it, reducing latency to near zero and eliminating unnecessary polling.

**What would happen if `notify_all()` was not called after setting the stop flag?**
* The receiver thread would remain blocked on `cv.wait()` even after `stop` is set to `true`, because nothing wakes it up.
* The main thread would then block forever on `join()`, and the program would hang.

**Why is `notify_all()` used here instead of `notify_one()`?**
* In this design there is only one receiver thread, so `notify_one()` would be sufficient here.
* `notify_all()` is used for correctness in the general case — if multiple threads were waiting on the same condition variable, `notify_one()` would only wake one of them, leaving the others blocked indefinitely.

---

### Exercise 7.1
**What does `std::launch::async` guarantee compared to omitting the launch policy?**
* `std::launch::async` guarantees the function runs in a new thread immediately, independently of when `get()` is called.
* Without the policy, the implementation may choose `std::launch::deferred`, which defers execution until `get()` is called — running the function synchronously on the calling thread instead.

**At what point does the main thread block?**
* The main thread blocks when `future.get()` is called.
* Everything between `std::async` and `future.get()` executes concurrently while validation runs in the background.

**What would happen if `future.get()` was called immediately after `std::async`?**
* The main thread would block immediately, waiting for `validateFirmware()` to complete before continuing.
* No concurrent work would happen — it would behave like a synchronous function call.

---

### Exercise 7.2
**How does `wait_for()` differ from `get()`?**
* `wait_for()` waits at most for the given duration and returns a status (`ready`, `timeout`, or `deferred`) without consuming the result or blocking indefinitely.
* `get()` blocks until the result is available and then returns or re-throws it, consuming the future.

**Is it safe to call `get()` after `wait_for()` returns `std::future_status::ready`?**
* Yes. Once `wait_for()` returns `std::future_status::ready` the result is guaranteed to be available, and `get()` will return immediately without blocking.

---
