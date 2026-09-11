# L06 Test Suite

Unit tests for everything [Appendix B](../../appendix/b_exercises.md) asks you to write, using the
[QAcademy Test](https://github.com/qrtech-academy/test-framework) framework, checked out as the
submodule `libs/test`. Run `git submodule update --init` once if that directory is empty.

```bash
make                    # Build and run the suite against your code in ../
make SOLUTIONS=1        # The same tests, against ../../appendix/solutions
make clean              # Remove the test binaries.
```

`make test` at the repository root runs this suite and every other lecture's. Every binary here
starts threads, so every one is built with `-pthread`.

---

## What runs, and when

Each exercise set is a separate program, so each is tested by a separate binary, built only when
the file the set asks for exists:

| Binary | Built when | Tests |
|---|---|---|
| `language` | **always** | Claims Appendix A makes about threads, mutexes, atomics and futures |
| `exercise1` | `exercise1/exercise1.cpp` exists | `workerThread` and its stop flag |
| `exercise2` | `exercise2/exercise2.cpp` exists | `incrementCounter` on an atomic counter, and the program |
| `exercise3` | `exercise3/exercise3.cpp` exists | `SharedMem`, `txThread` and `rxThread` |
| `exercise4-5` | both headers in `exercise4-5/include/driver/counter/` exist | The interface, the thread-safe stub, and, once `source/main.cpp` exists, `counterThread` and the program |
| `exercise6` | `exercise6/exercise6.cpp` exists | `hasNewData`, and the condition variable threads |
| `exercise7` | `exercise7/exercise7.cpp` exists | `validateFirmware`, and the polling program |

A set whose file does not exist yet is reported as `SKIP`, with the path the suite is waiting for,
so a file written under the wrong name is visible rather than silently untested.

**Each binary tests where its set ends up.** Exercise 1.3 replaces 1.1's `printCount` with a stop
flag, Exercise 2.3 replaces 2.1's plain counter with an atomic one, and Exercise Set 5 adds
`setInitialized()` to Set 4's stub; the tests check the final version. A stub that does not have
`setInitialized()` yet still compiles: that one test says what is missing instead.

**`language` always runs, and that is not decoration.** `runAllTests()` reports failure when it
has no tests to run, so a suite whose every test waited for your files would be red on a fresh
clone. So it holds Appendix A's statements in forms that are guaranteed however the threads are
scheduled: `std::ref` hands a thread a reference, a joined thread is no longer joinable, atomic
increments from four threads sum exactly, a lock guard releases its mutex at the end of its scope,
`std::launch::deferred` runs the task on the calling thread and `std::launch::async` does not.

---

## Invariants, not interleavings

Two threads print in whatever order the scheduler picks, and a test that expected one particular
order would pass or fail by luck. So no test here compares interleaved output. Each checks what a
correct program **guarantees**, however it is scheduled:

* **Counts come out exact.** Eight threads of 20000 increments on the stub end at 160000, every
  run, and a reader calling `value()` meanwhile never sees the count go down.
* **Threads stop when asked.** A worker, a receiver or a transmitter must return promptly once
  the stop flag is set, and the condition variable receiver is stopped fifty times over, at
  different moments, because a lost wakeup is a matter of timing.
* **What is sent is received, once.** The receiver prints exactly one line for each value the
  transmitter publishes, and clears `newData` when it has consumed it.
* **Timing gives bounds, not values.** A worker that sleeps `printSpeed_ms` between prints cannot
  print more than once per interval, and a program that polls a future every 200 ms cannot report
  "still waiting" more often than that. Neither test says how many lines there must be, only how
  many there cannot be, which catches a missing sleep without depending on the scheduler.

Your `main()` functions sleep for three or ten seconds, so the tests do not run them; they start
the threads themselves, and stop them the way the exercises say `main()` must. The exceptions are
Sets 2, 4 and 7, whose programs finish quickly and print a result the tests can check.

**No test can hang the suite.** A failed check throws, and destroying a `std::thread` that was
never joined terminates the program, so the tests check nothing while a thread is running: they
record what they see, stop and join, and only then compare. A thread that ignores its stop flag
is given until a deadline, reported, and then nudged with new data until it does finish; one that
never finishes at all is stopped by the suite's 60-second limit.

---

## What the tests cannot see

**A missing lock is a data race, and a data race may happen to give the right answer.** A receiver
that reads `shared.newData` without locking the mutex passes these tests: nothing it does is wrong
often enough to observe. A stub whose `increment()` forgets the mutex usually loses thousands of
increments here and fails, but "usually" is all a test can promise about undefined behaviour.

The tool that sees races is ThreadSanitizer. To run one binary under it, from this directory:

```bash
g++ -std=c++17 -g -pthread -fsanitize=thread -I../../../../libs/test/include \
    -I../../../../libs/support/include -I../exercise4-5/include -I../exercise4-5/source \
    exercise4-5_test.cpp ../../../../libs/support/source/testsuite.cpp \
    -L../../../../libs/test -lqacademy_test -o tsan && ./tsan
```

If it fails with "unexpected memory mapping", run it as `setarch $(uname -m) -R ./tsan`.

**What only `main()` does is not tested.** Exercise 6.1 asks `main()` to set the stop flag while
holding the mutex before calling `notify_all()`. The tests stop the receiver that way themselves;
whether your `main()` does is for you and the reflection questions.

---

## How the tests reach code in your programs

Every set except 4 and 5 puts its functions in one `.cpp` file, some in an anonymous namespace,
which nothing outside that file can see. So the tests include your file:

```cpp
#define main exercise3Main
#include "exercise3.cpp"
#undef main
```

That makes `rxThread()` callable from the test, and your program runnable as `exercise3Main()`.
Renaming `main` has one side effect: `main` is the only function allowed to leave out its
`return`, so the test silences that one warning for your file.

---

## Adding a test

Add a `TEST` to the set's `exerciseN_test.cpp`, then check it against the reference solution with
`make SOLUTIONS=1`, **many times over**: a concurrency test that passes once has proved very
little. `threads.hpp` has the helpers for waiting on a condition and joining a thread against a
deadline. A new binary is a new entry in the [Makefile](./Makefile)'s table; how the table is
used is in [`ci/suite.mk`](../../../../ci/suite.mk).
