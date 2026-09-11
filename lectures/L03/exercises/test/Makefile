# L03 test suite: one binary per exercise set, plus the always-on language tests. What each
# variable means is in ci/suite.mk, which does the building; see README.md for what is tested.
#
# All four sets build one program, so the workspace is lectures/L03/exercises itself, laid out
# like lectures/L03/appendix/solutions: include/ and source/ beside this test/ directory.

BINARIES := language exercise1 exercise2 exercise3 exercise4

# Always built: the facts about inheritance and interfaces that Appendices A and B state.
language_NEEDS :=

# Exercise Set 1: the serial interface.
exercise1_NEEDS    := include/driver/serial/interface.hpp
exercise1_INCLUDES := include

# Exercise Set 2: the stub driver, declared in its header and defined in its source file.
exercise2_NEEDS    := include/driver/serial/stub.hpp source/driver/serial/stub.cpp
exercise2_INCLUDES := include
exercise2_SOURCES  := source/driver/serial/stub.cpp

# Exercise Set 3: the console driver, a singleton.
exercise3_NEEDS    := include/driver/serial/console.hpp source/driver/serial/console.cpp
exercise3_INCLUDES := include
exercise3_SOURCES  := source/driver/serial/console.cpp

# Exercise Set 4: sendMessage() and the program, which use both drivers.
exercise4_NEEDS    := source/main.cpp \
                      source/driver/serial/console.cpp source/driver/serial/stub.cpp
exercise4_INCLUDES := include source
exercise4_SOURCES  := source/driver/serial/console.cpp source/driver/serial/stub.cpp

include ../../../../ci/suite.mk
