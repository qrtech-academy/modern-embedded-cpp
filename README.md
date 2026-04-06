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

## Structure

```text
info/        Course info
lectures/    Lecture plans, examples, and exercises
```

---
