#!/usr/bin/env bash
set -euo pipefail

if [ ! -x ./parser ]; then
  echo "parser not found or not executable. Run 'make' first." >&2
  exit 2
fi

RUN_TIMEOUT=5
fail=0

echo "Running tests in tests/"
for f in tests/*.ml; do
  echo "=== $f ==="
  rm -f output.c output

  if ! ./parser "$f"; then
    echo "Test failed (parser crashed): $f" >&2
    fail=1
    continue
  fi

  # Pas d'output.c genere : normal si le fichier contient des erreurs
  # semantiques volontaires (ex: tests/test_semantic_errors.ml).
  if [ ! -f output.c ]; then
    echo "-- no output.c generated (ok if this test expects semantic errors)"
    continue
  fi

  if ! gcc -Wall output.c -o output -lm 2>&1; then
    echo "Test failed (generated C does not compile): $f" >&2
    fail=1
    continue
  fi

  if timeout "$RUN_TIMEOUT" ./output < /dev/null > /dev/null; then
    status=0
  else
    status=$?
  fi

  if [ "$status" -ne 0 ]; then
    if [ "$status" -eq 124 ]; then
      echo "Test failed (timeout / infinite loop): $f" >&2
    else
      echo "Test failed (runtime error, exit $status): $f" >&2
    fi
    fail=1
    continue
  fi

  echo "-- ok (compiled and ran)"
done

if [ $fail -ne 0 ]; then
  echo "Some tests failed." >&2
  exit 1
fi

echo "All tests passed."