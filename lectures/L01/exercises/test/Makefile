# L01 test suite: one binary per exercise set, plus the always-on language tests. What each
# variable means is in ci/suite.mk, which does the building; see README.md for what is tested.

BINARIES := language exercise1 exercise2 exercise3 exercise4

# Always built: the facts about C++ that Appendix B states, checked.
language_NEEDS :=

# Exercise Set 1: debug::log and app::delay_ms, in one main.cpp.
exercise1_NEEDS    := exercise1/main.cpp
exercise1_INCLUDES := exercise1

# Exercise Set 2: the software timer, driver/timer.hpp, and the program that uses it.
exercise2_NEEDS    := exercise2/driver/timer.hpp
exercise2_INCLUDES := exercise2

# Exercise Set 3: swap, in an anonymous namespace in main.cpp.
exercise3_NEEDS    := exercise3/main.cpp
exercise3_INCLUDES := exercise3

# Exercise Set 4: the clear and toggle templates, in an anonymous namespace in main.cpp.
exercise4_NEEDS    := exercise4/main.cpp
exercise4_INCLUDES := exercise4
exercise4_REJECTS  := exercise4_rejects_float.cpp

include ../../../../ci/suite.mk
