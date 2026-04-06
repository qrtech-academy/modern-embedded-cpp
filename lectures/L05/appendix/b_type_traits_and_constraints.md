# Appendix B

## Type Traits and Constraints
Type traits allow programs to inspect and reason about types at compile time.

They are provided in the C++ standard library via the header:

``` cpp
#include <type_traits>
```

Type traits are commonly used together with templates to create safer and more robust code, for instance to prevent misuse of low-level hardware APIs (e.g., restricting register operations to integral types).

---

## Common Type Traits
Some commonly used type traits include:
* `std::is_integral` - Checks if a type is an integer type.
* `std::is_arithmetic` - Checks if a type is numeric (integer or floating point).
* `std::is_unsigned` - Checks if a type is an unsigned integer.
* `std::is_floating_point` - Checks if a type is a floating-point type.

Each type trait provides a boolean value:

``` cpp
std::is_integral<int>::value     // true
std::is_integral<double>::value  // false
```

Since C++17, a shorthand form is also available:

``` cpp
std::is_integral_v<int> // true
```

---

## Compile-Time Constraints with `static_assert()`
Type traits are often used together with `static_assert()` to enforce constraints at compile time.

Example:

``` cpp
#include <type_traits>

/**
 * @brief Add two numbers.
 *
 * @tparam T The numeric type. Must be arithmetic.
 */
template<typename T>
constexpr T add(const T x, const T y) noexcept
{
    static_assert(std::is_arithmetic<T>::value, "Cannot add values of non-arithmetic types!");
    return x + y;
}
```

If the function is used incorrectly:

``` cpp
add("Hello", "World");
```

The compiler generates an error instead of allowing a run-time failure.

---

## Why Compile-Time Checks Matter in Embedded Systems
In embedded systems:
* Errors should be detected as early as possible.
* Debugging run-time issues can be difficult.
* Safety and robustness are critical.

Compile-time checks provide:
* Early error detection.
* Clear diagnostics.
* Zero run-time overhead.

---

## Creating Custom Type Traits
If no suitable type trait exists, we can create our own.

A type trait is typically implemented as a template struct that exposes a boolean constant named `value`. We set the constant `value` to `false` by default for a given type `T`:

``` cpp
template<typename T>
struct isUnsigned
{
    static constexpr bool value{false};
};
```

We then set the constant `value` to `true` for unsigned types, such as `std::uint8_t` and `std::uint32_t`:

``` cpp
#include <cstdint>

template<>
struct isUnsigned<std::uint8_t>
{
    static constexpr bool value{true};
};

template<>
struct isUnsigned<std::uint16_t>
{
    static constexpr bool value{true};
};

template<>
struct isUnsigned<std::uint32_t>
{
    static constexpr bool value{true};
};
```

Usage:

``` cpp
// Generate a compiler error if T isn't of unsigned type:
static_assert(isUnsigned<T>::value, "T must be unsigned!");
```

---

## Variable Templates (C++17)
For convenience, we can define a variable template:

``` cpp
template<typename T>
inline constexpr bool isUnsigned_v{isUnsigned<T>::value};
```

This allows:

``` cpp
// Generate a compiler error if T isn't of unsigned type:
static_assert(isUnsigned_v<T>, "T must be unsigned!");
```

**Note**: In practice, the standard library already provides many useful traits (e.g., `std::is_unsigned`). Custom traits are mainly used when no suitable standard trait exists.

---

## Summary
* Type traits allow inspection of types at compile time.
* `static_assert()` enables compile-time validation.
* Compile-time checks improve safety and robustness.
* Custom type traits can be created when needed.
* These tools are essential for building safe template-based APIs.

---
