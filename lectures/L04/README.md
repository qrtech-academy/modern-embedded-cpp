# L04 - Factory pattern

## Agenda
* Motivation for the factory pattern.
* Implementing factories using:
  * Raw pointers.
  * Smart pointers.
* How the system logic uses factories to create drivers.

---

## Objectives
* Understand what the factory pattern is and why it is used.
* Understand how a factory enables switching between:
  * Real drivers.
  * Stubs.
* Understand how the system logic can create drivers via a factory.
* Be able to read and follow a simple factory example.

## Prerequisites
* Basic knowledge of classes and interfaces in C++.
* Understanding of stubs and system logic.

---

## Instructions

### Preparation
* Read [Appendix A](./appendix/a_factory_raw_pointers.md) for information about factories using raw pointers.
* Read [Appendix B](./appendix/b_factory_smart_pointers.md) for information about factories using smart pointers.

### During the Lecture
* Participate in the lecture (live coding session).

### After the Lecture
* Complete the exercises in [Appendix C](./appendix/c_exercises.md).
* Exercise solutions will be uploaded a few days after the session.

---

## Evaluation
1. What is the purpose of the factory pattern?
2. What does it mean that the system logic only depends on driver interfaces and not on concrete driver types?
3. Who is responsible for deleting driver objects when factories return raw pointers?
4. What is the difference between raw pointers and `std::unique_ptr`?
5. Why is `std::unique_ptr` suitable in the factory pattern?

## Next lecture
* Templates – Generic programming for embedded systems without runtime overhead.

---
