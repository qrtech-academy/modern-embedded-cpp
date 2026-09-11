# L04 Test Suite

Unit tests for everything [Appendix C](../../appendix/c_exercises.md) asks you to write, using the
[QAcademy Test](https://github.com/qrtech-academy/test-framework) framework, checked out as the
submodule `libs/test`. Run `git submodule update --init` once if that directory is empty.

```bash
make                    # Build and run the suite against your code in ../
make SOLUTIONS=1        # The same tests, against ../../appendix/solutions
make clean              # Remove the test binaries.
```

`make test` at the repository root runs this suite and every other lecture's.

---

## What runs, and when

Each exercise set is tested by a separate binary, built only when the files the set asks for
exist:

| Binary | Built when | Tests |
|---|---|---|
| `language` | **always** | What Appendices A and B say about owning an object, checked |
| `exercise1` | `raw_factory/include/driver/serial/{interface,stub,esp32s3}.hpp` exist | The serial interface and its two drivers |
| `exercise2` | `raw_factory/include/driver/factory/{interface,esp32s3,stub}.hpp` exist | The factory interface and the two factories |
| `exercise3` | `raw_factory/include/app/logic/logic.hpp` exists | `app::logic::Logic` owning a raw pointer, and `main.cpp` once it exists |
| `exercise4` | `smart_factory/include/driver/factory/interface.hpp` exists | The same, with `std::unique_ptr`: the factories and `Logic` as their files appear |

A set whose files do not exist yet is reported as `SKIP`, with the path the suite is waiting for,
so a file written under the wrong name is visible rather than silently untested.

**`language` always runs**, because `runAllTests()` reports failure when it has no tests, and a
suite that is red on a fresh clone looks broken when it is merely empty. It holds the appendices'
claims about ownership: that `delete` through an interface destroys the derived object because the
destructor is virtual, that a `std::unique_ptr` can be moved but not copied, that moving one leaves
the source empty, that it destroys its object exactly once when it goes out of scope, and that
`std::make_unique<Derived>()` converts to a `std::unique_ptr` to the interface, which is how a
factory returns a driver.

---

## What the tests check

**The declarations, as types.** A method's exact signature, `const` and `noexcept` included, is part
of its type, so the tests compare member function pointer types: `write()` must be exactly
`void (Interface::*)(std::uint8_t) noexcept`, and Set 4's `serial()` must return
`std::unique_ptr<driver::serial::Interface>`. A mismatch fails one named test. Deleted copy and move
operations, `final`, and virtual destructors are checked with type traits, and `explicit` on the
ESP32-S3 driver's constructor by checking that `Esp32s3 serial = {17U, 18U};` does not compile.

**The output, exactly.** `driver::serial::Esp32s3::write()` must print
`Transmitting byte 0xFF via TX pin 17!`, as in the exercise. A second byte and pin make sure neither
is hard-coded, and that it is the transmit pin that is printed.

**Ownership, by counting.** To test `Logic`, the tests give it a factory of their own, which
records what it was asked for and creates drivers that count their own destruction. So a `Logic`
that asks for its driver more than once, with the wrong pins, never deletes it, or deletes it twice,
fails with a number. Constructing and destroying the `Logic` happens in a child process, because a
driver deleted twice usually crashes: the test then fails with the signal and a question
(*is its driver deleted twice?*), and the tests after it still run.

**`run()`, from outside.** `run()` never returns, so the tests cannot call it and wait.
[`run_probe.hpp`](./run_probe.hpp) runs it in a child process with a serial driver that reports
every call through a pipe, watches for 450 ms, then kills it. That is long enough to see what
Exercises 3.1 d) and 4.3 d) ask for: bytes 0, 1, 2, ... in order, a read after every write, four or
five writes rather than thousands (the 100 ms delay), and one printed line per received byte and
none otherwise. What `run()` prints is not compared, because the exercise does not say what it
should look like.

**What is not tested.** That `isInitialized()` and `serial()` are `[[nodiscard]]`, and that `read()`
is not: GCC does not warn about a discarded result of a virtual call made through an interface,
with the attribute or without it, so no test can tell them apart. `main.cpp` is compiled against
your headers but never run, because it never stops; run it yourself.

---

## Adding a test

Add a `TEST` to the set's `exerciseN_test.cpp`, then check it against the reference solution with
`make SOLUTIONS=1` before relying on it. A new binary is a new entry in the [Makefile](./Makefile)'s
table; how the table is used is in [`ci/suite.mk`](../../../../ci/suite.mk).
