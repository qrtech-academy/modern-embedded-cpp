# Course Information

## Instructor
Erik Pihl ([erik.axel.pihl@gmail.com](mailto:erik.axel.pihl@gmail.com))

---

# Course Plan – Modern Embedded C++

| Week | Lecture | Topic |
|------|---------|------|
| 12 | L01 | Modern Embedded C++ concepts |
| 14 | L02 | Classes |
| 16 | L03 | Inheritance and interfaces |
| 18 | L04 | Factory pattern |
| 20 | L05 | Templates |
| 22 | L06 | Multithreading and synchronization |

---

## Lecture Content

### L01 – Modern Embedded C++ Concepts
Introduction to modern C++ in embedded systems.

Topics include:
* `.h` vs `.hpp` header conventions
* `extern "C"` and `#ifdef __cplusplus`
* Namespaces
* Keywords `constexpr` and `noexcept`
* Default arguments
* Structs with member functions
* References
* The `auto` keyword
* Function templates
* Parameter packs
* `[[nodiscard]]`

---

### L02 – Classes
Introduction to writing classes in modern C++.

Topics include:
* Classes vs structs
* Constructors and destructors
* Keywords `explicit`, `final`, `default`, and `delete`
* `static` methods and `static constexpr` members
* Copy and move constructors
* Copy and move assignment operators
* Object lifetime

---

### L03 – Inheritance and Interfaces
Object-oriented design for embedded systems.

Topics include:
* Inheritance
* Virtual functions
* Abstract classes
* Interfaces
* Polymorphism
* Interface-based driver design

---

### L04 – Factory Pattern
Construction patterns for modular embedded systems.

Topics include:
* Motivation for factories
* Factory pattern design
* Dependency management
* Decoupling system logic from hardware
* Using factories with interfaces
* Smart pointers (`std::unique_ptr`) vs raw pointers

---

### L05 – Templates
Generic programming for embedded systems.

Topics include:
* Template fundamentals
* Function templates
* Class templates
* Template instantiation (how the compiler generates code)
* Template specialization
* Code size considerations (impact in embedded systems)
* Type traits (compile-time type checks)
* Parameter packs (advanced)

---

### L06 – Multithreading
Concurrency concepts in modern C++.

Topics include:
* Threads
* Mutexes
* Atomic variables
* Lock guards
* `std::condition_variable`
* `std::future` and `std::async`
* Priority inversion
* Thread-safe design

---

## Course Material

### Literature
The course material consists of:
* Lecture notes
* Code examples
* Exercises completed after the lectures

---

### Software
Recommended tools:
* **[Visual Studio Code](https://code.visualstudio.com/download)** – Primary editor for writing and exploring code examples
* **GCC / G++** – Used to compile example programs
* **Linux / WSL** – Used to compile and run examples in a terminal environment

Instructions for installing the development environment are available [here](../lectures/L01/appendix/a_compilation.md).

---