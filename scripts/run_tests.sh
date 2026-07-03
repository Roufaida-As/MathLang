#!/usr/bin/env bash
set -euo pipefail

if [ ! -x ./parser ]; then
  echo "parser not found or not executable. Run 'make' first." >&2
  exit 2
fi

echo "Running tests in tests/"
fail=0
for f in tests/*.ml; do
  echo "=== $f ==="
  if ! ./parser "$f"; then
    echo "Test failed: $f" >&2
    fail=1
  fi
done

if [ $fail -ne 0 ]; then
  echo "Some tests failed." >&2
  exit 1
fi

echo "All tests passed."
