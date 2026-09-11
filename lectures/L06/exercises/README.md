# L06 Exercises - Your Workspace

Write your solutions to the exercises in [Appendix B](../appendix/b_exercises.md) here, one
directory per exercise set, with the file names below. That is all it takes for `make test` to
find them:

```text
lectures/L06/exercises/
    exercise1/exercise1.cpp       Exercise Set 1: workerThread and an atomic stop flag
    exercise2/exercise2.cpp       Exercise Set 2: incrementCounter and a shared counter
    exercise3/exercise3.cpp       Exercise Set 3: SharedMem, txThread and rxThread
    exercise4-5/                  Exercise Sets 4 and 5: the thread-safe counter stub
        include/driver/counter/interface.hpp
        include/driver/counter/stub.hpp
        source/main.cpp
    exercise6/exercise6.cpp       Exercise Set 6: the condition variable version of Set 3
    exercise7/exercise7.cpp       Exercise Set 7: validateFirmware and std::async
    test/                         The test suite. You do not edit this.
```

Build and run each program with a Makefile of its own, as in
[L01 Appendix A](../../L01/appendix/a_compilation.md#simple-makefile), with `-pthread` added to the
compiler flags. Then, from the repository root:

```bash
make test               # Every lecture's suite, against your code.
```

or from `lectures/L06/exercises/test`, for this lecture alone:

```bash
make                    # This lecture's suite, against your code.
make SOLUTIONS=1        # The same tests, against the reference solutions.
```

A set you have not started is reported as `SKIP`, with the file the tests are waiting for. Once
the file exists its tests are built and run; there is nothing to register.

**Each file grows through its set, and the tests check where it ends up**: Exercise 1.3's stop
flag, Exercise 2.3's atomic counter, Exercise Set 5's initialization flag. Until you reach the end
of a set, expect its tests to fail to compile or to report what is still missing.

**Your work here is not committed.** The repository's `.gitignore` ignores everything in this
directory except this README and `test/`, so `git status` stays quiet and pulling new course
material never collides with your files. Use `git add -f` for a file you do want to commit.

The reference solutions are in [`appendix/solutions`](../appendix/solutions/README.md). Try each
exercise before you read them.
