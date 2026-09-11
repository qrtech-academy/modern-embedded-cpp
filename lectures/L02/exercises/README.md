# L02 Exercises - Your Workspace

Write your solutions to the exercises in [Appendix C](../appendix/c_exercises.md) here, one
directory per exercise set, with the file names below. That is all it takes for `make test` to
find them:

```text
lectures/L02/exercises/
    exercise1/include/driver/gpio/led.hpp       Exercise Set 1: the Led class
    exercise1/include/driver/gpio/button.hpp                    the Button class
    exercise1/main.cpp                                          and the program that uses both
    exercise2/include/driver/gpio/led.hpp       Exercise Set 2: the same classes,
    exercise2/include/driver/gpio/button.hpp
    exercise2/main.cpp                                          controlled and blinked
    exercise3/include/driver/buzzer.hpp         Exercise Set 3: the Buzzer class,
    exercise3/source/driver/buzzer.cpp                          split across two files,
    exercise3/source/main.cpp                                   and the program that uses it
    exercise4/include/driver/timer.hpp          Exercise Set 4: the Timer class,
    exercise4/source/driver/timer.cpp                           split across two files,
    exercise4/source/main.cpp                                   and the program that uses it
    test/                                       The test suite. You do not edit this.
```

Build and run each program with a Makefile of its own: the one in
[L01 Appendix A](../../L01/appendix/a_compilation.md#simple-makefile) for Exercise Sets 1 and 2,
with `-Iinclude` added to the compiler flags, and the one for an
[embedded project structure](../../L01/appendix/a_compilation.md#embedded-project-structure) for
Exercise Sets 3 and 4. Then, from the repository root:

```bash
make test               # Every lecture's suite, against your code.
```

or from `lectures/L02/exercises/test`, for this lecture alone:

```bash
make                    # This lecture's suite, against your code.
make SOLUTIONS=1        # The same tests, against the reference solutions.
```

A set you have not started is reported as `SKIP`, with the file the tests are waiting for. Once
the file exists its tests are built and run; there is nothing to register. Exercise Set 1 is tested
as soon as `led.hpp` exists, the `Button` tests join once `button.hpp` does, and the program's output
is checked once `main.cpp` does.

**Your work here is not committed.** The repository's `.gitignore` ignores everything in this
directory except this README and `test/`, so `git status` stays quiet and pulling new course
material never collides with your files. Use `git add -f` for a file you do want to commit.

The reference solutions are in [`appendix/solutions`](../appendix/solutions/README.md). Try each
exercise before you read them.
