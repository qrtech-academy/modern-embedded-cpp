# L01 Test Suite

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
the file the set asks for exists:

| Binary | Built when | Tests |
|---|---|---|
| `language` | **always** | Claims Appendix B makes about C++, checked by the compiler |
| `exercise1` | `exercise1/main.cpp` exists | `debug::log` and `app::delay_ms` |
| `exercise2` | `exercise2/driver/timer.hpp` exists | `driver::Timer`, and the program once `main.cpp` exists too |
| `exercise3` | `exercise3/main.cpp` exists | `swap`, and the program's output |
| `exercise4` | `exercise4/main.cpp` exists | `clear` and `toggle`, and the program's output |

A set whose file does not exist yet is reported as `SKIP`, with the path the suite is waiting for,
so a file written under the wrong name is visible rather than silently untested.

**`language` always runs, and that is not decoration.** `runAllTests()` reports failure when it
has no tests to run, so a suite whose every test waited for your files would be red on a fresh
clone, and look broken when it was merely empty. Something has to run from the first day, and it
may as well be worth knowing: that a `constexpr` function can size an array, that `auto x{10}` is
an `int` in C++17, that a template is instantiated once per type, and that `std::size_t` is another
name for a fixed-width type, which is why Appendix B's `isUnsigned` must not specialize both.

---

## How the tests reach code in your `main.cpp`

Exercise Sets 1, 3 and 4 put their functions in `main.cpp`, some in an anonymous namespace, which
nothing outside that file can see. So the tests include your file:

```cpp
#define main exercise3Main
#include "main.cpp"
#undef main
```

That makes `swap()` callable from the test, and your program runnable as `exercise3Main()`, whose
output the test compares with the exercise's example output. Renaming `main` has one side effect:
`main` is the only function allowed to leave out its `return`, so the test silences that one
warning for your file.

---

## What the tests check

**The output, exactly.** Where an exercise gives example or expected output, the test compares
what your code prints with it, character for character, and reports the first line that differs.
`Sensor failure, log level = 2` and `Sensor failure, level: 2` both include the level; only the
first is what the exercise asks for.

**The declarations, not just the behaviour.** `swap()` is declared `constexpr`, so the test calls
it at compile time; a `swap()` that is merely correct fails to compile there, and the compiler's
message says why. The same goes for `noexcept` wherever an exercise asks for it.

**Wide registers, which the exercise does not use.** `clear()` and `toggle()` are tested on 32- and
64-bit registers as well as the 8-bit one in the exercise, because `1U << 40` is undefined for a
32-bit `unsigned` and `static_cast<T>(1U) << 40` is what gets it right. An implementation that
passes the exercise's example with `1U << bit` fails here, by wiping the upper half of the
register.

**What must not compile.** Exercise 4.1 asks for a `static_assert` that rejects a non-integral
register. `exercise4_rejects_float.cpp` calls `clear()` on a `float`, and the suite checks that it
fails to compile *because of a static assertion*, rather than compiling, or failing for some other
reason such as `&=` on a `float`, which would also stop the build but tell the reader nothing.

---

## Adding a test

Add a `TEST` to the set's `exerciseN_test.cpp`, then check it against the reference solution with
`make SOLUTIONS=1` before relying on it. A new binary is a new entry in the [Makefile](./Makefile)'s
table; how the table is used is in [`ci/suite.mk`](../../../../ci/suite.mk).
