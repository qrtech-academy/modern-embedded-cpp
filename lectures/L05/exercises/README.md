# L05 Exercises - Your Workspace

Write your solutions to the exercises in [Appendix D](../appendix/d_exercises.md) here, one
directory per exercise set, with the file names below. That is all it takes for `make test` to
find them:

```text
lectures/L05/exercises/
    function_template/main.cpp                    Exercise Set 1: the clear and toggle templates
    class_template/include/driver/timer/timer.hpp  Exercise Set 3: the Timer class template
    class_template/source/main.cpp                                 and the program that tests it
    test/                                          The test suite. You do not edit this.
```

Exercise Set 2 and Exercise 3.2 are questions to answer in prose, so there is nothing to write here
for them.

Build and run each program with a Makefile of its own, as in
[L01 Appendix A](../../L01/appendix/a_compilation.md#simple-makefile-with-parameters); the timer
program has `include` and `source` directories, and builds with the Makefile under
[Embedded project structure](../../L01/appendix/a_compilation.md#embedded-project-structure). Then,
from the repository root:

```bash
make test               # Every lecture's suite, against your code.
```

or from `lectures/L05/exercises/test`, for this lecture alone:

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
