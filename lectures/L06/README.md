# L06 - Multithreading and Synchronization Mechanisms

## Agenda
* Introduction to multithreading in C++ (`std::thread`).
* Concurrency vs. true parallelism — single-core vs. multi-core.
* Scheduling: time-slicing vs. cooperative.
* Data races and why they are dangerous.
* Critical sections using `std::mutex` and `std::lock_guard`.
* The difference between `std::mutex` and `std::atomic`.
* Signalling between threads using `std::condition_variable`.
* Launching background tasks with `std::future` and `std::async`.
* Priority inversion — a common pitfall in embedded systems.

---

## Lecture Objectives
* Explain what a thread is and how it is created using `std::thread`.
* Explain the difference between concurrency and parallelism.
* Explain preemptive and cooperative scheduling.
* Explain what a data race is and why it leads to *undefined behavior*.
* Use `std::mutex` and `std::lock_guard` correctly.
* Explain when `std::atomic` is sufficient and when `std::mutex` is required.
* Use `std::condition_variable` to signal between threads without busy-waiting.
* Launch asynchronous tasks using `std::async` and retrieve results via `std::future`.
* Explain what priority inversion is and why `std::mutex` does not solve it on an RTOS.

---

## Prerequisites
* Good knowledge of C++ (classes, references, RAII).
* Basic understanding of program flow and object lifecycle.

---

## Instructions

### Preparation
* Read [Appendix A](./appendix/a_multithreading_synchronization.md), which covers multithreading and synchronization mechanisms in C++.

### During the Lecture
* Participate in the lecture (live coding session).

### After the Lecture
* Complete the exercises in [Appendix B](./appendix/b_exercises.md).
* Exercise solutions will be uploaded a few days after the session.

---

## Evaluation
* What is the difference between a `mutex` and an `atomic`?
* What is a data race?
* What is the difference between concurrency and parallelism?
* When would you use a `condition_variable` instead of polling in a loop?
* What is priority inversion, and why should you be careful with `std::mutex` on an RTOS?

---

## Next Lecture
* The C++ track is now complete.
* Next track after the summer break - Machine Learning for Embedded Systems:
    * Implementing algorithms from scratch in C++.
    * Training algorithms by hand to understand the underlying mathematics.

---