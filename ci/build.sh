#!/usr/bin/env bash
#
# Build the lecture demo code.
#
# Usage:
#   ci/build.sh
set -euo pipefail

# Root directory.
ROOT_DIR="$(dirname "${BASH_SOURCE[0]}")/.."

# Build every lecture demo that has a Makefile. The test suites are left to ci/test.sh: they
# build against your own code in lectures/LNN/exercises, which `make build` has no business
# requiring.
while IFS= read -r -d '' makefile; do
    demo_dir="$(dirname "$makefile")"
    echo "Building $demo_dir"
    make -C "$demo_dir" build
done < <(find "$ROOT_DIR/lectures" -path '*/exercises/*' -prune -o -iname Makefile -print0 | sort -z)
