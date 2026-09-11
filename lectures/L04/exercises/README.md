# L04 Exercises - Your Workspace

Write your solutions to the exercises in [Appendix C](../appendix/c_exercises.md) here. The
exercises build one program twice: first with raw pointers (Exercise Sets 1 to 3), then with
`std::unique_ptr` (Exercise Set 4). Each version has a directory of its own, laid out as the
appendix asks, and that is all it takes for `make test` to find them:

```text
lectures/L04/exercises/
    raw_factory/                          Exercise Sets 1 to 3
        include/
            app/logic/logic.hpp           Set 3: app::logic::Logic, owning a raw pointer
            driver/factory/interface.hpp  Set 2: driver::factory::Interface
            driver/factory/esp32s3.hpp    Set 2: driver::factory::Esp32s3
            driver/factory/stub.hpp       Set 2: driver::factory::Stub
            driver/serial/interface.hpp   Set 1: driver::serial::Interface
            driver/serial/esp32s3.hpp     Set 1: driver::serial::Esp32s3
            driver/serial/stub.hpp        Set 1: driver::serial::Stub
        source/main.cpp                   Set 3: the program
    smart_factory/                        Exercise Set 4: a copy of raw_factory, then updated
        include/...                       the same files, returning std::unique_ptr
        source/main.cpp
    test/                                 The test suite. You do not edit this.
```

Build and run each program with a Makefile of its own, as in
[L01 Appendix A](../../L01/appendix/a_compilation.md#embedded-project-structure). Then, from the
repository root:

```bash
make test               # Every lecture's suite, against your code.
```

or from `lectures/L04/exercises/test`, for this lecture alone:

```bash
make                    # This lecture's suite, against your code.
make SOLUTIONS=1        # The same tests, against the reference solutions.
```

An exercise set you have not started is reported as `SKIP`, with the file the tests are waiting
for. Once the file exists its tests are built and run; there is nothing to register. Set 4 is tested
file by file: its tests start with the new factory interface, and pick up the factories and the
logic class as you update them.

**Your work here is not committed.** The repository's `.gitignore` ignores everything in this
directory except this README and `test/`, so `git status` stays quiet and pulling new course
material never collides with your files. Use `git add -f` for a file you do want to commit.

The reference solutions are in [`appendix/solutions`](../appendix/solutions/README.md). Try each
exercise before you read them.
