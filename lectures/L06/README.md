# L06 - Multithreading and Synchronization Mechanisms

## Agenda
* Introduction to multithreading in C++ (`std::thread`).
* Data races and why they are dangerous.
* Critical sections using `std::mutex` and `std::lock_guard`.
* The difference between `std::mutex` and `std::atomic`.

---

## Lecture Objectives
* Explain what a thread is and how it is created using `std::thread`.
* Explain what a data race is and why it leads to *undefined behavior*.
* Use `std::mutex` and `std::lock_guard` correctly.
* Explain when `std::atomic` is sufficient and when `std::mutex` is required.

---

## Prerequisites
* Good knowledge of C++ (classes, references, RAII).
* Basic understanding of program flow and object lifecycle.

---

## Instructions

### Preparation
* Read [Appendix A](./appendix/a_threads_mutex_atomic.md), which covers multithreading and synchronization mechanisms in C++.

### During the Lecture
* Participate in the lecture (live coding session).

### After the Lecture
* Complete the exercises in [Appendix B](./appendix/b_exercises.md).
* Exercise solutions will be uploaded a few days after the session.

---

## Evaluation
* What is the difference between a `mutex` and an `atomic`?
* What is a data race?

---

## Next Lecture
* The C++ track is now complete.
* Next track after the summer break - Machine Learning for Embedded Systems:
    * Implementing algorithms from scratch in C++.
    * Training algorithms by hand to understand the underlying mathematics.

---
