# L01 - Modern Embedded C++ Concepts

## Agenda
* Transition from C to modern C++ in embedded systems.
* Namespaces.
* `constexpr` and compile-time constants.
* `noexcept` and exception handling in embedded systems.
* Default arguments.
* Modern C++ structs (member functions, constructor/destructor, and encapsulation).
* References.
* Function templates and parameter packs.

---

## Goals of the Lecture
After this lecture, participants should:
* Become familiar with modern C++ language features commonly used in embedded systems.
* Understand how C++ can improve structure and safety in embedded software.
* Understand how modern C++ structs can group data and behavior in a single type.
* Learn how simple drivers can be implemented using modern C++ structs with member functions, constructors/destructors, and encapsulation.
* Understand how references simplify function interfaces compared to pointers.
* Understand how templates enable generic utilities such as bit manipulation helpers.
* Gain intuition about compile-time programming (`constexpr` and templates).

---

## Prerequisites
* Knowledge of C programming (functions, pointers, structs, etc.).
* Familiarity with bit manipulation and embedded programming concepts.

---

## Instructions

### Preparation
* Install WSL and the GCC compiler as described in [Appendix A](./appendix/a_compilation.md).
* Read [Appendix B](./appendix/b_from_c_to_cpp.md).

---

## During the Lecture
* Complete the exercises in [Appendix C](./appendix/c_exercises.md).
* Participants will be given time during the lecture to complete the exercises.
* Solutions will then be discussed in class.

---

## Evaluation
Participants should be able to explain:
* What is the purpose of `constexpr`?
* Why is `noexcept` often used in embedded systems?
* How does a C++ struct differ from a traditional C struct?
* Why are constructors and destructors useful in embedded C++?
* What is the purpose of making member variables private?
* What advantages do references have compared to pointers?
* Why can templates increase binary size in embedded systems?
* What are parameter packs used for?

---

## Next Lecture
Classes in C++:
* Deeper look at constructors and destructors.
* Keywords `explicit`, `final`, `default`, and `delete`.
* Copy constructors.
* Move constructors.
* Assignment operators.

Participants will implement their first class-based drivers.

---
