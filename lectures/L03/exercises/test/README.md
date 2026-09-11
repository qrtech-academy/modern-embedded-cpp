# L03 Test Suite

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

## One program, four binaries

All four exercise sets in this lecture build one program, so the workspace is `../` itself, laid
out like `../../appendix/solutions`: `include/` and `source/` beside this directory. Each set is
still tested by a binary of its own, built as soon as that set's files exist, so the interface can
be tested before any driver is written:

| Binary | Built when | Tests |
|---|---|---|
| `language` | **always** | Claims Appendices A and B make about inheritance and interfaces |
| `exercise1` | `include/driver/serial/interface.hpp` exists | The interface, through a test double |
| `exercise2` | `stub.hpp` and `stub.cpp` exist | The stub driver |
| `exercise3` | `console.hpp` and `console.cpp` exist | The console driver, a singleton |
| `exercise4` | `source/main.cpp` and both drivers' sources exist | `sendMessage()`, and the program |

A set whose files do not exist yet is reported as `SKIP`, with the path the suite is waiting for,
so a file written under the wrong name is visible rather than silently untested.

**`language` always runs, and that is not decoration.** `runAllTests()` reports failure when it
has no tests to run, so a suite whose every test waited for your files would be red on a fresh
clone. It may as well hold something worth knowing: that a class with a pure virtual method is
abstract, that a virtual destructor is what makes deleting through a base pointer destroy the
derived object, that public inheritance makes a `Derived*` a `Base*` and private inheritance does
not, and that `final` is visible to the compiler.

---

## What the tests check

**An interface, through a test double.** An interface cannot be instantiated, so
`exercise1_test.cpp` implements it: a minimal class overriding each method with the exact
signature Exercise 1.1 specifies. It compiles only if your interface declares those methods, and it
overrides them only if they are virtual. Through it the tests check that the interface is abstract,
that its destructor is virtual (by deleting the double through a pointer to the interface and
counting the destructor calls), and that each method's return type, `const` and `noexcept` are as
specified.

**What a singleton cannot do.** The console is tested as much by what is impossible as by what it
does: it cannot be default-constructed, destroyed, copied or moved from outside the class, and
`instance()` returns the same object every time, as a reference to the interface. Each of those is
a type trait, so a console with a public constructor, or one whose copy operations were never
deleted, fails a named test rather than compiling quietly.

One subtlety is worth knowing, because the tests depend on it: deleting the move operations is
enough, on its own, to make the copy operations deleted too, since a class that declares a move
operation gets no implicit copy. So a console that deletes only its moves is, correctly, reported
as not copyable. Appendix C asks for all four deletions anyway, because saying so is clearer than
relying on the rule.

**The output, exactly.** The console prints what it is given, and the test compares that with the
characters written, byte for byte. The program's output is checked the same way: its first line
must be exactly `Transmitting data with a serial driver!`, printed through the console driver.

**Through the interface, not just directly.** The stub is exercised both as a `Stub` and through a
reference to the interface, and `sendMessage()` is tested with a recorder that keeps every byte
written to it, so a message sent with a missing character or a trailing null byte is caught, not
merely one whose last byte is wrong.

**What cannot be tested.** Whether a method is `[[nodiscard]]` cannot be checked with GCC: as
Appendix B notes, GCC does not warn when the result of a virtual call is discarded, so a test that
discards one compiles either way. `override` is checked by its effect: a method that was meant to
override but has the wrong signature leaves the stub abstract, and the tests then fail to compile
with *cannot declare variable to be of abstract type*.

---

## How the tests reach code in your `main.cpp`

`sendMessage()` lives in `main.cpp`, which is compiled into a program with a `main()` of its own,
and may well be in an anonymous namespace, which nothing outside the file can see. So
`exercise4_test.cpp` includes your file:

```cpp
#define main exercise4Main
#include "main.cpp"
#undef main
```

That makes `sendMessage()` callable from the test. Renaming `main` has one side effect: `main` is
the only function allowed to leave out its `return`, so the test silences that one warning for your
file.

**Your program is run as the program it is.** The suite also builds it on its own, with its own
`main()`, and the test runs it as a separate process, the way you would: it must end normally,
returning 0, and print exactly the expected output. A crash or a non-zero exit status is reported
as one. The renamed `main` in the included copy is never called, and could not safely be: only the
real `::main` may leave out its `return`, and renamed, the same code falls off the end of an
ordinary function, which GCC compiles into a trap.

---

## Adding a test

Add a `TEST` to the set's `exerciseN_test.cpp`, then check it against the reference solution with
`make SOLUTIONS=1` before relying on it. A new binary is a new entry in the [Makefile](./Makefile)'s
table; how the table is used is in [`ci/suite.mk`](../../../../ci/suite.mk).
