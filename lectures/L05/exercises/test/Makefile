# L05 test suite: one binary per exercise set that has a program, plus the always-on language
# tests. What each variable means is in ci/suite.mk, which does the building; see README.md for
# what is tested. Exercise Set 2 is questions to answer in prose, so it has no binary.

BINARIES := language function_template class_template

# Always built: the facts about templates that Appendices A to C state, checked.
language_NEEDS :=

# Exercise Set 1: the clear and toggle function templates, in main.cpp.
function_template_NEEDS    := function_template/main.cpp
function_template_INCLUDES := function_template
function_template_REJECTS  := function_template_rejects_float_clear.cpp \
                              function_template_rejects_float_toggle.cpp

# Exercise Set 3: the Timer class template and its STM32 specialization, and the program that uses
# them once source/main.cpp exists too.
class_template_NEEDS    := class_template/include/driver/timer/timer.hpp
class_template_INCLUDES := class_template/include class_template/source

include ../../../../ci/suite.mk
