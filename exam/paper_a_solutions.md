# Paper A - Solutions

Marks are shown per part. Method carries them: a correct diagnosis with a clumsy fix is worth more
than a correct fix with no diagnosis, and several later parts consume earlier answers, so an error
should be followed through rather than penalised twice.

Code is marked on semantics. Missing semicolons, missing includes and misplaced braces cost nothing.

---

## Question 1 - Crossing the line between C and C++ (12 marks)

### (a) 4 marks

**The mechanism is name mangling.** C++ supports function overloading, so several functions may
share a name and differ only in their parameter lists. The compiler therefore encodes the parameter
types into the symbol it emits, turning `halGpioInit()` into something like `_Z11halGpioInitv`. 
C has no overloading and emits the name unchanged.

The vendor's library was compiled by a C compiler and exports `halGpioInit()`. Your translation 
unit was compiled by a C++ compiler, saw an ordinary C++ declaration, and emitted a call to the mangled name. The two do not match, so the linker finds nothing. The error message is itself the clue: the linker prints the missing symbol demangled, `halGpioInit()` with parentheses, which only happens when the symbol was a C++ one.
*(2 marks)*

**The fix.** Tell the C++ compiler to use C linkage for those declarations:

```c
#ifdef __cplusplus
extern "C" {
#endif

void halGpioInit(void);
void halGpioWrite(uint8_t pin, bool state);

#ifdef __cplusplus
}
#endif
```

*(1 mark)*

**Why the guard.** `extern "C"` is C++-only syntax. A C compiler reading the unguarded header would
fail on it. `__cplusplus` is defined only when the file is compiled as C++, so the C compiler sees
the preprocessor strip both blocks and is left with plain declarations.
*(1 mark)*

Credit either of these two additions: that `extern "C"` changes only linkage and does not make
C++-only constructs legal, so the contents must remain valid C; or that a header written this way is
exactly the kind that keeps the `.h` extension rather than `.hpp`, because it is meant to be
compiled as both.

### (b) 3 marks

Three advantages, any three of:

* **Type safety.** `constexpr std::uint32_t cpuFrequency{16000000U}` has a type the compiler checks
  at every use. `#define CPU_FREQUENCY 16000000` is a textual substitution with no type at all, and
  the error it eventually produces names the expansion, not the macro.
* **Scoping.** A `constexpr` obeys namespaces, class scope and block scope, so two modules may each
  have their own `bufLen`. A macro is invisible to all of that: it is live from its `#define` to the
  end of the translation unit and collides with anything of the same name, including member
  variables.
* **Debuggability and diagnostics.** The value exists in the symbol table as a named entity of known
  type. A macro has been erased before the compiler proper ever sees it.

*(1 mark for any two, 2 for all three)*

**What a runtime `const` cannot do.** A `constexpr` value may be used anywhere a *constant
expression* is required: an array bound, a non-type template argument, a `static_assert` condition,
a `case` label. A `const` variable whose initializer is not itself a constant expression, for
example `const std::uint32_t n{readTrimRegister()};`, cannot be used in any of those places.
`constexpr` also states the requirement at the point of definition, so it is diagnosed there rather
than at the first use that needed it.
*(1 mark)*

### (c) 3 marks

**What it guarantees.** That no exception will propagate out of the function.
*(1 mark)*

**If one does anyway.** The program calls `std::terminate()` immediately. There is no handler
search and no guarantee of unwinding — the failure is turned into an abort at the point it occurred.
*(1 mark)*

**Two reasons to disable exceptions in an embedded build**, any two of:

* Binary size: unwind tables and the exception-handling runtime cost flash the device may not have.
* Determinism: the cost of a throw depends on the depth of the stack to be unwound and is not
  statically bounded, which is unacceptable on a path with a deadline.
* A simpler runtime environment; the error-handling scheme is then return codes, status flags or
  error callbacks, which are visible in the signature.

*(1 mark)*

### (d) 2 marks

The two are not in conflict because `noexcept` never meant "cannot throw". It means "if an exception
tries to leave here, terminate".

L01's rule is about a function whose failure the caller could reasonably handle: if a function might
fail by throwing and that failure is recoverable, taking away the caller's ability to catch it is
wrong. L04 takes the opposite view of the same fact deliberately. `std::make_unique` can throw
`std::bad_alloc`; on a device where the heap is exhausted, there is nothing useful to recover to.
On such a target the surrounding code is typically compiled without exception support and has no
handler anywhere.
*(1 mark)*

**The behavior selected** is therefore: on allocation failure, `std::terminate()` at the point of
allocation, rather than an exception propagating through code that has no handler for it. A
predictable stop instead of undefined drift.
*(1 mark)*

---

## Question 2 - Object lifetime, and a driver that nearly works (12 marks)

### (a) 4 marks

```text
Init 9
Init 13
Init 7
Inner
Release 7
Release 13
Outer
Release 9
```

*(2 marks; deduct 1 for one line out of place, 2 for a scrambled second half)*

**The two rules.**

1. A destructor runs when the object goes out of scope, which for `button` and `buzzer` is the
   closing brace of the inner block, not the end of `main`. This is why `Release 7` and
   `Release 13` appear **before** `Outer`.
2. Objects in the same scope are destroyed in the **reverse** order of their construction. `buzzer`
   was built last, so it is released first.

*(2 marks, 1 each)*

Both rules together are the mechanism behind RAII, which is part (c).

### (b) 6 marks

*(1.5 marks per defect: 0.5 for naming it, 0.5 for the consequence, 0.5 for the corrected line.)*

**1. The constructor is not `explicit`.**

It takes a single argument, so it doubles as a conversion from `std::uint16_t` to `Timer`. Given
`void arm(driver::Timer timer);`, the call `arm(500U)` compiles silently and builds a temporary
timer out of a number. The compiler stops being able to tell a timer from an integer, which is
precisely the mistake it exists to catch.

```cpp
explicit Timer(const std::uint16_t timeout_ms) noexcept
```

**2. The member initializer list is in the wrong order.**

Members are initialized in the order they are **declared**, never in the order written in the list.
The declaration order is `myTimeout_ms`, `myCounter_ms`, `myRunning`; the list says
`myCounter_ms`, `myTimeout_ms`, `myRunning`. Nothing goes wrong at runtime here because no
initializer reads another member, but the compiler warns (`-Wreorder`, which `-Wall` enables, so
under the course's `-Werror` the snippet would not even build) and the list is now actively
misleading: the day somebody writes `myCounter_ms{myTimeout_ms}` it will read a member that has not
been initialized yet, and the code will look correct.

```cpp
: myTimeout_ms{timeout_ms}
, myCounter_ms{0U}
, myRunning{false}
```

**3. `timeout_ms()` is a query but is not `const`.**

It reads a member and changes nothing, but without the trailing `const` it cannot be called on a
`const Timer&`. Any function that takes the timer read-only is then unable to ask it its own
timeout, which forces callers to pass non-const references and gives away the protection that
`const` was there to provide. It should carry `[[nodiscard]]` too, as `elapsed()` already does.

```cpp
[[nodiscard]] std::uint16_t timeout_ms() const noexcept { return myTimeout_ms; }
```

**4. Copy and move operations are not deleted.**

A `Timer` stands for one hardware timer. The compiler supplies a copy constructor, so
`driver::Timer backup{timer};` compiles and produces a second object counting independently against
the same peripheral, with its own idea of when the timeout is due.

```cpp
Timer(const Timer&)            = delete; // No copy constructor.
Timer(Timer&&)                 = delete; // No move constructor.
Timer& operator=(const Timer&) = delete; // No copy assignment.
Timer& operator=(Timer&&)      = delete; // No move assignment.
```

*A candidate who also notes that no `Timer() = delete;` is needed here — declaring any constructor
already suppresses the implicit default one — has understood the rule rather than copied the
pattern. Credit it, but not at the expense of one of the four above.*

### (c) 2 marks

**RAII**, Resource Acquisition Is Initialization: a resource is acquired when the object is created
and released automatically when the object is destroyed. Because destruction is driven by scope and
the compiler emits it on every exit path, the release cannot be forgotten.
*(1 mark)*

**An embedded example**, any one of: disabling the interrupt the driver enabled; returning a pin to
its reset direction and pull configuration; stopping a timer or PWM channel so it stops toggling a
pin; releasing a DMA channel or a bus lock; powering down the peripheral clock.
*(1 mark)*

---

## Question 3 - Classes, and what belongs to the type (12 marks)

### (a) 2 marks

**The formal difference is the default access level.** Members of a `class` are `private` unless
stated otherwise; members of a `struct` are `public`. Everything else — constructors, destructors,
methods, inheritance, templates, encapsulation — is available to both. *(A candidate who also names
the default access of a base class, `private` for `class` and `public` for `struct`, is right and
should be credited.)*
*(1 mark)*

**The course convention.** `struct` is for simple data-oriented types: plain data, or a holder for
`static constexpr` constants. `class` is for anything with real encapsulation, inheritance,
polymorphism, or custom copy and move semantics.
*(1 mark)*

### (b) 3 marks

**What the compiler does.** `playAlarm` wants a `Buzzer` and is handed a `std::uint16_t`. The
single-argument constructor is a viable implicit conversion, so the compiler calls it to build a
`Buzzer{4000U}` directly in the by-value parameter (in C++17 no copy is made) and runs the function.
The program prints `Alarm at 4000 Hz` and looks entirely correct.
*(1 mark)*

**Why it is a hazard.** The call site says "play the alarm on this buzzer" and the program does
something else: it manufactures a buzzer out of a number, uses it once and throws it away. Whatever
buzzer the caller meant was never involved; any hardware setup in the constructor, and any teardown
in the destructor, ran for a throwaway object. Worse, the conversion accepts
anything convertible to `std::uint16_t` — a pin number, a duration in milliseconds, a loop counter —
so an entire class of type errors passes review and compiles clean.
*(1 mark)*

**The change:** `explicit`.

```cpp
explicit Buzzer(const std::uint16_t frequency_hz) noexcept
```

After that, `playAlarm(4000U)` is a compile error and `playAlarm(Buzzer{4000U})` is the only way to
say it, which at least says what it means.
*(1 mark)*

### (c) 4 marks

```cpp
public:
    /** Maximum number of GPIO instances supported by this MCU. */
    static constexpr std::uint8_t MaxInstances{40U};

private:
    /** Number of currently active GPIO instances. */
    static std::uint8_t ourInstanceCount;
```

*(2 marks: 1 for the pair, 1 for the naming — leading capital on the `static constexpr`, `our`
prefix on the private static.)*

**Where each is defined.** `MaxInstances` needs no definition anywhere. Since C++17 a `static
constexpr` data member is implicitly `inline`, so the line inside the class is both its declaration
and its definition and the header may be included in any number of translation units.

`ourInstanceCount` is a plain `static` and must be defined exactly once, in a source file:

```cpp
std::uint8_t Gpio::ourInstanceCount{0U};
```

Without it the linker reports an undefined reference; put it in the header instead and every
translation unit that includes it defines the same symbol, which is a duplicate-definition error.
The two differ because one is a compile-time constant that never needs storage and the other is a
mutable object that needs exactly one.
*(1 mark)*

**What a `static` method cannot access.** A static method is called on the class, not on an object,
so it has no `this` pointer. It can therefore only touch other static members — never `myPin`,
`myState`, or any non-static method. Asking it to would be asking "which object's `myState`?", a
question with no answer.
*(1 mark)*

### (d) 3 marks

**`final` on a class forbids deriving from it.** Any `class X : public Gpio` is then a compile
error.
*(1 mark)*

**Why a driver normally has it.** A concrete driver is the end of the chain: nothing is meant to
specialize an ATmega328P LED further. Sealing it says so, removes the question of whether its
destructor needs to be virtual, and lets the compiler devirtualize calls when it knows the static
type is the final one.
*(1 mark)*

**Why you might leave it off.** So that a test build can derive from it and override behavior —
the appendix names exactly this case. You cannot inherit from a `final` class even for a fake, so
sealing a class is a decision about who is allowed to test it.
*(1 mark)*

---

## Question 4 - The class that owns something (13 marks)

### (a) 3 marks

```cpp
Buffer(const Buffer& other) noexcept;             // Copy constructor.
Buffer(Buffer&& other) noexcept;                  // Move constructor.
Buffer& operator=(const Buffer& other) noexcept;  // Copy assignment operator.
Buffer& operator=(Buffer&& other) noexcept;       // Move assignment operator.
```

*(3 marks, or 2 if the names are right but a signature is not: the `&&` on the move operations and
the returned reference on the assignments are the parts worth checking.)*

### (b) 6 marks

**The copy constructor is a shallow copy.** *(3 marks)*

It copies the pointer, not the bytes. After `util::Buffer b{a};`, `a.myData` and `b.myData` hold the
same address. There is now one 64-byte block and two objects that each believe they own it. Writes
through `b` are visible through `a`, which is already wrong. The fatal part is destruction: both
destructors run `delete[]` on that address. The first frees the block; the second frees a block that
is no longer allocated. That is undefined behavior, and in practice a heap corruption abort or a
silent corruption of whatever the allocator has since handed out.

The fix is to allocate a new block and copy `mySize` bytes into it — or, for a class representing a
hardware buffer, to `= delete` the copy constructor and stop the question from arising.

**The move assignment operator leaks, then dangles.** *(3 marks)*

The operator has three defects, and `c = std::move(a);` shows the first two:

1. **It leaks.** `c` already owned a 32-byte block. `myData` is overwritten without `delete[]`
   first, so those 32 bytes are unreachable for the rest of the program's life.
2. **It leaves the source owning what it gave away.** `other.myData` is not cleared, so `a` is
   still a live object whose destructor will `delete[]` the 64-byte block that `c` now points at.
   Whichever is destroyed first leaves the other holding a dangling pointer, and the second
   destructor double-frees.
3. **It has no self-assignment guard.** `a = std::move(a)` is harmless only because the operator is
   already broken; once `delete[] myData` is added in front, self-assignment frees the buffer and
   then assigns the freed pointer to itself.

**Taken together**, after the four lines in the question, one 64-byte block is referenced by `a`,
`b` and `c`, the 32-byte block is lost, and three destructors will attempt to free the same address.
*(No extra marks, but a candidate who states this has read the whole snippet rather than each
operation separately.)*

*Also creditable, not required: the constructor is `noexcept` and calls `new`, which can throw
`std::bad_alloc`, so an allocation failure terminates. That is a defensible choice on an embedded
target — it is the L04 position from Question 1(d) — but it is a choice, and it should be a
deliberate one.*

### (c) 2 marks

```cpp
Buffer& operator=(Buffer&& other) noexcept
{
    if (this != &other)
    {
        delete[] myData;

        myData = other.myData;
        mySize = other.mySize;

        other.myData = nullptr;
        other.mySize = 0U;
    }
    return *this;
}
```

*(1 mark for releasing the current buffer and guarding self-assignment, 1 mark for leaving `other`
in a valid, destructible state.)*

Nulling `other.myData` is what makes the moved-from object safe: `delete[] nullptr` is defined and
does nothing, so `other`'s destructor is now harmless. "Valid but unspecified" is the contract a
moved-from object owes — it must still be destructible.

### (d) 2 marks

**`std::move` moves nothing.** It is a cast: it returns an rvalue reference to its argument,
`static_cast<T&&>`, and does not touch the object. Its entire effect is on overload resolution —
it makes the compiler prefer an overload taking `T&&`, which is how a move constructor or move
assignment operator gets chosen over the copying one. All of the actual work happens inside that
operation.
*(1 mark)*

Two consequences worth stating: an object is unchanged by `std::move` itself and only changes if the
selected operation changes it; and `std::move` on a `const` object yields a `const` rvalue, which
binds to `const T&` and therefore silently **copies**.
*(1 mark)*

*Worth knowing, and a good place to say it: `Buffer` has a user-declared copy constructor, a
user-declared move assignment operator and a user-declared destructor, so no move constructor is
implicitly declared. `util::Buffer d{std::move(a)};` therefore selects the copy constructor — the
broken, shallow one — and the word `move` at the call site is doing nothing at all. For the same
reason the copy assignment operator is implicitly deleted, so `b = a;` does not compile. This is the
argument for declaring all five or deleting all four, never some of them.*

---

## Question 5 - Interfaces, and the keyword that is missing (13 marks)

### (a) 4 marks

`~Interface()` is **not** `virtual`. Deleting an object of derived type through a pointer to a base
class with a non-virtual destructor is **undefined behavior**.
*(2 marks)*

What a typical implementation actually does is worth spelling out, because it is the reason nobody
notices: the call is resolved statically from the pointer's type, so only `~Interface()` runs.
`~Bme280()` never runs, `delete[] mySamples` never happens, and 16 × 2 = 32 bytes leak on every
sensor destroyed. The program does not crash, does not warn and passes its tests; on a device that
constructs and destroys sensors over months of uptime, it runs out of heap and dies somewhere
unrelated.
*(2 marks)*

*Also creditable: `operator delete` is handed the wrong type and, in a sized-deallocation build, the
wrong size. Under AddressSanitizer this is exactly what the program reports —
`new-delete-type-mismatch` — and, if it is allowed to continue past that report
(`ASAN_OPTIONS=halt_on_error=0`), `32 byte(s) leaked in 1 allocation(s)`, one `std::uint16_t[16]` per
sensor.*

*A note for the marker: GCC and Clang do warn here, `-Wdelete-non-virtual-dtor`, which `-Wall`
enables. Two things blunt it. The warning fires at the line with the `delete`, not at the interface
where the mistake was made, so it lands in code the interface's author may never compile; and it
disappears entirely once the `delete` is inside a `std::unique_ptr`'s deleter in some other header.
A candidate who says "the compiler would have told you" is half right and should be told which
half.*

### (b) 3 marks

**The fix:**

```cpp
virtual ~Interface() noexcept = default;
```

*(1 mark)*

**The other line.** The derived destructor should have been written:

```cpp
~Bme280() noexcept override { delete[] mySamples; }
```

With `override` on it, the broken version does not compile at all: `override` asserts that this
function overrides a virtual one, and if the base destructor is not virtual there is nothing to
override, so the compiler rejects it and points at the exact line. That is why the appendix marks
the derived destructor `override` — it is not decoration, it is a tripwire that converts a silent
runtime leak into a build failure.
*(2 marks)*

### (c) 3 marks

The base declares `read() const`. The colleague's `read()` is not `const`, so it is a **different**
signature and does not override anything.

**With `override`:** a hard compile error at that line, along the lines of *"method declared
'override' but does not override a base class method"*. The mistake is reported where it was made.
*(1.5 marks)*

**Without `override`:** the class silently fails to implement the pure virtual `read() const`, so it
is still abstract. The compiler says nothing at the declaration; the error arrives later and
somewhere else, at the first attempt to create one — *"invalid new-expression of abstract class
type"*, or *"cannot declare variable to be of abstract type"* — followed by a note listing
`read() const` as the pure virtual still unimplemented. The non-const `read()` also **hides** the base
name for lookup on the derived type, so even where it compiles it does not do what the reader
expects.
*(1.5 marks)*

*Also creditable: the same trap exists for `noexcept`. An override may not have a looser exception
specification than the function it overrides, so dropping `noexcept` from an override of a
`noexcept` virtual is an error too — which is exactly why the appendix puts `noexcept` in the
interface, to force it on every implementation.*

### (d) 3 marks

**Two advantages**, any two of:
*(2 marks)*

* **Testability.** The logic can be exercised against a stub with no hardware present. This is the
  advantage that pays for the other ones, and the reason the factory in L04 exists.
* **Replaceability.** A second chip means a new class and no change to any existing code that uses
  the interface.
* **Reusability.** Code written against the abstraction is not tied to one MCU and can move between
  projects.

**The runtime cost.** Each object carries a hidden pointer to its class's vtable — one pointer of
storage per object — and a call through the interface is an indirect call through that table rather
than a direct one, so it normally cannot be inlined. That is the whole cost, and it is the same
mechanism the C version in the linked example builds by hand out of a struct of function pointers.
*(1 mark)*

---

## Question 6 - The factory, and who owns what (13 marks)

### (a) 4 marks

**Purpose.** A factory centralizes the creation of hardware-specific objects, so the decision about
*which concrete driver type* exists in exactly one place instead of being scattered through the code
that uses them. It plays the role a HAL or BSP plays in a C project.
*(1.5 marks)*

**What "depends only on interfaces" means.** `system::logic::Logic` names
`driver::gpio::Interface` and `driver::factory::Interface` and never names `Esp32s3` or `Stub`. It
does not include their headers, does not know how a pin is configured on any particular chip, and
does not change or need recompiling when a platform is added. It knows it will be given something
that can `read`, `write` and `toggle`; it does not know what.
*(1.5 marks)*

**What has to change to move to stubs.** One line in `main` — `driver::factory::Esp32s3 factory{};`
becomes `driver::factory::Stub factory{};` — and the corresponding `#include`. Nothing under
`system::logic` changes at all.
*(1 mark)*

### (b) 5 marks

**Defect 1: the discarded owning pointer.** *(2.5 marks)*

```cpp
factory.gpio(4U);
```

`gpio()` allocates with `new` and hands ownership of the result to the caller. Here the returned
pointer is thrown away, so nothing in the program holds the object's address and nothing can ever
`delete` it: a leak, and for a real driver, a pin that has been configured with no way to release
it. This is exactly the case the factory interface marks `gpio()` `[[nodiscard]]` for.

*A note for the marker, not required of the candidate: do not treat the attribute as a guarantee
that the build would have caught this. Even where a diagnostic is produced it is a warning and not
an error, and whether one appears at all depends on the compiler and on where the call is resolved.
The attribute is not inherited by an override — which is why the course repeats it on every
overriding method — and GCC 13 diagnoses only a call it can resolve statically. This very line is
flagged, because `Esp32s3` is `final` and its override repeats the attribute (and under the course's
`-Werror` it stops the build), but the same call made through a `driver::factory::Interface&`
produces nothing at all. `[[nodiscard]]` documents an intention; holding the pointer is what
enforces it.*

**Defect 2: `Logic` is copyable and owns raw pointers.** *(2.5 marks)*

```cpp
system::logic::Logic backup{logic};
```

`Logic` declares a destructor that deletes both drivers, but declares neither a copy constructor nor
a deleted one, so the compiler generates one that copies the two pointer **values**. `backup` and
`logic` now hold the same two driver objects — both drive the same physical pins — and when the two
are destroyed, at the end of `main` if `run()` ever returns, both destructors run and each deletes
both objects. Two deletes per object: undefined behavior, and the failure lands during shutdown,
which is the worst possible place to find it. Under AddressSanitizer, with a `run()` that returns,
the program reports a heap-use-after-free inside `Logic::~Logic()`.

Nothing warns about this by default. The implicit copy constructor of a class with a user-declared
destructor is *deprecated*, but the diagnostic for it is `-Wdeprecated-copy-dtor`, which neither
`-Wall` nor `-Wextra` turns on.

The rule broken is the rule of three: a class that needs a destructor needs its copy operations
written or deleted. For a class holding a unique hardware resource, deleted — which is what L04
Appendix A does, and why the paper is careful to say that *this* version declares nothing but the
constructor, the destructor and `run()`. A candidate who answers "it does not compile, because
`Logic` deletes its copy constructor" has read the appendix rather than the question; award the
marks if they then state what the deletion is there to prevent, since that is the same knowledge:

```cpp
Logic(const Logic&)            = delete;
Logic(Logic&&)                 = delete;
Logic& operator=(const Logic&) = delete;
Logic& operator=(Logic&&)      = delete;
```

### (c) 4 marks

**`Logic`'s destructor disappears entirely.** With

```cpp
std::unique_ptr<driver::gpio::Interface> myLed;
std::unique_ptr<driver::gpio::Interface> myButton;
```

each member destroys the object it owns when `Logic` is destroyed, so there is no `delete` left to
write and therefore no destructor left to write. The code that could be forgotten no longer exists.
*(2 marks)*

**Defect 2 becomes a compile error.** `std::unique_ptr` represents exclusive ownership and its copy
constructor is deleted. A class whose member cannot be copied cannot have an implicit copy
constructor, so `Logic`'s is defined as deleted, and `system::logic::Logic backup{logic};` no longer
compiles. The double delete has been moved from run time to build time, and nobody had to remember
to write the four `= delete` lines: the member's own semantics propagated to the class that holds
it.
*(2 marks)*

*Note for completeness: defect 1 does not become an error. Discarding the returned `std::unique_ptr`
still compiles. What changes is the consequence — instead of leaking, the temporary is destroyed at
the end of the full expression, so the object is created and immediately destroyed. No leak, but the
driver you asked for does not exist.*

---

## Question 7 - Templates (12 marks)

### (a) 4 marks

**Why.** A template is not code; it is a pattern from which code is generated. The compiler produces
an actual function only when it sees a use with concrete template arguments, and it can only do that
where the **definition** is visible. `bit_util.cpp` is compiled on its own, contains no use of the
template, and therefore generates nothing at all. The translation unit that calls
`clear<std::uint8_t>` sees only the declaration, assumes the function exists somewhere and emits a
call to a symbol nobody ever defined. The failure is at link time, not compile time, which is why
the error is `undefined reference` rather than anything that names templates.
*(2 marks)*

**The two arrangements:**
*(2 marks, 1 each)*

1. Declaration and definition together in the header, e.g. all of `container/vector.hpp`.
2. Declaration in `container/vector.hpp`, definition in `container/impl/vector_impl.hpp`, with
   `#include "container/impl/vector_impl.hpp"` at the **bottom** of the first. The compiler sees the
   same thing; the reader gets a header that is readable.

*Explicit instantiation in the `.cpp` for each type used is a third technically correct answer and
should be credited, with the observation that it requires naming every type in advance and so gives
up the genericity the template was for.*

### (b) 5 marks

**The value:**

```text
reg = 0x00000000FFFFFFFB
```

*(2 marks)*

**How it got there.** `1U` is `unsigned int`, 32 bits wide. `1U << 2` is `4U`, still `unsigned int`.
`~4U` is `0xFFFFFFFB`, still 32 bits. Only at the `&=` is that value converted to `std::uint64_t`,
and because it is **unsigned** it is zero-extended: `0x00000000FFFFFFFB`. The mask therefore carries
32 leading zeros that were never asked for.

```text
0xFFFFFFFFFFFFFFFF & 0x00000000FFFFFFFB = 0x00000000FFFFFFFB
```

Bit 2 is cleared as requested — and so is every bit from 32 to 63. The function did what it was
told and half the register with it. It went unnoticed for a year because for `T` of 32 bits or
fewer, the mask is at least as wide as the register and no bits are lost.
*(1 mark)*

**The second failure.** For `bit >= 32`, `1U << bit` shifts an `unsigned int` by at least its own
width, which is **undefined behavior** — not merely a wrong answer. In practice many targets take
the shift count modulo 32, so `clear(reg, 32U)` clears bit 0 of a 64-bit register, on top of the
upper half it already loses.
*(1 mark)*

**The corrected body:**

```cpp
reg &= static_cast<T>(~(static_cast<T>(1U) << bit));
```

The cast forces the shift and the complement to happen in `T`'s own width, which is what the
original meant to say. *(This is the same `static_cast<T>(1U)` that the L01 `set()` function uses,
and the reason it is there.)*
*(1 mark)*

*Worth noting and creditable: for a narrow `T` such as `std::uint8_t` the shift still promotes to
`int`, which is harmless here — `~(1 << 2)` is `-5`, and converting that to `std::uint8_t` gives
`0xFB` — but a `bit` outside the register's own width silently clears nothing. Neither version
defends against a caller asking for bit 9 of an 8-bit register.*

### (c) 3 marks

**Two versions**, `clear<std::uint8_t>` and `clear<std::uint32_t>`. `reg1` and `reg3` have the same
type, so the third call reuses the first instantiation, and `bit` is an ordinary function parameter
rather than a template parameter, so its differing values generate nothing extra.
*(1 mark)*

**The implication.** Code size grows with the number of distinct **types** a template is used with,
not with the number of call sites. Ten calls on `std::uint8_t` cost one function; one call each on
five register widths costs five. On a resource-constrained target that is the number to watch.
*(1 mark)*

**Where a template is the wrong tool**, either of:
*(1 mark)*

* When the implementation must be chosen at **run time** — a driver selected from a configuration
  byte or a detected board revision. A template resolves everything at compile time, so the choice
  has to exist in the binary as a virtual call behind an interface.
* When the body is large and the type list is long: N copies of a big function cost more flash than
  one indirect call ever costs cycles.

---

## Question 8 - Two threads and a shared struct (13 marks)

### (a) 6 marks

*(2 marks per defect: 1 for identifying it, 1 for the consequence.)*

**1. TX never locks the mutex.**

`txThread` writes `shared.data` and `shared.newData` with no lock at all. A mutex excludes only the
threads that lock it; RX locking on its own excludes nobody. Both fields are read by RX while TX
writes them, which is a data race and therefore undefined behavior. The concrete failure is also
easy to state: nothing orders the two writes, so RX can see `newData == true` while `data` still
holds the previous value, and print a message that was never sent.

This is also the case where making each field `std::atomic` would **not** be a fix. The two fields
have to change together, as one unit, and only a mutex can make a multi-variable update atomic.

**2. `stop` is a plain `bool` shared between two threads.**

TX writes it, RX reads it, and nothing synchronizes the two: a data race, undefined behavior. Beyond
the standard's verdict, the practical failure is specific. Nothing inside the RX loop can change
`stop`, so the compiler is entitled to load it once and hoist the test out of the loop entirely, and
RX then spins forever after TX has finished. The fix is `std::atomic<bool> stop{false};` with
`stop.store(true)` and `stop.load()`. This is the case an atomic is exactly right for: one
independent flag.

**3. RX busy-waits.**

The loop locks and unlocks the mutex as fast as the CPU allows, with no sleep and no wait, to
discover that nothing has changed. Between messages that is 10 ms of a core spent doing nothing —
on a battery-powered target, the difference between idle current and full current, and on a
single-core MCU it also delays every other thread. The structure that fixes it is a
`std::condition_variable`, which is part (c).

*A candidate who instead offers "a detached thread may still be touching `shared`, `mutex` and
`stop` while those namespace-scope objects are being destroyed after `main` returns" has named a
real fourth defect and should be given the marks for it.*

### (b) 3 marks

**At the closing brace of `main`**, `t2` is destroyed while still **joinable** — it was never joined
and never detached. The destructor of a joinable `std::thread` calls `std::terminate()`. The
program does not return 0; it aborts.
*(1.5 marks)*

**The two legal ways to end a `std::thread`'s lifetime** are `join()`, which blocks the caller until
the thread finishes, and `detach()`, which severs the association and lets the thread run on its
own. Exactly one of them must happen before the `std::thread` object is destroyed.
*(1 mark)*

**Which this program should use: `join()`, on both threads.** `detach()` on `t1` is the second half
of the same bug — `main` returns immediately afterwards, so a detached thread is left running
against objects whose lifetime is ending.
*(0.5 marks)*

### (c) 4 marks

```cpp
void rxThread() noexcept
{
    while (!stop.load())
    {
        std::unique_lock<std::mutex> lock{mutex};
        condition.wait(lock, [] { return shared.newData || stop.load(); });

        // Terminate if the wait was ended by the stop flag rather than by data.
        if (!shared.newData) { break; }

        std::printf("RX: %u\n", shared.data);
        shared.newData = false;
    }
}
```

*(2 marks for the structure and the predicate.)*

**The predicate** is `shared.newData || stop.load()` — "there is data, or we are shutting down".
The second term is what makes the wait exitable; without it the thread has no reason to ever wake.

**Why `std::unique_lock` and not `std::lock_guard`.** `wait()` must release the mutex while the
thread sleeps and re-acquire it before returning, otherwise no other thread could ever take the lock
to produce the data being waited for. `std::lock_guard` locks in its constructor and unlocks in its
destructor and offers nothing in between. `std::unique_lock` exposes the unlock and relock
operations that `wait()` needs.
*(1 mark)*

**What the writer of the stop flag must do.** Set the flag while holding the mutex, and then call
`condition.notify_all()`:

```cpp
{
    std::lock_guard<std::mutex> lock{mutex};
    stop.store(true);
}
condition.notify_all();
```

Once TX has finished, nothing else will ever notify; a receiver already asleep inside `wait()`
would stay there forever, holding a predicate that would now return true if only something woke it
up to evaluate it. `notify_all()` wakes every waiter so each can re-check and exit. The lock matters
even though `stop` is atomic: without it the store and the notification can both land after the
receiver has evaluated the predicate as false but before it has gone to sleep, the notification is
lost, and the receiver sleeps forever anyway.
*(1 mark)*

*Two things worth saying to a candidate who got the rest right. The predicate overload of `wait()`
is also what handles spurious wakeups: it re-checks the condition on every wake and goes back to
sleep if it is false, which is why this is written with a predicate rather than a bare
`wait(lock)`. And TX must now take the mutex around its two writes and call `notify_one()` after
releasing it — the fix to (a)(1) and this part are the same change seen from the two ends.*

---
