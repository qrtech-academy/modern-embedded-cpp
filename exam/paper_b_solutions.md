# Paper B - Solutions

Marks are shown per part. Method carries them: a correct diagnosis with a clumsy fix is worth more
than a correct fix with no diagnosis, and several later parts consume earlier answers, so an error
should be followed through rather than penalised twice.

Code is marked on semantics. Missing semicolons, missing includes and misplaced braces cost nothing.

---

## Question 1 - The vocabulary (12 marks)

### (a) 3 marks

**To the compiler, none.** `#include` behaves identically whatever the extension, and the extension
of a *header* has no bearing on the language it is compiled as. What decides that is the compiler
invocation — `gcc` against `g++` — or the extension of the **source** file that pulled the header
in.
*(1 mark)*

**By convention:** `.h` is shared ground. A `.h` file may be valid C, valid C++, or both, and the
extension alone does not say which. `.hpp` states that the header uses C++-only constructs —
classes, templates, namespaces, references — and therefore needs a C++ compiler.
*(1 mark)*

**The vendor's register header carries `.h`.** It is shipped for C users and included from C++
applications through an `extern "C"` block, so it must compile as either. `.hpp` would claim it is
C++-only, which is exactly the thing it must not be.
*(1 mark)*

### (b) 3 marks

**An anonymous namespace** restricts everything declared inside it — variables, functions, types and
compile-time constants — to a single translation unit. It replaces C's `static` on file-scope
functions and variables, and it is used in `.cpp` files to hide internal helpers.
*(1 mark)*

**The purpose of `driver::gpio`** is to avoid symbol collisions and to mirror the structure of the
project, so that `driver::gpio::init()` and `driver::timer::init()` can both exist and the reader
can see at the call site which module a function belongs to.
*(1 mark)*

```cpp
// Pre-C++17.
namespace driver
{
namespace gpio
{
void init() {}
} // namespace gpio
} // namespace driver

// Compact form, C++17 and later.
namespace driver::gpio
{
void init() {}
} // namespace driver::gpio
```

*(1 mark)*

### (c) 3 marks

**Three advantages**, any three of: a reference cannot be null; the call site needs no `&`; reading
it needs no `*`; the syntax at the point of use is that of an ordinary variable, so the code reads
the same whether the argument is a reference or a value.
*(2 marks)*

**What a pointer can do that a reference cannot:** be *reseated*. A reference is bound to one object
when it is initialized and can never be made to refer to another. A pointer can be reassigned, and
can be null to mean "nothing".

**Where it matters in the course:** `container::Vector<T>` holds its storage as `T* myData`, because
`resize()` and `pushBack()` call `std::realloc` and the block's address changes — a reference could
not follow it. The raw-pointer factory in L04 is the other case: it must hand back the address of a
heap object, and only a pointer can carry that.
*(1 mark)*

### (d) 3 marks

```cpp
((reg |= (static_cast<T>(1U) << bits)), ...);
```

*(2 marks. Award 1 for the loop form `for (const auto& bit : {bits...})`, which is correct but is
not what the question asked for.)*

*A note for the marker. The operand of a comma fold must be a cast-expression, so the assignment
needs its own pair of parentheses; written as `(reg |= (static_cast<T>(1U) << bits), ...)` — one
pair short, the form the L01 appendix warns against — GCC rejects it with "binary expression in
operand of fold-expression". **Award full marks either way.** The paper marks semantics, and the
missing parentheses are exactly the class of slip the rubric says costs nothing.*

**What the compiler generates.** The pack is expanded and the operation performed once per element,
folded together with the comma operator:

```cpp
reg |= (static_cast<T>(1U) << 1U);
reg |= (static_cast<T>(1U) << 2U);
reg |= (static_cast<T>(1U) << 3U);
```

This is one instantiation of `set`, not three — but the pack's length and element types are part of
the template arguments, so a call with a *different number* of bits instantiates a different
specialization. That is the code-size caveat for parameter packs.
*(1 mark)*

---

## Question 2 - The same driver, twice (12 marks)

### (a) 6 marks

```cpp
/**
 * @file PWM driver implementation.
 */
#pragma once

#include <cstdint>

namespace driver::pwm
{
/**
 * @brief PWM driver.
 */
class Pwm final
{
public:
    explicit Pwm(const std::uint8_t pin, const std::uint16_t period_ms = 20U) noexcept
        : myPin{pin}
        , myPeriod_ms{period_ms}
        , myEnabled{false}
    {}

    ~Pwm() noexcept = default;

    [[nodiscard]] std::uint8_t pin() const noexcept { return myPin; }
    [[nodiscard]] std::uint16_t period_ms() const noexcept { return myPeriod_ms; }
    [[nodiscard]] bool isEnabled() const noexcept { return myEnabled; }

    void setEnabled(const bool enabled) noexcept { myEnabled = enabled; }

    Pwm()                      = delete; // No default constructor.
    Pwm(const Pwm&)            = delete; // No copy constructor.
    Pwm(Pwm&&)                 = delete; // No move constructor.
    Pwm& operator=(const Pwm&) = delete; // No copy assignment.
    Pwm& operator=(Pwm&&)      = delete; // No move assignment.

private:
    /** Pin the PWM output is connected to. */
    const std::uint8_t myPin;

    /** PWM period in milliseconds. */
    const std::uint16_t myPeriod_ms;

    /** Indicate whether the PWM output is enabled. */
    bool myEnabled;
};
} // namespace driver::pwm
```

*Marks, 1 each, capped at 6:* `namespace driver::pwm` and `class Pwm final`; `explicit` constructor
with the default argument `= 20U`; `const` on `myPin` and `myPeriod_ms`, both private with the `my`
prefix; `[[nodiscard]]` on the three queries; trailing `const` on the three queries; `noexcept`
throughout; the deleted copy and move operations.

*Do not deduct for `#pragma once` or the includes; do deduct if a default argument appears anywhere
other than the declaration.*

### (b) 4 marks

*(1 mark each, any four.)*

1. **The null `self`.** Every C function has to test it, the test costs a branch on every call, and
   it has to be remembered in each new function somebody adds. A member function is called *through*
   an object; `backlight.setEnabled(true)` cannot be written without a `backlight`. The check is not
   optimized away — it ceases to have a case to check.
2. **A forgotten `pwm_init`.** `pwm_t backlight;` is a perfectly legal C object full of whatever was
   on the stack, and `pwm_set_enabled(&backlight, true)` will happily operate on it. A constructor
   cannot be skipped: the object does not exist until it has run.
3. **An error returned as data.** `pwm_is_enabled(NULL)` returns `false`, which is indistinguishable
   from a genuine "not enabled". The caller cannot tell a failure from an answer. The C++ query has
   no failure mode to disguise.
4. **Direct writes to the members.** `backlight.pin = 12;` compiles in C, and from then on the
   driver silently drives the wrong pin. Making the members private puts them out of reach, and
   `const` on `myPin` means even the class cannot change it after construction.
5. **Accidental duplication.** `pwm_t copy = backlight;` gives two objects for one peripheral. The
   deleted copy operations turn that into a compile error.

### (c) 2 marks

**Your header is `.hpp`.** It contains a class, `noexcept`, default arguments and deleted operations
— none of which a C compiler can read — so the extension should say so.
*(1 mark)*

**The vendor's header stays `.h`**, because it must remain compilable as C. Before your C++ driver
can call into that library, its declarations need a guarded `extern "C"` block:

```c
#ifdef __cplusplus
extern "C" {
#endif
/* declarations */
#ifdef __cplusplus
}
#endif
```

Without it the C++ compiler mangles the names and the linker cannot match them against the C
library's unmangled symbols.
*(1 mark)*

---

## Question 3 - Enumerations, files, and lifetime (12 marks)

### (a) 5 marks

```cpp
namespace driver::gpio
{
/**
 * @brief GPIO direction configuration.
 */
enum class Direction : std::uint8_t
{
    Input,       ///< Input without pull-up.
    InputPullup, ///< Input with pull-up enabled.
    Output,      ///< Output.
    Count,       ///< Number of supported directions.
};

[[nodiscard]] constexpr bool isDirectionValid(const Direction direction) noexcept
{
    return static_cast<std::uint8_t>(Direction::Count) > static_cast<std::uint8_t>(direction);
}
} // namespace driver::gpio
```

*(2 marks: 1 for the enumeration with a fixed underlying type, 1 for the check. `Count` is the
"whatever is needed" the question asked for — the check has nothing to compare against without
it, and it must be last.)*

**Three reasons for an enumeration class**, any three of:
*(2 marks)*

* **The enumerators are scoped.** They are named `Direction::Input`, not `Input`, so they cannot
  collide with anything and need no `GPIO_DIRECTION_` prefix. The prefix convention that C forces on
  you is built into the language here.
* **No implicit conversion to an integer.** A plain `enum` decays to `int` and will silently take
  part in arithmetic, in comparisons against an unrelated enumeration, and in overload resolution.
  An `enum class` requires an explicit `static_cast`, which is why the validity check above has two
  of them, and why it is obvious when a conversion is happening.
* **The enumerators do not leak into the enclosing scope**, so the namespace stays clean.
* **The underlying type is always fixed** — `int` unless you name another — so the size is known
  and the enumeration can be forward-declared. A plain `enum` gets that only if you write the type
  out, which C++11 allows and almost nobody does.

**What fixing the underlying type buys.** The size is known, minimal and the same on every compiler
— one byte instead of whatever `int` happens to be — which matters when the value is a member of a
struct that goes into a frame or is stored in RAM in quantity. Choosing an *unsigned* type also
means no negative value can exist, which reduces validation to the single upper-bound comparison
above.
*(1 mark)*

### (b) 4 marks

*(0.5 marks for each of the seven, counting `= default` and `= delete` separately, plus 0.5 for a
coherent statement of the principle.)*

| Construct                | Where                | Why                                                        |
| ------------------------ | -------------------- | ---------------------------------------------------------- |
| `explicit`               | Header only          | Belongs to the declaration; repeating it on the out-of-class definition is an error. |
| Default argument         | Header only          | May be specified once per scope; repeating it in the definition is an error. |
| `[[nodiscard]]`          | Header only          | It is the declaration callers see. Repeating it is legal but redundant; the course's rule is declaration only. |
| Trailing `const`         | **Both**             | It is part of the function's type. Omit it in the source file and you are defining a *different* function, which does not match anything declared. |
| `noexcept`               | **Both**             | Every declaration must carry the same exception specification; omit it in the source file and the definition is rejected for having a different one. Since C++17 it is also part of the function type. |
| `= default` / `= delete` | Header only          | Written in the class, they are complete definitions with no body to place anywhere else; `= delete` must be on the first declaration. |

**The principle:** anything that is part of the function's **type** must appear in both places, or
the definition does not define the function that was declared. Anything that describes how the
function may be *called* — `explicit`, default arguments, `[[nodiscard]]` — belongs to the
declaration alone.

*Also creditable, from the same appendix: `const` on a parameter passed **by value** is dropped from
the header declaration and kept in the definition, because it constrains only the function body and
is not part of the type.*

### (c) 3 marks

*(0.5 marks each for the three pairs, 1.5 for the hazard.)*

1. **A local object.** Constructed where control reaches its declaration; destroyed at the closing
   brace of its scope, in reverse order of construction.
2. **An object at namespace scope.** Constructed during program start-up, **before `main` is
   entered**; destroyed **after `main` returns**, in reverse order of construction.
3. **An object created with `new`.** Constructed as part of the `new` expression; destroyed only
   when `delete` is called on the pointer. If `delete` never happens, the destructor never runs —
   that, and not merely the unreleased memory, is what a leak costs you.

**The hazard with namespace-scope objects.** Their constructors run before `main`, and therefore
before any clock, power or peripheral initialization the program performs in `main`. A driver object
at namespace scope may configure a peripheral that is not yet clocked. On top of that, the order of
construction between objects in *different* translation units is unspecified, so one such object
cannot safely depend on another. And because they are destroyed after `main` returns, anything still
running at that point — a detached thread, for example — is touching objects whose lifetime is
ending.

---

## Question 4 - What the compiler writes for you (13 marks)

### (a) 4 marks

**Why the copy constructor works.** A `const` non-static data member can be **initialized**, once,
in the member initializer list, at the moment the object comes into existence. A copy constructor is
building a brand-new object, so `myPin{other.myPin}` is a legal initialization and nothing is being
modified.
*(1.5 marks)*

**Why the copy assignment operator cannot be written.** Assignment operates on an object that
already exists and whose `myPin` was fixed when it was constructed. The operator would have to
write `myPin = other.myPin;`, which is ill-formed — a `const` object is never assignable. There is
no ordering of statements that gets round this: the member simply cannot be made to hold a different
value for the rest of its life. The simplified operator in the appendix compiles precisely because
it copies only `myState`: the target keeps its own pin, so the result is not a copy.
*(1.5 marks)*

**What the compiler does.** It still implicitly *declares* a copy assignment operator, but because
the class has a `const` non-static data member, that operator is **defined as deleted**. `led2 =
led1;` is therefore a compile error about a *deleted function* rather than an assignment to a
`const`: GCC reports `use of deleted function 'Gpio& Gpio::operator=(const Gpio&)'`, and only its
follow-up note names the `const` member, which is why the message can be puzzling if you have not
met the rule. The same applies to the move assignment operator. The class in the appendix deletes
all four explicitly anyway, which states the intent and produces a clearer diagnostic.
*(1 mark)*

### (b) 4 marks

```cpp
Buffer(Buffer&& other) noexcept
    : myData{other.myData}
    , mySize{other.mySize}
{
    other.myData = nullptr;
    other.mySize = 0U;
}
```

*(2 marks: 1 for taking the members, 1 for clearing `other`.)*

Clearing `other.myData` is not tidiness. `other` is still a live object and its destructor will run
and `delete[]` whatever it holds; leaving the old address there means the block this object now owns
is freed behind its back. `delete[] nullptr` is defined and does nothing, so nulling the pointer
makes the moved-from object harmless while keeping it destructible — which is the whole contract a
moved-from object owes.

**What move assignment must do that a move constructor need not:** release the resource the target
**already owns** before taking the new one, and guard against self-assignment.
*(1 mark)*

**Why the constructor is exempt.** It is building an object that did not exist a moment ago. It owns
nothing yet, so there is nothing to free; and the object under construction cannot possibly be the
same object as `other`, so `this != &other` is not merely unnecessary, it can never be false.
*(1 mark)*

### (c) 5 marks

**`= default`** asks the compiler for its own implementation of a special member function, stated
explicitly. The reader can see that the function exists and was intended, and you can attach
`noexcept`, `virtual` or an access level to it while you are there.

**`= delete`** removes the function entirely. Any attempt to use it is a compile error that names
the deleted function, rather than a program that compiles and does something you did not want.
*(2 marks)*

```cpp
Gpio()                       = delete; // No default constructor.
Gpio(const Gpio&)            = delete; // No copy constructor.
Gpio(Gpio&&)                 = delete; // No move constructor.
Gpio& operator=(const Gpio&) = delete; // No copy assignment.
Gpio& operator=(Gpio&&)      = delete; // No move assignment.
```

**The design reason:** a `Gpio` object stands for one physical pin. A copy would be a second object
that believes it owns the same pin and acts on it independently; nothing in the hardware was
duplicated, only the software's idea of it. Deleting the operations makes the mistake a build
failure. The deleted default constructor is there for a related reason — a GPIO with no pin number
is not a thing.
*(1.5 marks)*

**What `~Gpio() noexcept = default;` nevertheless changes.** Declaring a destructor at all — even
defaulted, even in the class body — makes it a **user-declared** destructor, and a class with a
user-declared destructor gets **no implicitly declared move constructor and no implicitly declared
move assignment operator**. The class silently loses its move operations and falls back to copying
wherever a move would have been used. The copy operations are still generated, so nothing fails to
compile and nothing warns.

That is precisely why the course writes all of them out: once you touch one special member function,
the compiler stops supplying the others on the terms you assumed, and the only readable answer is to
state every one of them.
*(1.5 marks)*

---

## Question 5 - Designing against an abstraction (13 marks)

### (a) 5 marks

```cpp
/**
 * @file Serial driver interface.
 */
#pragma once

#include <cstdint>

namespace driver::serial
{
/**
 * @brief Serial driver interface.
 */
class Interface
{
public:
    virtual ~Interface() noexcept = default;

    virtual void init(std::uint32_t baudrate) noexcept        = 0;
    virtual void write(const char* message) noexcept          = 0;
    [[nodiscard]] virtual bool isInitialized() const noexcept = 0;
};
} // namespace driver::serial
```

*(2 marks for the interface; the trailing `const` on the query and `= 0` on all three are the parts
worth checking.)*

**Four conventions**, 0.75 marks each (the appendix lists five; the fifth is below):

1. **The destructor is `virtual`.** Concrete drivers are deleted through an `Interface*`. Without
   `virtual` that is undefined behavior, and in practice the derived destructor never runs, so
   whatever the driver acquired is never released.
2. **The destructor is `= default`.** The interface owns nothing and has nothing to clean up, so the
   compiler's implementation is exactly right. Writing it explicitly shows it was decided rather
   than forgotten — and it is the only way to state `virtual` on it.
3. **Every method is pure virtual (`virtual ... = 0`).** This is what makes the class abstract, so
   nobody can create an `Interface`, and what obliges every concrete driver to provide the method.
   The interface defines *what*, never *how*.
4. **`noexcept` on every method.** It is part of the function type, and an override may not weaken
   it, so declaring it here forces every implementation on every platform to be `noexcept`. On an
   embedded target that is exactly the constraint you want to impose from the abstraction downwards.

*A fifth, worth crediting in place of any of the above: `[[nodiscard]]` on the query, so that a
caller who asks whether the port is initialized and then ignores the answer is warned, where the
compiler supports it (the appendix notes that GCC does not warn for a call made through the
interface itself). Note that the attribute is **not** inherited — it belongs to the declaration it
is written on — so every overriding implementation must repeat it, which is what the course does.*

### (b) 4 marks

**`private` against `protected`.** Neither is accessible from outside the class. The difference is
that a **derived** class can reach `protected` members and cannot reach `private` ones. That is the
entire difference, and it is why a base class intended to be inherited from puts its shared state
and helpers under `protected`.
*(1 mark)*

**The three kinds of inheritance:**
*(1.5 marks)*

* **Public.** Public members of the base stay public in the derived class and protected stay
  protected. Used when the derived class *is a* specialization of the base. By far the most common.
* **Protected.** Public and protected members of the base become protected in the derived class.
  Like private inheritance, except that classes derived further down still get access.
* **Private.** Everything from the base becomes private in the derived class. Used when the base is
  an implementation detail of the derived class rather than something it presents to the world.

**A concrete driver uses public inheritance**, because the point of the exercise is that a
`driver::serial::Esp32s3` *is a* `driver::serial::Interface` and can be handed to anything expecting
one. With private or protected inheritance, the conversion from derived to base is not accessible to
outside code, so passing the driver as an `Interface&` would not compile — the abstraction would
exist and be unusable.
*(1.5 marks)*

### (c) 4 marks

**It does not compile.** `Interface` has pure virtual members and is therefore an **abstract class**.
A parameter of abstract class type cannot be declared, because passing by value would require
constructing one, and an abstract class cannot be instantiated. The compiler says so directly:
*"cannot declare parameter 'serial' to be of abstract type 'driver::serial::Interface'"*, followed
by a note listing the pure virtual functions responsible.
*(1.5 marks)*

**Had the base not been abstract** — every method with a default implementation — it would have
compiled and **sliced**. Only the base subobject of the argument is copied into the parameter; the
derived part is discarded and the parameter's dynamic type is the base. `serial.write(...)` would
then call the base version, not the driver's override. Virtual dispatch works through references and
pointers; a by-value copy has already thrown away the thing that makes it work. The mechanism is
**object slicing**, and it is the more dangerous of the two outcomes because it compiles.
*(1.5 marks)*

**The corrected signature:**

```cpp
void sendGreeting(driver::serial::Interface& serial) noexcept
```

**Why not `const`.** `write()` changes the state of the port and is therefore not a `const` method,
so a `const Interface&` could not call it. A `const` reference to an interface restricts you to its
query methods — here, `isInitialized()` alone.
*(1 mark)*

---

## Question 6 - Factories (13 marks)

### (a) 3 marks

An arrow reads "depends on", that is, includes the header of:

```text
app::logic::Logic          --> driver::factory::Interface, driver::gpio::Interface
driver::factory::Interface --> driver::gpio::Interface     (returned by gpio())
driver::factory::Esp32s3   --> driver::factory::Interface  (implements)
                           --> driver::gpio::Esp32s3       (creates)
driver::factory::Stub      --> driver::factory::Interface  (implements)
                           --> driver::gpio::Stub          (creates)
driver::gpio::Esp32s3      --> driver::gpio::Interface     (implements)
driver::gpio::Stub         --> driver::gpio::Interface     (implements)
```

*(1.5 marks for the four layers and the direction of the arrows. The architecture overview in the
appendix draws the same four layers as a stack, top to bottom; a candidate who reproduces it has the
layers but has drawn who sits above whom, not what depends on what.)*

**The dependency that makes it testable** is the one from `Logic` to the **interfaces** rather than
to any concrete type. Because `Logic` names only `driver::factory::Interface` and
`driver::gpio::Interface`, the entire lower half of the graph can be replaced without `Logic`
changing, or even recompiling for a different reason. Logic points only at abstractions, and nothing
points from an abstraction down to an implementation. That is the property, and swapping in stubs is
one consequence of it.
*(1.5 marks)*

### (b) 4 marks

```cpp
#pragma once

#include <cstdint>
#include <memory>

#include "driver/factory/interface.hpp"
#include "driver/gpio/stub.hpp"

namespace driver::factory
{
class Stub final : public Interface
{
public:
    Stub() noexcept           = default;
    ~Stub() noexcept override = default;

    [[nodiscard]] std::unique_ptr<gpio::Interface> gpio(const std::uint8_t pin) noexcept override
    {
        // Ignore the pin number since it is not used by the stub.
        (void)(pin);
        return std::make_unique<gpio::Stub>();
    }

    Stub(const Stub&)            = delete;
    Stub(Stub&&)                 = delete;
    Stub& operator=(const Stub&) = delete;
    Stub& operator=(Stub&&)      = delete;
};
} // namespace driver::factory
```

*(1 mark each, capped at 4:* `final` and `: public Interface`; `override` on both the method and the
destructor; the return type matching the interface exactly and `std::make_unique`; `(void)(pin)`;
`[[nodiscard]]` repeated on the override; the deleted copy and move operations.)*

*On `[[nodiscard]]`: the attribute belongs to the declaration it is written on and is **not**
inherited by an override, so the course repeats it on every overriding method. A candidate who omits
it has lost one of the six available marks, not four — but a candidate who omits it and explains
that it is inherited from the interface has got the rule backwards and should be told so.*

Worth stating: `std::make_unique<gpio::Stub>()` produces a `std::unique_ptr<gpio::Stub>`, which
converts implicitly to `std::unique_ptr<gpio::Interface>` because `Stub` derives from `Interface`.
That conversion is what makes the `return` statement work, and it is safe only because
`gpio::Interface` has a virtual destructor.

### (c) 4 marks

*(0.75 marks each for the four, 1 mark for the `std::move` answer.)*

1. **Ownership is stated in the type.** `std::unique_ptr` says "you own this, exclusively". A raw
   pointer says nothing at all; the answer lives in a comment, or nowhere.
2. **Release is automatic.** The `unique_ptr`'s destructor deletes the object when it goes out of
   scope. The raw pointer needs a matching `delete` on every path out, and the class holding it
   needs a destructor written for the purpose.
3. **It cannot be copied.** `std::unique_ptr`'s copy constructor is deleted, so two owners of one
   object cannot be created by accident — the double delete that a copied raw pointer produces at
   run time becomes a compile error instead.
4. **Transfer is explicit.** Handing ownership on requires `std::move`, so the moment ownership
   changes hands is visible in the source. A raw pointer changes owner by plain assignment, and
   nothing marks it.

**Where `std::move` becomes necessary.** Not in the L04 constructor: `factory.gpio(pin)` returns a
temporary, and initializing a member from a temporary already moves. It is needed as soon as the
source is a **named** `unique_ptr` — a logic class taking one as a parameter and storing it,
`myLed{std::move(led)}`, or reassigning `myLed = std::move(other)`.

**Afterwards the source holds `nullptr`.** It is still a valid object; it simply owns nothing.
Dereferencing it is undefined behavior, which is why a moved-from pointer should not be used again.

### (d) 2 marks

**The cost: dynamic allocation.** The factory creates its objects with `new` or
`std::make_unique`, which means a heap on a target that may not want one: fragmentation over a long
uptime, an allocation time that is not deterministic, and a failure mode that terminates the program
(Paper A, Question 1(d)). It also adds a level of indirection and one more abstraction to read
through.
*(1 mark)*

**The alternative shown in L04**: construct the concrete drivers directly, as objects with automatic
or static storage, and pass references into the logic class.

```cpp
driver::gpio::Esp32s3 led{ledPin};
driver::gpio::Esp32s3 button{buttonPin};
app::logic::Logic logic{led, button};
```

with `Logic` holding `driver::gpio::Interface&` members. No heap is involved at all, everything is
sized at compile time, and the logic is still written entirely against the interface. The appendix
says as much: this is "very efficient when dynamic memory allocation should be limited". What it
gives up is choosing the driver type at run time, and the single point of construction that a
factory provides once there are many drivers to make.
*(1 mark)*

---

## Question 7 - A container with its size in the type (12 marks)

### (a) 6 marks

```cpp
/**
 * @file Fixed-capacity array implementation.
 */
#pragma once

#include <cstddef>

namespace container
{
template<typename T, std::size_t Size>
class Array final
{
    static_assert(Size > 0U, "Array size must be greater than zero!");

public:
    Array() noexcept
        : myData{}
        , mySize{}
    {}

    [[nodiscard]] bool push(const T& element) noexcept
    {
        // Reject the element if the array is already full.
        if (Size <= mySize) { return false; }
        myData[mySize++] = element;
        return true;
    }

    [[nodiscard]] std::size_t size() const noexcept { return mySize; }
    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return Size; }

    T& operator[](const std::size_t index) noexcept { return myData[index]; }
    const T& operator[](const std::size_t index) const noexcept { return myData[index]; }

private:
    /** Element storage. */
    T myData[Size];

    /** Number of elements currently held. */
    std::size_t mySize;
};
} // namespace container
```

*Marks, 1 each:* `template<typename T, std::size_t Size>` with `Size` as a **non-type** parameter;
the `static_assert` with a message; storage as a member array rather than a pointer, with no
allocation anywhere; `push` returning `false` when full and marked `[[nodiscard]]`; both
`operator[]` overloads, the second `const` and returning `const T&`; `size()` and `capacity()`,
`const` and `[[nodiscard]]`.

*Creditable extras, not required: `constexpr` on `capacity()`, since the answer is a template
argument and is known at compile time; the copy and move operations, which for a container are a
design decision rather than an automatic deletion.*

### (b) 4 marks

**Why the definitions cannot live in `array.cpp`.** The compiler generates code for
`Array<std::uint8_t, 8U>` only where it sees a use with those arguments **and** the definition of
what to generate. `array.cpp` is compiled alone, contains no use, and generates nothing. The
translation unit that does use it sees only a declaration and emits calls to functions nobody
defined. The build therefore fails at **link** time with `undefined reference`, not at compile time
— which is why the error mentions nothing about templates.
*(2 marks)*

**Three distinct types.** A non-type template argument is part of the type exactly as a type
argument is, so changing `8U` to `16U` produces a different type just as surely as changing
`std::uint8_t` to `std::uint16_t` does. The three are unrelated: no conversion between them, no
common base, and a function taking `Array<std::uint8_t, 8U>&` will not accept `b`.
*(1 mark)*

**For the binary:** every member function that is actually used is generated once per instantiation,
so `push`, once it is called on all three, exists three times over. Code size scales with the
number of instantiations, and a non-type parameter makes those very easy to multiply — one set per
buffer size anybody in the project happens to pick.
*(1 mark)*

### (c) 2 marks

**The advantage: no heap.** The storage is a member array whose size is known at compile time, so
the object lives on the stack or in `.bss`. There is no allocation to fail, no fragmentation, no
`std::realloc`, and no non-deterministic call on a path with a deadline. `push` cannot fail for want
of memory — only for want of capacity, and it says so by returning `false`.
*(1 mark)*

**What you give up.** The capacity is part of the type, so `Array<T, 8U>` and `Array<T, 16U>` are
unrelated and no ordinary function can accept both — anything wanting to must become a template
itself, which multiplies the instantiations again. And the full capacity is paid for in RAM whether
it is used or not.
*(1 mark)*

---

## Question 8 - Sharing state between threads (13 marks)

### (a) 3 marks

**The three conditions**, all of which must hold:
*(1.5 marks)*

1. Two or more threads access the same memory location.
2. At least one of those accesses is a **write**.
3. Nothing synchronizes them.

**What the standard says:** the behavior of the **whole program** is undefined.
*(0.5 marks)*

**Why that is stronger than "the value may be wrong."** Undefined behavior does not promise you a
wrong value. The compiler is entitled to assume no race exists and to optimize on that basis: it may
cache the variable in a register, hoist a load out of a loop, reorder the two writes, or read the
variable twice and get different answers within one expression. The failure therefore need not look
anything like a torn value, and the code that breaks may be nowhere near the racing access. It also
means the program can pass every test at `-O0` and fail in the field at `-O2` — "it worked when we
tested it" is not evidence of anything.
*(1 mark)*

### (b) 4 marks

**Is the data race gone? Yes.** Every shared access is now an atomic operation, so condition (3) of
part (a) no longer holds and the program has no undefined behavior from this cause.
*(1 mark)*

**Is the program correct? No.** Atomicity per *variable* is not atomicity per *transaction*. The two
fields have to change together, and nothing makes the pair of writes indivisible.
*(1 mark)*

**An interleaving:**
*(1 mark)*

TX writes `data` and then `newData`, exactly as it did under the mutex:

```text
TX: data.store(1); newData.store(true)   <- message 1
RX: newData.load() == true               <- RX sees message 1
RX: data.load() == 1                     <- and consumes it
TX: data.store(2); newData.store(true)   <- message 2 is announced
RX: newData.store(false)                 <- RX clears the flag: message 2 is lost
```

RX's test of `newData` and its clearing of it are two separate atomic operations, and TX can run
between them. Let TX write `data` for message 2 between RX's two loads instead, and announce it
after RX has cleared the flag, and RX prints message 2 twice and message 1 never. Each individual
access is atomic; the check-then-act sequence is not.

**Two things a mutex provides that a pair of atomics does not:**
*(1 mark)*

1. **Mutual exclusion over a region**, not over a single access. Several statements run without
   another thread interleaving between them, which is exactly what a multi-field update needs.
2. **Memory synchronization between threads.** Everything written inside a critical section is
   guaranteed visible to the next thread that acquires the same mutex, and not only the fields you
   remembered to make atomic.

### (c) 3 marks

**`std::launch::async`** — the task runs immediately, in a new thread.
**`std::launch::deferred`** — no thread is created; the task runs synchronously on the **calling**
thread, the first time `get()` or `wait()` is called on the future.
*(1 mark)*

**With no policy passed**, the implementation is free to choose either, and nothing in the source
says which you got.
*(0.5 marks)*

**The failure.** Code written as "start the checksum, print the frame, then collect the result"
silently becomes fully sequential when the implementation picks `deferred`: the frame is printed
first, and only then does the checksum begin — on the same thread, inside `get()`. Nothing crashes
and nothing warns; the concurrency you believed you had is simply absent, and the measurement that
was supposed to show an improvement shows none. Worse, if `get()` is never called, a deferred task
never runs at all. The fix is to always pass the policy explicitly.
*(1.5 marks)*

*Worth knowing, not required: the destructor of a future returned by `std::async` with the async
policy blocks until the task completes, so discarding the future also turns the call back into a
synchronous one.*

### (d) 3 marks

**The sequence**, with three threads:
*(1.5 marks)*

1. The **low**-priority thread locks a mutex and begins a long operation.
2. The **high**-priority thread wakes, tries to lock the same mutex, and blocks.
3. The **medium**-priority thread wakes and preempts the low-priority thread. It does not use the
   mutex; it is doing entirely unrelated work. The scheduler simply gives the CPU to the
   higher-priority runnable thread, which is a normal and correct scheduling decision.
4. The medium-priority thread runs on, so the low-priority thread cannot finish and cannot release
   the lock.
5. The high-priority thread stays blocked — not on its own work, and not on anything the medium
   thread holds, but because the medium thread is delaying the release.

The high-priority task ends up waiting on the medium-priority one. The intended scheduling order has
been inverted, and the highest-priority thread — often the one with the deadline, resetting the
watchdog — is the one that misses it.

**Why `std::mutex` cannot prevent it.** It has no **priority inheritance**: no mechanism to
temporarily raise the lock holder's priority to that of the highest-priority thread waiting on it,
so that the holder can finish and release before anything of middling priority cuts in. The
preemption in step 3 is a scheduler decision the mutex plays no part in. `std::thread` compounds
this by exposing no notion of priority at all — setting one means going through `native_handle()`
and the platform API — so the standard library gives you nothing to work with here.
*(1 mark)*

**What does fix it:** the platform's priority-inheritance mutex — an RTOS mutex configured for
priority inheritance, or a POSIX mutex with the `PTHREAD_PRIO_INHERIT` protocol. If you are mixing
thread priorities on an RTOS, that is what belongs in the code; `std::thread` itself is not the
problem.
*(0.5 marks)*

---
