# L05 – Templates

## Agenda
* Motivation for templates in embedded systems.
* Function templates.
* Template instantiation (compile-time code generation).
* Class templates.
* Code size considerations in embedded systems.
* Introduction to type traits.
* If time allows: Introduction to parameter packs.

---

## Objectives
* Understand what templates are and why they are used.
* Understand how function templates enable generic code.
* Understand how class templates can be used for reusable components (e.g., buffers, frames).
* Understand that templates are instantiated at compile time and generate concrete types/functions.
* Understand how templates affect code size in embedded systems.
* Understand when templates are appropriate in embedded systems and when they are not.
* Be able to read and follow simple template-based code.
* Understand the purpose of type traits for basic compile-time checks.

---

## Prerequisites
* Basic knowledge of functions and classes in C++.
* Understanding of interfaces and system architecture.
* Basic understanding of compilation.

---

## Instructions

### Preparation
* Read [Appendix A](./appendix/a_function_templates.md) for an introduction to function templates.
* Read [Appendix B](./appendix/b_type_traits_and_constraints.md) for an introduction to type traits and constraints.
* Read [Appendix C](./appendix/c_class_templates.md) for an introduction to class templates.

### During the Lecture
* Participate in the lecture (live coding session).

### After the Lecture
* Complete the exercises in [Appendix D](./appendix/d_exercises.md).
* Exercise solutions will be uploaded a few days after the session.

---

## Evaluation
1. What problem do templates solve in C++?
2. What is the difference between a function template and a class template?
3. What does it mean that templates are instantiated at compile time, and what does the compiler generate?
4. Why can templates increase code size in embedded systems?
5. Give an example where a template is more suitable than an interface.
6. Why must template implementations typically be placed in header files?

---

## Next lecture
* Concurrency concepts in modern C++:
    * Threads.
    * Mutexes.
    * Atomic variables.
    * Lock guards.
    * Synchronization primitives.
    * Thread-safe design.

---