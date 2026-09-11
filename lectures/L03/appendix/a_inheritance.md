# Appendix A

## Inheritance in C++
Inheritance is a fundamental principle in C++ that makes it possible to create new classes that build upon existing classes. In this way, one can "reuse" all or parts of existing classes and thereby avoid writing the same code for several different classes.

In terms of terminology:
* A class that inherits from another is called a derived class (or subclass).
* The class that is inherited from is called a base class (or superclass).

Assume that a base class named `driver::Gpio` has been created in order to easily control a GPIO pin on a microcontroller:

```cpp
namespace driver
{
class Gpio
{
public:
    explicit Gpio(std::uint8_t pin) noexcept;
    ~Gpio() noexcept;

    [[nodiscard]] bool read() const noexcept;
    void write(bool enable) noexcept;

    Gpio()                       = delete;
    Gpio(const Gpio&)            = delete;
    Gpio(Gpio&&)                 = delete;
    Gpio& operator=(const Gpio&) = delete;
    Gpio& operator=(Gpio&&)      = delete;

protected:
    // Add GPIO-specific methods for initialization and such here.
    void init() noexcept;

    /** The pin the GPIO is connected to.*/
    std::uint8_t myPin;
};
} // namespace driver
```

Note that the keyword `protected` is used instead of `private` in this case:
* The reason for this is that we want potential derived classes to be able to access the parts of the class that would otherwise be private (provided that so-called public inheritance is used).
* The difference between private and protected members is therefore that derived classes can access protected members, but not private ones. However, neither protected nor private members are accessible otherwise.

### Types of inheritance
There are three types of inheritance in C++:

* **Public inheritance** The by far most common type. Public and protected members in the base class retain their access rights in the derived class. Used when the derived class is a specialization of the base class.
* **Private inheritance:** All members of the base class become private in the derived class. Used when the base class should only be used internally within the derived class.
* **Protected inheritance** Public and protected members in the base class become protected in the derived class. It therefore works like private inheritance, with the difference that potential subclasses of the current derived class also gain internal access to the base class.

Below is an example of public inheritance, where a class named `driver::Led` inherits the GPIO functionality from the class `driver::Gpio`:
* The functions `write` and `read` from the base class `driver::Gpio` can therefore be used by instances of the class `driver::Led`, i.e. LEDs.
* A new method, `toggle`, has been added to toggle the LED:

```cpp
namespace driver
{
class Led final : public Gpio
{
public:
    explicit Led(std::uint8_t pin) noexcept;
    ~Led() noexcept;

    void toggle() noexcept;

    Led()                      = delete;
    Led(const Led&)            = delete;
    Led(Led&&)                 = delete;
    Led& operator=(const Led&) = delete;
    Led& operator=(Led&&)      = delete;
};
} // namespace driver
```

Note that:
* The class `driver::Led` publicly inherits the class `driver::Gpio` by writing `: public Gpio` after the class name. Since both classes are in the same namespace, we do not need to use the prefix `driver` before `Gpio`.
* The keyword `final` has been added directly after the class name `driver::Led` so that this class cannot be inherited. As a rule of thumb, it is a good idea to add `final` if the class is not intended to be inherited. However, in some cases it may be desirable to inherit the implementation for testing purposes, and in that case this keyword must be omitted.

By inheriting the class `driver::Gpio`, we avoided having to add a member variable that stores the LED's pin number as well as routines for writing to or reading the LED output signal. Instead, we can easily create an LED via the constructor of the class `driver::Led` and then use the methods `write()` and `read()` to control the LED:

```cpp
// Create an LED connected to pin 9.
driver::Led led1{9U};

// Enable the LED.
led1.write(true);
```

In the same way, we could have created a derived class named `driver::Button` in order to control a button.

In this case, a method has been added to enable/disable interrupts when the push button is pressed.
A corresponding method for checking whether interrupts are enabled has also been added.
Since the button should not be controllable, we ensure that the method `write()` is private;
we cannot remove inherited methods, but we can change their visibility.

```cpp
namespace driver
{
class Button final : public Gpio
{
public:
    explicit Button(std::uint8_t pin) noexcept;
    ~Button() noexcept;

    void enableInterrupt(bool enable) noexcept;
    [[nodiscard]] bool isInterruptEnabled() const noexcept;

    Button()                         = delete;
    Button(const Button&)            = delete;
    Button(Button&&)                 = delete;
    Button& operator=(const Button&) = delete;
    Button& operator=(Button&&)      = delete;

private:
    void write(bool enable) noexcept;
};
} // namespace driver
```

Through this class, we can then easily read the LED output signal, enable interrupts, and so on:

```cpp
// Create a button connected to pin 13.
driver::Button button1{13U};

// Enable interrupt at pressdown.
button1.enableInterrupt(true);

// Toggle led1 if the button is pressed.
if (button1.read()) { led1.toggle(); }
```

---
