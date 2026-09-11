# L02 Test Suite

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

Each exercise set is a separate program, so each is tested by a separate binary, built only when
the files the set asks for exist:

| Binary | Built when | Tests |
|---|---|---|
| `language` | **always** | Claims Appendices A and B make about classes, checked by the compiler |
| `exercise1` | `exercise1/include/driver/gpio/led.hpp` exists | `Led`; `Button` once `button.hpp` exists; the program once `main.cpp` does too |
| `exercise2` | `exercise2/main.cpp` exists | The program's output: the LED following the button, then blinking |
| `exercise3` | `exercise3/include/driver/buzzer.hpp` and `exercise3/source/driver/buzzer.cpp` exist | `Buzzer`, and the program once `source/main.cpp` exists |
| `exercise4` | `exercise4/include/driver/timer.hpp` and `exercise4/source/driver/timer.cpp` exist | `Timer`, and the program once `source/main.cpp` exists |

A set whose files do not exist yet is reported as `SKIP`, with the path the suite is waiting for,
so a file written under the wrong name is visible rather than silently untested. Exercise Sets 1
and 2 are separate binaries even though they use the same classes: each has its own copy of
`led.hpp` and `button.hpp`, and one binary holding two different definitions of
`driver::gpio::Led` would be an ODR violation rather than a test.

**`language` always runs, and that is not decoration.** `runAllTests()` reports failure when it
has no tests to run, so a suite whose every test waited for your files would be red on a fresh
clone, and look broken when it was merely empty. It holds the appendices' claims about classes, in
a form the compiler can confirm or the running program can show: that a struct's members are
public and a class's private by default, that `ledPrint(8U)` compiles and prints an LED that was
never created until the constructor is made `explicit`, that a static member counts instances
across every object, that a `static constexpr` member can size an array, that a `const` member
rules out assignment but not copying, and that `std::move()` falls back to copying in a class that
declares a destructor, because such a class gets no move constructor from the compiler.

---

## How the tests reach code in your `main.cpp`

The programs' output is part of every exercise, so the tests include your `main.cpp`:

```cpp
#define main exercise1Main
#include "main.cpp"
#undef main
```

That makes your program runnable as `exercise1Main()`, whose output the test compares with the
exercise's example output. For Exercise Sets 3 and 4 the suite adds `source/` to the include path,
so the same line finds `source/main.cpp`, and compiles every other file in `source/driver/` into
the test. Renaming `main` has one side effect: `main` is the only function allowed to leave out its
`return`, so the test silences that one warning for your file.

---

## What the tests check

**The declarations, not just the behaviour.** Most of what the exercises ask for is a declaration:
`explicit`, `noexcept`, `final`, a destructor that is `default`, deleted copy and move operations.
Each is visible to the compiler, so the tests ask it, with type traits such as
`std::is_convertible<std::uint8_t, Led>` (false once the constructor is `explicit`) and
`noexcept(led.on())`. A class that behaves correctly but leaves one out fails.

**A defaulted destructor, not merely an empty one.** `~Led() noexcept = default;` leaves `Led`
trivially destructible; `~Led() noexcept {}` does not, though it does nothing either. The test
checks `std::is_trivially_destructible`, which is how "mark it `default`" becomes something a test
can see.

**Deleted operations, as the compiler sees them.** The tests check that the class cannot be
default-constructed, copied or moved, whichever way you arrange it. Deleting any one of the move
operations already makes the compiler delete the implicit copy constructor, so a class that forgets
one deletion but keeps another may still pass; one that forgets them all does not.

**The output, exactly.** Where an exercise gives example output, the test compares what your code
prints with it, character for character, and reports the first line that differs. That includes
the destructors: the buzzer's release message with its own pin, and the timer's
`Stopping timer before deletion!`, which a running timer prints and a stopped one must not.

**The edges the specification names.** A timer created with a timeout of 0 is not initialized, and
`start()` and `toggle()` must leave it stopped. A running timer times out on exactly its
timeout-th tick, and `hasTimedOut()` resets the counter, so the next timeout is a whole timeout
later.

---

## Adding a test

Add a `TEST` to the set's `exerciseN_test.cpp`, then check it against the reference solution with
`make SOLUTIONS=1` before relying on it. A new binary is a new entry in the [Makefile](./Makefile)'s
table; how the table is used is in [`ci/suite.mk`](../../../../ci/suite.mk).
