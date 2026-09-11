# Build every lecture demo that has a Makefile.
build:
	@bash ci/build.sh

# Build and run every lecture's test suite against your code in lectures/LNN/exercises.
# SOLUTIONS=1 runs the same tests against the reference solutions instead.
test:
	@SOLUTIONS=$(SOLUTIONS) bash ci/test.sh

# Format all C/C++ files (clang-format) in place.
format:
	@bash ci/format.sh

# Check formatting without modifying any files; fails if something isn't formatted.
format-check:
	@bash ci/format.sh --check

.PHONY: build test format format-check
