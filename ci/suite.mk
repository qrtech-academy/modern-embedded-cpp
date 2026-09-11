# How a lecture's test suite is built and run. Every lectures/LNN/exercises/test/Makefile sets
# a few variables describing its tests and then includes this file, so there is one definition of
# how a suite works and six short descriptions of what each one tests.
#
# A lecture declares:
#
#   BINARIES         One test binary per exercise set, plus the lecture's always-on one. Binary
#                    <b> is built from <b>_test.cpp. Sets are separate binaries because each set
#                    is a separate program: two sets may both define driver::gpio::Led, and one
#                    binary holding both would be an ODR violation, not a test.
#   <b>_NEEDS        The files, relative to the workspace, that the set asks you to write. The
#                    binary is built once they all exist, and reported as SKIP until then. Empty
#                    for the always-on binary.
#   <b>_INCLUDES     Include directories, relative to the workspace.
#   <b>_SOURCES      Source files to compile in, relative to the workspace; wildcards allowed.
#                    main.cpp is always left out, since the test binary has a main() of its own.
#                    A test that needs functions from main.cpp includes it with main renamed, and
#                    never calls that renamed main; the program itself is <b>_PROGRAM.
#   <b>_FLAGS        Extra compiler flags, e.g. -pthread.
#   <b>_PROGRAM      The source files of the set's program, main.cpp included, relative to the
#                    workspace; wildcards allowed. Once they exist, the program is built on its
#                    own, as build/<b>_program, and its path is passed to the tests as PROGRAM, so
#                    a test can run it and check its output (support/program.hpp). A program is
#                    run rather than its main() called, because only the real ::main may leave out
#                    its return statement.
#   <b>_REJECTS      Files, in this directory, that must NOT compile against your code, because
#                    the exercise asks for a static_assert that rejects them.
#
# And then:
#
#   make                  test the code in your workspace, lectures/LNN/exercises
#   make SOLUTIONS=1      test the lecture's reference solutions instead
#   make clean            remove the test binaries

SHELL := /bin/bash

# Repository root. ci/test.sh passes an absolute path; the default is what makes `make` work when
# you are standing in the suite's own directory.
ROOT_DIR ?= ../../../..

# The QAcademy Test framework, a submodule at libs/test, and the course's own test support.
QACADEMY_TEST_DIR ?= $(ROOT_DIR)/libs/test
QACADEMY_TEST_LIB := $(QACADEMY_TEST_DIR)/libqacademy_test.a
SUPPORT_DIR       := $(ROOT_DIR)/libs/support

# Where the code under test lives: your workspace by default, the reference solutions with
# SOLUTIONS=1. The second is how CI proves that every test in the course passes against a correct
# answer; the first is what you run.
LECTURE := $(notdir $(abspath ../..))
ifeq ($(SOLUTIONS),1)
WORK_DIR  := ../../appendix/solutions
WORK_NAME := lectures/$(LECTURE)/appendix/solutions
else
WORK_DIR  := ..
WORK_NAME := lectures/$(LECTURE)/exercises
endif

# How long one test binary may run before it counts as hung. Generous: the slowest tests in the
# course run threads for a fraction of a second.
TEST_TIMEOUT ?= 60s

CXX_COMPILER := g++
CXX_FLAGS    := -std=c++17 -Wall -Werror -I$(QACADEMY_TEST_DIR)/include -I$(SUPPORT_DIR)/include
LD_FLAGS     := -L$(QACADEMY_TEST_DIR) -lqacademy_test

# What a binary needs, what is missing of it, and what it compiles with.
needs    = $(addprefix $(WORK_DIR)/,$($(1)_NEEDS))
missing  = $(filter-out $(wildcard $(call needs,$(1))),$(call needs,$(1)))
shown    = $(patsubst $(WORK_DIR)/%,$(WORK_NAME)/%,$(1))
includes = $(addprefix -I$(WORK_DIR)/,$($(1)_INCLUDES))
sources  = $(filter-out %/main.cpp,$(wildcard $(addprefix $(WORK_DIR)/,$($(1)_SOURCES))))
compile  = $(CXX_COMPILER) $(CXX_FLAGS) $($(1)_FLAGS) $(call includes,$(1))

# The same for a set's program: the files it needs (its wildcard entries aside, which may match
# nothing yet), what is missing of them, and what it is built from.
literal         = $(foreach f,$(1),$(if $(findstring *,$(f)),,$(f)))
program_missing = $(filter-out $(wildcard $(addprefix $(WORK_DIR)/,$(call literal,$($(1)_PROGRAM)))),\
                    $(addprefix $(WORK_DIR)/,$(call literal,$($(1)_PROGRAM))))
program_sources = $(wildcard $(addprefix $(WORK_DIR)/,$($(1)_PROGRAM)))

# Build a set's program, if it has one and its files exist, and leave the -DPROGRAM that tells the
# tests where it is in $$program.
build_program = program=""; \
  $(if $($(1)_PROGRAM),$(if $(call program_missing,$(1)),\
    echo "SKIP  $(1): its program test waits for $(call shown,$(firstword $(call program_missing,$(1))))";,\
    if $(call compile,$(1)) $(call program_sources,$(1)) -o build/$(1)_program; then \
      program='-DPROGRAM="$(abspath build)/$(1)_program"'; \
    else \
      echo "FAIL  $(1): your program does not compile on its own."; failed=1; \
    fi;))

# What to say when a binary does not compile. Kept out of the recipe because make would split the
# messages at their commas.
not_compiling = echo "FAIL  $(1): the tests do not compile against $(WORK_NAME)."; \
                echo "      Compare the names, types and signatures above with the exercise.";
suite_broken  = echo "FAIL  $(1): the always-on tests do not compile, which is a fault in the suite.";

ENABLED := $(foreach b,$(BINARIES),$(if $(call missing,$(b)),,$(b)))
SKIPPED := $(filter-out $(ENABLED),$(BINARIES))

.PHONY: default test build run clean FORCE
.NOTPARALLEL:

default: test

# Forced rather than guarded on the .a existing, so that the sub-make, which knows whether
# anything changed, is always the one to decide.
FORCE:
$(QACADEMY_TEST_LIB): FORCE
	@if [ ! -f $(QACADEMY_TEST_DIR)/Makefile ]; then \
	  echo "error: test framework not found in libs/test. Run 'git submodule update --init'." >&2; \
	  exit 1; fi
	@$(MAKE) -s -C $(QACADEMY_TEST_DIR) lib

# Build and run every binary whose files exist, one after another, so that one set's compile
# error does not hide another set's result. The exit status is non-zero if anything failed.
test: $(QACADEMY_TEST_LIB)
	@mkdir -p build
	@failed=0; \
	$(foreach b,$(SKIPPED),\
	  echo "SKIP  $(b): $(call shown,$(firstword $(call missing,$(b)))) is not written yet.";) \
	$(foreach b,$(ENABLED),\
	  echo "TEST  $(b)"; \
	  $(call build_program,$(b)) \
	  if $(call compile,$(b)) $$program $(b)_test.cpp $(SUPPORT_DIR)/source/testsuite.cpp \
	       $(call sources,$(b)) -o build/$(b) $(LD_FLAGS); then \
	    timeout $(TEST_TIMEOUT) ./build/$(b); status=$$?; \
	    if [ $$status -eq 124 ]; then \
	      echo "FAIL  $(b): still running after $(TEST_TIMEOUT); an endless loop, a thread never"; \
	      echo "      joined, or a wait that is never notified?"; \
	    elif [ $$status -eq 134 ]; then \
	      echo "FAIL  $(b): aborted (SIGABRT) before its tests finished. std::terminate: a"; \
	      echo "      std::thread destroyed without join(), or a throw from a noexcept function?"; \
	    elif [ $$status -gt 128 ]; then \
	      echo "FAIL  $(b): crashed (signal $$((status - 128)), $$(kill -l $$((status - 128))))"; \
	      echo "      before its tests finished; a double delete or a dangling pointer?"; \
	    fi; \
	    [ $$status -eq 0 ] || failed=1; \
	  else \
	    $(if $($(b)_NEEDS),$(call not_compiling,$(b)),$(call suite_broken,$(b))) \
	    failed=1; \
	  fi; \
	  $(foreach r,$($(b)_REJECTS),\
	    if errors=$$($(call compile,$(b)) -fsyntax-only $(r) 2>&1); then \
	      echo "FAIL  $(b): $(r) compiled, but the exercise's static_assert should reject it."; \
	      failed=1; \
	    elif grep -q "static assertion failed" <<< "$$errors"; then \
	      echo "PASS  $(b): $(r) is rejected by a static_assert, as it should be."; \
	    else \
	      echo "FAIL  $(b): $(r) failed to compile, but not because of a static_assert:"; \
	      grep -m1 "error" <<< "$$errors" | sed 's/^/      /'; \
	      failed=1; \
	    fi;)) \
	exit $$failed

# The two halves, for anyone who wants them separately.
build: test
run: test

clean:
	@rm -rf build
