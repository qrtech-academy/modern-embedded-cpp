# Appendix C

## Class Templates
Class templates allow us to define generic data structures, where the type is determined at compile time.

This enables reusable and efficient code without run-time overhead.

---

## Example - A Generic Container

In this appendix, we implement a simple container similar to `std::vector`:

``` cpp
container::Vector<T>
```

The purpose is to:
* Understand how class templates work.
* Understand how memory management affects embedded systems.
* Use compile-time constraints for safety.
* See compile-time polymorphism in practice.

---

## What is a Class Template?
A class template is a blueprint for a class where the type is not fixed until compilation.

Example:

``` cpp
namespace container
{
template<typename T>
class Vector
{
public:
    Vector() noexcept
        : myData{nullptr}
        , mySize{}
    {}
private:
    T* myData;
    std::size_t mySize;
};
} // namespace container
```

When used:

``` cpp
container::Vector<int> a{};
container::Vector<double> b{};
```

The compiler generates two separate types:

``` cpp
namespace container
{
// Vector specialization for int.
class Vector<int>
{
public:
    Vector() noexcept
        : myData{nullptr}
        , mySize{}
    {}
private:
    int* myData;
    std::size_t mySize;
};

// Vector specialization for double.
class Vector<double>
{
public:
    Vector() noexcept
        : myData{nullptr}
        , mySize{}
    {}
private:
    double* myData;
    std::size_t mySize;
};
} // namespace container
```

This is compile-time polymorphism.

---

## Example Implementation
Below is a simplified implementation of a vector-like container.

Key features:
* Dynamic resizing.
* Push-back functionality.
* Range-based iteration support.

``` cpp
#pragma once

#include <cstddef>
#include <cstdlib>
#include <type_traits>

namespace container
{
template<typename T>
class Vector
{
    // Generate a compiler error if the given type isn't trivially copyable or destructible, 
    // since std::realloc() and std::free() are used in this implementation.
    static_assert(std::is_trivially_copyable<T>::value && std::is_trivially_destructible<T>::value, 
        "Vector type T must be of trivial type!");
public:
    Vector() noexcept
        : myData{nullptr}
        , mySize{}
    {
        // Initialized empty vector.
    }

    template<typename... Args>
    Vector(const Args& ...args) noexcept
        : Vector()
    {
        // Try to resize the vector to fit the given number of arguments.
        // Copy the arguments to the vector on success.
        if (resize(sizeof...(args)))
        {
            std::size_t i{};
            ((myData[i++] = args), ...);
        }
    }

    ~Vector() noexcept 
    {
        // Release allocated resources before deletion. 
        clear(); 
    }

    T& operator[](const std::size_t index) noexcept 
    { 
        // Return a reference to the element at given index.
        return myData[index]; 
    }
    const T& operator[](const std::size_t index) const noexcept 
    {
        // Return a read-only reference to the element at given index. 
        return myData[index]; 
    }

    T* begin() noexcept 
    { 
        // Return a pointer to the beginning of the data field.
        return myData; 
    }
    const T* begin() const noexcept 
    {
        // Return a read-only pointer to the beginning of the data field. 
        return myData; 
    }
    
    T* end() noexcept { return nullptr != myData ? myData + mySize : nullptr; }
    const T* end() const noexcept { return nullptr != myData ? myData + mySize : nullptr; }

    const T* data() const noexcept 
    { 
        // Return a read-only pointer to the data field.
        return myData; 
    }

    std::size_t size() const noexcept 
    {
        // Return the size of the vector. 
        return mySize; 
    }

    bool empty() const noexcept 
    { 
        // Return true if the vector is empty.
        return 0U == mySize; 
    }

    void clear() noexcept
    {
        // Free allocated resources, then reset the vector parameters.
        std::free(myData);
        myData = nullptr;
        mySize = 0U;
    }

    bool resize(const std::size_t size) noexcept
    {
        // Clear the vector if the requested size is 0.
        if (0U == size)
        {
            clear();
            return true;
        }

        // Try to reallocate the data field to the requested size, return false on failure.
        T* data{static_cast<T*>(std::realloc(myData, sizeof(T) * size))};
        if (nullptr == data) { return false; }
        myData = data;

        // Initialize new elements (if any).
        // Update the vector size, then return true to indicate success.
        for (std::size_t i{mySize}; i < size; ++i) { myData[i] = {}; }
        mySize = size;
        return true;
    }

    bool pushBack(const T& element) noexcept
    {
        const std::size_t newSize{mySize + 1U};
        // Try to reallocate the data field to fit one more element, return false on failure.
        T* data{static_cast<T*>(std::realloc(myData, sizeof(T) * newSize))};
        if (nullptr == data) { return false; }

        // Push the new element to the back of the vector, then return true to indicate success.
        myData = data;
        myData[mySize++] = element;
        return true;
    }

    Vector(Vector&)                  = delete; // No copy constructor.
    Vector(Vector&&)                 = delete; // No move constructor.
    Vector& operator=(const Vector&) = delete; // No copy assignment.
    Vector& operator=(Vector&&)      = delete; // No move assignment.

private:        
    /** Pointer to dynamically allocated data field. */
    T* myData;

    /** Size of the data field in number of elements. */
    std::size_t mySize;
};
} // namespace container
```

The above implementation means that the class template `container::Vector` behaves in many ways like `std::vector`, including initialization with an argument list:

```cpp
container::Vector<int> vec1{1, 3, 5, 7, 9};
container::Vector<double> vec2{0.5, 1.5, 2.5, 3.5, 4.5};
```

---

## Placement of the implementation in header files
Since `container::Vector` is a template, this affects how the implementation must be placed in the source code.

The compiler must see the full definition of a template when it instantiates it, for example when we write:

```cpp
container::Vector<int>
```

This means that both the class declaration and the method implementations must be visible where the template is used.

There are two common solutions:

### a) Everything in the same header file
The most common approach is to place both declaration and implementation in the same header file, for example:

```
include/container/vector.h
```

Contains:

```cpp
template<typename T>
class Vector { ... };

template<typename T>
void Vector<T>::push_back(const T& value)
{
    ...
}
```

This works because the compiler sees all code when the template is instantiated.

---

### b) Header + implementation header
In larger projects, it is common to separate declaration and implementation into two files, but include the implementation at the end of the header file.

Structure:

```
include/container/vector.h
include/container/impl/vector_impl.h
```

And at the end of `vector.h`:

```cpp
#include "container/impl/vector_impl.h"
```

This gives the same effect as option 1, but provides better structure and readability.

---

### Why doesn’t a `.cpp` file work?
If the implementation is placed in a regular `.cpp` file, the compiler cannot see the code when the template is instantiated in other files. This often leads to linker errors such as:

```
undefined reference to ...
```

Templates must therefore be implemented in header files (directly or indirectly via include).

---

## Embedded perspective
In embedded systems, we need to consider the following:

### Heap usage
* Dynamic allocation may be forbidden.
* Fragmentation can occur.

### Binary size
* Each instance of `Vector<T>` generates new code.
* `Vector<int>` ≠ `Vector<double>`

### Exception handling
* In many embedded systems, exceptions are disabled.
* Therefore, `noexcept` is used.

---

## Summary
In this appendix, we have seen:
* How to create a class template.
* How to handle manual memory allocation.
* How copy/move semantics can be handled or restricted.
* Why template implementations must reside in header files.
* How templates affect embedded systems.

`container::Vector<T>` is a clear example of:
* Compile-time polymorphism.
* Generic programming.
* Performance-optimized C++.
* Full control over memory and lifecycle.

---
