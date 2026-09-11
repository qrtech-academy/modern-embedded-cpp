# Modern Embedded C++
Repository for the course **Modern Embedded C++**.

This course consists of six lectures and is intended for experienced C developers who want to learn how modern C++ can be used effectively in embedded systems.

---

## About the Course
The course covers development of embedded systems using modern C++ (C++17) with a focus on practical design techniques for resource-constrained systems.

Topics include:
* Transition from C to modern embedded C++.
* Compile-time programming (`constexpr`, templates).
* Deterministic embedded design (`noexcept`, controlled use of dynamic memory allocation).
* Structs and classes for hardware drivers.
* Encapsulation and object-oriented design in embedded systems.
* Interfaces and polymorphism.
* Factory patterns and dependency management.
* Templates and generic programming.
* Copy and move semantics in embedded C++.
* Template utilities for low-level programming.
* Threading and synchronization primitives.
* Designing reusable and testable driver libraries.

Unlike introductory C++ courses, this course assumes strong prior experience with C, and focuses only on the C++ concepts that are useful in embedded systems.

---

## During the Course
During the lectures participants will:
* Implement simple drivers using modern C++.
* Apply compile-time techniques to low-level programming.
* Refactor traditional C patterns into safer C++ constructs.
* Design modular embedded software using interfaces and factories.
* Explore how C++ can improve modularity and code structure without introducing runtime overhead.

Examples and exercises focus on typical embedded problems, such as:
* Bit manipulation utilities.
* Register access helpers.
* Simple driver abstractions.
* Compile-time utilities for low-level code.
* Interface-based driver design.
* Template-based utilities.

---

## Learning Outcomes
After completing the course, participants should be able to:
* Use modern C++ features effectively in embedded systems.
* Write low-level code using templates and compile-time techniques.
* Design simple hardware drivers using structs and classes.
* Use interfaces to decouple system logic from hardware implementations.
* Apply factory patterns to construct embedded components.
* Use templates to build reusable embedded utilities.
* Apply object-oriented design without sacrificing performance or determinism.
* Understand how C++ abstractions translate to machine code in embedded environments.

---

## Two Written Papers, and What They Are Not

Nothing in this course is marked. Assessment is the exercise set after every lecture, each with a
published solution, and the Evaluation questions that close every lecture README.

[`exam/`](./exam/README.md) holds two three-hour papers with worked solutions, and they check
something else: **what you can reconstruct on paper, with nothing in front of you.** Eight questions
each, mixing theory with C++ you either read and repair or write from scratch, and no compiler in
the room to tell you which it is.

**They exist purely so that participants can test their own knowledge after the course. They gate
nothing, they are not a qualification, and no part of the course requires them.** Nothing in this
repository depends on them, and neither `make build` nor `make format-check` knows they exist.

**Take one after the course is over.** Both papers draw on all six lectures, so sitting one partway
through examines material nobody has taught you yet.

---

## Checking Your Work

Every lecture has a test suite for its exercises. Clone the repository together with its test
framework, which is a submodule:

```bash
git clone --recursive https://github.com/qrtech-academy/modern-embedded-cpp.git
```

(In a clone made without `--recursive`, run `git submodule update --init` once.) Write your
solutions in `lectures/LNN/exercises`, where each lecture's README says which file goes where, and
run:

```bash
make test               # Every lecture's tests, against your code.
make test SOLUTIONS=1   # The same tests, against the reference solutions.
```

An exercise set you have not started is reported as `SKIP`, with the file its tests are waiting
for; once that file exists, its tests run. Where an exercise gives example output, the tests
compare your program's output with it, character for character.

---

## The Book

The whole course is also available as a book:
**[download the latest PDF](https://github.com/qrtech-academy/modern-embedded-cpp/releases/latest/download/modern-embedded-cpp.pdf)**.
It is built from the sources in [`book/`](./book/README.md), which also say how to build it yourself
(`make -C book`) and how a new edition is released.

---

## Structure

```text
ci/          CI scripts (build, test and format checks)
info/        Course info
lectures/    Lecture plans, examples, exercises, and each lecture's test suite
libs/        The test framework (a submodule) and the course's test support
exam/        Two written papers and their solutions. Optional, and marked by nobody here.
book/        The course typeset as a book with LuaLaTeX; `make -C book` builds the PDF.
```

---

## License

The source code is released under the [MIT License](./LICENSE): the example programs and the
exercise solutions, the test suites and their support code, the build and CI scripts, and the
book's build files.

The course material is licensed under [CC BY 4.0](./LICENSE-CONTENT): the lectures, exercises and
exam papers, the other Markdown documents, the figures, and the book typeset from them. You may
share and adapt it for any purpose, as long as you give credit. The code examples printed in the
lectures and in the book may also be used under the MIT License. The submodule `libs/test` carries
its own license.

---
