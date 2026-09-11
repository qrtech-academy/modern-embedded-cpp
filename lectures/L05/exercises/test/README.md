# L05 Test Suite

Unit tests for everything [Appendix D](../../appendix/d_exercises.md) asks you to write, using the
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

Each exercise set with a program is a separate program, so each is tested by a separate binary,
built only when the file the set asks for exists:

| Binary | Built when | Tests |
|---|---|---|
| `language` | **always** | Claims Appendices A to C make about templates, checked by the compiler |
| `function_template` | `function_template/main.cpp` exists | `clear` and `toggle`, and the program's output |
| `class_template` | `class_template/include/driver/timer/timer.hpp` exists | Both timers, and the program once `source/main.cpp` exists too |

Exercise Set 2 and Exercise 3.2 ask questions to answer in prose. They have no binary: the
answers are the subject of [Appendix B](../../appendix/b_type_traits_and_constraints.md) and
[Appendix C](../../appendix/c_class_templates.md), and the reference answers to 3.2 are in the
[solutions README](../../appendix/solutions/README.md).

A set whose file does not exist yet is reported as `SKIP`, with the path the suite is waiting for,
so a file written under the wrong name is visible rather than silently untested.

**`language` always runs, and that is not decoration.** `runAllTests()` reports failure when it
has no tests to run, so a suite whose every test waited for your files would be red on a fresh
clone, and look broken when it was merely empty. Something has to run from the first day, and it
may as well be worth knowing: that `add(1, 2)` and `add(1.0, 2.0)` are two instantiations returning
two types, that `std::is_arithmetic` lets a `float` through where `std::is_integral` does not, that a
full specialization wins over the primary template for its own argument, and that a value in a
template's argument list is part of the type.

---

## How the tests reach code in your `main.cpp`

Exercise Set 1 puts its templates in `main.cpp`, in an anonymous namespace, which nothing outside
that file can see. So the tests include your file:

```cpp
#define main functionTemplateMain
#include "main.cpp"
#undef main
```

That makes `clear()` and `toggle()` callable from the test. Renaming `main` has one side effect:
`main` is the only function allowed to leave out its `return`, so the test silences that one warning
for your file.

**Your program is run as the program it is.** The suite also builds it on its own, with its own
`main()`, and the test runs it as a separate process, the way you would: it must end normally,
returning 0, and print exactly the expected output. A crash or a non-zero exit status is reported
as one. The renamed `main` in the included copy is never called, and could not safely be: only the
real `::main` may leave out its `return`, and renamed, the same code falls off the end of an
ordinary function, which GCC compiles into a trap.

The timer program of Exercise Set 3 is run the same way, 1 ms sleeps and all, so its test takes
about two seconds.

---

## What the tests check

**The output, exactly.** Where an exercise gives expected output, the test compares what your code
prints with it, character for character, and reports the first line that differs. Every message the
timers print is checked the same way, one step at a time: creation, `start()`, `stop()`,
`toggle()` and destruction.

**The specialization, not just the interface.** Every timer test runs twice, on
`Timer<Type::Stub>` and on `Timer<Type::Stm32>`, and the two are told apart by what they print. A
header without the specialization still compiles, because `Timer<Type::Stm32>` then instantiates the
primary template, which is exactly what Exercise 3.2 e) asks about; the tests fail it, because that
timer prints `stub` where it should print `STM32`.

**The declarations.** Each timer must be `final`, have an `explicit` `noexcept` constructor, and be
neither default constructible, copyable nor movable; the queries must be callable on a `const`
timer, and every method must be `noexcept`. These are checked with type traits, so a missing
`explicit` is a failed test with a message rather than a compile error. `clear()` and `toggle()` must
be `constexpr`, so the test runs them at compile time; a template that is merely correct fails to
compile there, and the compiler's message says why.

**Wide registers and wide timeouts, which the exercises do not use.** `clear()` and `toggle()` are
tested on 16-, 32- and 64-bit registers as well as the 8-bit one in the exercise, because `1 << 40`
is undefined for an `int` and `static_cast<T>(1) << 40` is what gets it right. The timers are tested
with a 60000 ms timeout, which a counter narrower than the specified `std::uint16_t` can never reach.

**What must not compile.** Exercises 1.1 and 1.2 ask for a `static_assert` with `std::is_integral`.
`function_template_rejects_float_clear.cpp` calls `clear()` on a `float`, and
`function_template_rejects_float_toggle.cpp` calls `toggle()` on a `double`; the suite checks that
each fails to compile *because of a static assertion*. Writing `std::is_arithmetic` instead lets the
floating-point type through to the bit operation, which then fails for another reason, and the suite
reports exactly that.

**What the exercise leaves open is not tested.** Whether `start()` prints when the timer is already
running, or `stop()` when it is already stopped, is not specified, so the tests only check the
messages for calls that change the state, and for a timer destroyed while stopped only that its last
message is the destructor's.

---

## Adding a test

Add a `TEST` to the set's `<binary>_test.cpp`, then check it against the reference solution with
`make SOLUTIONS=1` before relying on it. A new binary is a new entry in the [Makefile](./Makefile)'s
table; how the table is used is in [`ci/suite.mk`](../../../../ci/suite.mk).
