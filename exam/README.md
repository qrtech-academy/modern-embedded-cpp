# Written Examinations

Two complete three-hour papers for **Modern Embedded C++**, with worked solutions.

```text
paper_a.md              Paper A, questions only. Hand this out.
paper_a_solutions.md    Paper A, model answers with marks.
paper_b.md              Paper B, questions only.
paper_b_solutions.md    Paper B, model answers with marks.
```

---

## What these are for

The course as written has **no exam and nothing is marked**. What you get instead is the exercise
sets after every lecture, each with a published solution, and the Evaluation questions that close
every lecture README.

These papers do not replace that. **They are here for you to test your own skills and knowledge,
nothing more.** They gate nothing, they are not a qualification, and no part of the course requires
them. Nothing in the repository depends on them, and neither `make build` nor `make format-check`
knows they exist.

What they check is something the exercises cannot. Every exercise is done at a keyboard, with the
appendix open in the next tab and a compiler ready to say no. These papers ask what you can
reconstruct with none of that in front of you — which of the rules you have absorbed and which you
have merely been looking up.

**Take one after the course is over.** Both papers draw on all six lectures, so sitting one partway
through examines material nobody has taught you yet, and the result says more about how far you have
read than about what you have understood.

---

## The two papers

Both cover the whole course, L01 to L06, at the same weighting. They share no question. Either can
be used alone; use both as a main sitting and a resit, or in alternate years.

| Question | Topic                                        | Lectures | Marks   |
| -------- | -------------------------------------------- | -------- | ------- |
| 1        | From C to C++: linkage, keywords, vocabulary | L01      | 12      |
| 2        | Structs, drivers and object lifetime         | L01, L02 | 12      |
| 3        | Classes: what belongs to the type            | L02      | 12      |
| 4        | Copy, move, and what the compiler writes     | L02      | 13      |
| 5        | Inheritance, interfaces and polymorphism     | L03      | 13      |
| 6        | The factory, and who owns what               | L04      | 13      |
| 7        | Templates and compile-time programming       | L05      | 12      |
| 8        | Threads and synchronization                  | L06      | 13      |
|          |                                              |          | **100** |

**Both papers mix theory with code**, because the subject does. Roughly half the marks are for
prose — what a keyword guarantees, why an interface is worth its vtable, what a data race is — and
roughly half are for reading or writing C++ on paper.

The two papers differ in which half they lean on:

* **Paper A leans towards code that is put in front of you.** Seven of its questions hand you a
  snippet that compiles, passes a smoke test, and is wrong: a software timer with four defects
  against the course's rules, a driver missing `explicit`, a class whose copy constructor shares a
  heap block, an interface without a virtual destructor, a factory whose product is leaked, a
  bit-clearing template that wipes the upper half of a 64-bit register, and a producer/consumer
  that holds a mutex and still races. Naming the defect is half the marks; saying what it does to
  the running program is the other half.
* **Paper B leans towards code you write and design decisions you justify.** It asks for a C driver
  rewritten as a C++ class, an enumeration class and its validity check, a move constructor, a
  serial interface with every convention the appendix imposes on one, a stub factory returning
  `std::unique_ptr`, and a fixed-capacity container with its size in the type.

Neither paper asks anything of a compiler that a careful reader could not answer from the
appendices. Where a question turns on a rule that is easy to state and easy to get backwards — which
`const` goes in the header and which in the source file, what a user-declared destructor does to the
implicit move operations, why `std::move` moves nothing — the solution says so at length, because
that is where the marks and the understanding both are.

---

## Conventions the papers assume

Both papers state these in their own rubric, so a candidate never has to have read this file.

* **C++17 throughout.** Standard headers may be assumed included unless a question asks for them.
* **Code is marked on semantics, not syntax.** A missing semicolon, a forgotten `#include` or a
  brace in the wrong column costs nothing. A missing `virtual` on a destructor costs everything.
* **The course's house style is expected** where a question asks for it: `camelCase`, type names
  with a leading capital, `my` on private members and `our` on private statics, a leading capital on
  a `static constexpr` member, brace initialization, `noexcept` on driver methods, `[[nodiscard]]`
  on queries and repeated on overriding methods, and copy and move deleted on anything that stands
  for a unique hardware resource.
* **"State the consequence" means the consequence.** Naming a defect earns half the marks; saying
  what it does to the running program earns the other half. "This is wrong" scores nothing.
* **Where a question asks for a stated number of defects**, listing more is not penalised, but only
  that many are marked, so the strongest answers go first.

---

## Marking

Every solution is written to be marked by somebody who has read the appendices and does not
otherwise write C++ for a living, so each carries the reasoning rather than the answer alone. Marks
are shown per part.

Three conventions worth agreeing before a paper is marked:

* **Method carries the marks.** A correct diagnosis with a clumsy fix is worth more than a correct
  fix with no diagnosis. Several questions feed the next part, so an error should be followed
  through rather than penalised twice.
* **The named traps are worth full marks on their own.** A number of parts exist entirely to see
  whether a candidate walks into a specific mistake: a shallow copy constructor, a move assignment
  that forgets to release what it already owns, a non-virtual destructor in an interface, a
  discarded owning pointer, `1U << bit` on a 64-bit register, a mutex that one of the two threads
  never locks, a `std::thread` that is neither joined nor detached. Where a solution flags one of
  these, a candidate who walks into it loses those marks and no others.
* **The discussion parts are not decoration.** "State why" is where the course's actual content is,
  and a paper marked only on whether the code compiles would pass a candidate who has understood
  none of it.

---
