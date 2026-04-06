# Appendix A

## Function Templates
Templates allow functions to operate on multiple data types without duplicating code.

In embedded systems, templates are often used to implement generic utilities without run-time overhead, while still allowing the compiler to optimize the code for each specific type.

---

## Basic Example
Consider a simple function template that adds two values:

``` cpp
/**
 * @brief Add two numbers.
 *
 * @tparam T The numeric type.
 *
 * @param[in] x The first value.
 * @param[in] y The second value.
 *
 * @return The sum of the two values.
 */
template<typename T>
constexpr T add(const T x, const T y) noexcept
{
    return x + y;
}
```

The function can be used with different types:

``` cpp
const auto sum1 = add(1, 2);
const auto sum2 = add(1.0, 2.0);
```

The compiler generates a separate version of the function for each type.

---

## Embedded Example - Bit Manipulation
In embedded systems, templates are often used for low-level utilities.

The example below sets a bit in a register of arbitrary integral type:

``` cpp
#include <cstdint>

/**
 * @brief Set bit in register.
 *
 * @tparam T The register type.
 *
 * @param[out] reg Register to write to.
 * @param[in] bit The bit to set.
 */
template<typename T>
constexpr void set(T& reg, const std::uint8_t bit) noexcept
{
    reg |= (static_cast<T>(1U) << bit);
}
```

Example usage:

``` cpp
std::uint8_t reg{};
set(reg, 1U);
set(reg, 2U);
```

This allows the same function to be reused for different register sizes.

---

## Template Instantiation
Templates are instantiated at compile time.

For example:

``` cpp
std::uint8_t reg1{};
std::uint32_t reg2{};

set(reg1, 1U);
set(reg2, 12U);
```

The compiler generates two separate functions:

``` cpp
set(std::uint8_t&, std::uint8_t)
set(std::uint32_t&, std::uint8_t)
```

Each version is compiled independently.

---

## Code Size Considerations
Templates are often described as zero-cost abstractions:
* No run-time overhead.
* Code is specialized for each type.

However, in embedded systems this also means:
* Multiple instantiations → increased binary size.
* More types → more generated code.

Therefore:
* Use templates when they provide clear value.
* Avoid unnecessary instantiations in resource-constrained systems.

---

## Function Overloading vs Templates
C++ supports function overloading:

``` cpp
constexpr void set(std::uint8_t& reg, std::uint8_t bit) noexcept;
constexpr void set(std::uint32_t& reg, std::uint8_t bit) noexcept;
```

Templates provide a more scalable solution:

``` cpp
template<typename T>
constexpr void set(T& reg, std::uint8_t bit) noexcept;
```

---

## Summary
* Function templates enable generic and reusable code.
* Templates are instantiated at compile time.
* Each type results in a separate generated function.
* Templates provide no run-time overhead, but may increase code size.
* Templates are especially useful for:
    * Bit manipulation.
    * Utility functions.
    * Hardware abstraction helpers.

---
