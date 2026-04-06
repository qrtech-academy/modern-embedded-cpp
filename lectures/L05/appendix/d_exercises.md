# Appendix D

## Exercises

## Exercise Set 1 – Function Templates

### Exercise 1.1 – Clear Bit
Create a function template to clear a bit in a register:

```cpp
template<typename T>
constexpr void clear(T& reg, std::uint8_t bit) noexcept;
```

Tasks:
1. Use `static_assert()` with `std::is_integral<T>::value`.
2. Error message:
   ```
   Cannot perform bit operation with non-integral type!
   ```
3. Clear the selected bit.
4. Print the result using `std::cout` and `std::bitset`.

---

### Exercise 1.2 – Toggle Bits
Create the following function template to toggle one or several bits in a register:

```cpp
template<typename T, typename... Bits>
constexpr void toggle(T& reg, const Bits... bits) noexcept;
```

Tasks:
1. Use a parameter pack.
2. Ensure that `T` is an integral type using `static_assert()`.
3. Iterate over the bits (for example using `{bits...}`).
4. Toggle the specified bits.
5. Print the result using `std::cout` and `std::bitset`.

---

## Exercise Set 2 – Function Template Concepts

### Exercise 2.1 – Template Constraints
Tasks:
1. Explain why `static_assert()` is useful.
2. What happens if the constraint fails?
3. Why use compile-time checks instead of runtime?

---

## Exercise Set 3 – Class Templates

### Exercise 3.1 – Fixed-Size Array
Create the following class template:

```cpp
namespace container
{
template<typename T, std::size_t Size>
class Array;
} // namespace container
```

Tasks:
1. Store the elements in a fixed-size array using the template parameter:
   * `T myData[Size];`
2. Implement the following methods:
   * `T& operator[](std::size_t index) noexcept`
   * `const T& operator[](std::size_t index) const noexcept`
   * `std::size_t size() const noexcept`
   * `T* begin() noexcept`
   * `const T* begin() const noexcept`
   * `T* end() noexcept`
   * `const T* end() const noexcept`
3. Test with `int` and `double`.

---

### Exercise 3.2 – Fill Function
Add the following method to fill the entire array with the given value:

```cpp
void fill(const T& value) noexcept;
```

Tasks:
1. Implement the method.
2. Assign the given value to all elements in the array.
3. Test the method with an `container::Array<int, Size>`.
4. Print the result.

---
