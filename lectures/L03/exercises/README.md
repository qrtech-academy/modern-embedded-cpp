# L03 Exercises - Your Workspace

Write your solution to the exercises in [Appendix C](../appendix/c_exercises.md) here. Unlike the
earlier lectures, all four exercise sets build **one** program, so this directory is that program:
the directory structure Appendix C asks for goes here, directly beside this README and `test/`.
That is all it takes for `make test` to find it:

```text
lectures/L03/exercises/
    Makefile                              Your program's Makefile.
    include/
        driver/
            serial/
                interface.hpp             Exercise Set 1: the serial interface
                stub.hpp                  Exercise Set 2: the stub driver
                console.hpp               Exercise Set 3: the console driver
    source/
        driver/
            serial/
                stub.cpp                  Exercise Set 2
                console.cpp               Exercise Set 3
        main.cpp                          Exercise Set 4: sendMessage() and the program
    test/                                 The test suite. You do not edit this.
```

Build and run the program with the Makefile from
[Appendix A of L01](../../L01/appendix/a_compilation.md#embedded-project-structure), listing the
three source files and adding `-Iinclude` to the compiler flags. Then, from the repository root:

```bash
make test               # Every lecture's suite, against your code.
```

or from `lectures/L03/exercises/test`, for this lecture alone:

```bash
make                    # This lecture's suite, against your code.
make SOLUTIONS=1        # The same tests, against the reference solution.
```

Each exercise set is still tested on its own, as soon as its files exist, so you can check the
interface before you have written a single driver. A set you have not started is reported as
`SKIP`, with the file the tests are waiting for.

**Your work here is not committed.** The repository's `.gitignore` ignores everything in this
directory except this README and `test/`, so `git status` stays quiet and pulling new course
material never collides with your files. Use `git add -f` for a file you do want to commit.

The reference solution is in [`appendix/solutions`](../appendix/solutions/README.md), laid out
exactly like this directory. Try each exercise before you read it.
