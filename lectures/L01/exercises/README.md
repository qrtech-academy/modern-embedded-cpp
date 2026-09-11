# L01 Exercises - Your Workspace

Write your solutions to the exercises in [Appendix C](../appendix/c_exercises.md) here, one
directory per exercise set, with the file names below. That is all it takes for `make test` to
find them:

```text
lectures/L01/exercises/
    exercise1/main.cpp            Exercise Set 1: debug::log and app::delay_ms
    exercise2/driver/timer.hpp    Exercise Set 2: the software timer
    exercise2/main.cpp                            and the program that uses it
    exercise3/main.cpp            Exercise Set 3: swap
    exercise4/main.cpp            Exercise Set 4: the clear and toggle templates
    test/                         The test suite. You do not edit this.
```

Build and run each program with a Makefile of its own, as in
[Appendix A](../appendix/a_compilation.md#simple-makefile). Then, from the repository root:

```bash
make test               # Every lecture's suite, against your code.
```

or from `lectures/L01/exercises/test`, for this lecture alone:

```bash
make                    # This lecture's suite, against your code.
make SOLUTIONS=1        # The same tests, against the reference solutions.
```

A set you have not started is reported as `SKIP`, with the file the tests are waiting for. Once
the file exists its tests are built and run; there is nothing to register.

**Your work here is not committed.** The repository's `.gitignore` ignores everything in this
directory except this README and `test/`, so `git status` stays quiet and pulling new course
material never collides with your files. Use `git add -f` for a file you do want to commit.

The reference solutions are in [`appendix/solutions`](../appendix/solutions/README.md). Try each
exercise before you read them.
