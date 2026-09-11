#!/usr/bin/env bash
#
# Build and run every lecture's test suite.
#
# Each suite tests the code in its lecture's workspace, lectures/LNN/exercises. A test binary for
# an exercise set is built only once the files that set asks for exist, and reported as SKIP until
# then, so a fresh clone runs each lecture's always-on tests and nothing else, and passes. Write a
# set's files and its tests switch themselves on, with no list to edit anywhere.
#
# With SOLUTIONS=1, every suite tests its lecture's reference solutions instead. That is how CI
# proves that every test in the course passes against a correct answer, and how you would check a
# test you have just written.
#
# Usage:
#   ci/test.sh                 test your workspace
#   SOLUTIONS=1 ci/test.sh     test the reference solutions
set -euo pipefail
shopt -s nullglob

# Absolute path to the repository root, resolved before the cd below.
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

FRAMEWORK_DIR="$ROOT/libs/test"

# Nothing here can run without the test framework, and the failure it would otherwise produce is a
# missing header nobody can act on.
if [ ! -f "$FRAMEWORK_DIR/Makefile" ]; then
    echo "error: test framework not found in libs/test. Run 'git submodule update --init'." >&2
    exit 1
fi

suites=0
failed=()

for makefile in lectures/*/exercises/test/Makefile; do
    test_dir="$(dirname "$makefile")"
    echo "==== ${test_dir}"
    # Run every suite even if an earlier one failed, so one report shows everything, and clean up
    # the binaries either way.
    if ! make -s -C "$test_dir" clean test ROOT_DIR="$ROOT" QACADEMY_TEST_DIR="$FRAMEWORK_DIR" \
        SOLUTIONS="${SOLUTIONS:-0}"; then
        failed+=("$test_dir")
    fi
    make -s -C "$test_dir" clean
    suites=$((suites + 1))
    echo
done

if [ ${#failed[@]} -ne 0 ]; then
    echo "Test: ${#failed[@]} of ${suites} suite(s) failed: ${failed[*]}"
    exit 1
fi
echo "Test: ${suites} suite(s) passed."
