# Getting started

This page gets you from a clean clone to a green grader run on a
reference fixture in under five minutes.

## Prerequisites

- **Linux** (WSL is fine). MinGW / native Windows is unsupported because
  the harness relies on `__attribute__((weak))`.
- **CMake ≥ 3.21** and a **C++20** compiler (GCC ≥ 12 or Clang ≥ 15).
- **libspdlog-dev** for structured logging.
- **Python ≥ 3.12** (used by the build to patch student source).
- **ccache** (optional but recommended — wired automatically when found).

```bash
sudo apt update
sudo apt install -y build-essential cmake ccache libspdlog-dev python3
```

## Clone

```bash
git clone https://github.com/Marius-Juston/AutomaticGrader.git
cd AutomaticGrader
```

## Build for an assignment

The build is parameterised by the `ASSIGNMENT` cache variable (or env
var). Valid values: `HW1`..`HW6`, `Lab1`..`Lab6`, `Lab7-1`, `Lab7-2`.

=== "Configure once, build many"

    ```bash
    cmake -S . -B cmake-build-debug -DASSIGNMENT=HW1
    cmake --build cmake-build-debug -j
    ./cmake-build-debug/AutomaticGrader
    ```

=== "Switch assignments"

    Switching assignments requires a fresh `cmake` invocation (the value
    is `FORCE`d into the cache):

    ```bash
    cmake -S . -B cmake-build-debug -DASSIGNMENT=HW3
    cmake --build cmake-build-debug -j
    ```

=== "Env var (legacy)"

    The legacy `HW=N` env var still maps to `ASSIGNMENT=HW${N}`:

    ```bash
    HW=1 cmake -S . -B cmake-build-debug
    cmake --build cmake-build-debug -j
    ```

## Run against a reference fixture

The repo ships reference student solutions under `context/code_solutions/`.
Point the build at one via `STUDENT_SRC`:

```bash
cmake -S . -B cmake-build-debug \
    -DASSIGNMENT=HW2 \
    -DSTUDENT_SRC=context/code_solutions/myHW_Yingyord_First/HW2/HW2fy_main.c
cmake --build cmake-build-debug -j
./cmake-build-debug/AutomaticGrader
```

A successful run looks like:

```
[info] Homework: 2
[info] Succeeded all tests!
```

A failed check logs the diverging register field via `spdlog::warn`/
`spdlog::error` and exits non-zero.

## Emit a JSON report

CI consumes the same binary; pass `--report-json` to get a machine-readable
result:

```bash
./cmake-build-debug/AutomaticGrader --report-json /tmp/HW2.json
```

## Run the self-test

The shared infrastructure (format parser, synthetic clock, printf
capture, stimulus, expectations) has a built-in self-test that runs in
a few hundred milliseconds:

```bash
./cmake-build-debug/AutomaticGrader --selftest
# or
GRADER_SELFTEST=1 ./cmake-build-debug/AutomaticGrader
```

Run this before any non-trivial change to `grader_infra` — a green
self-test is the contract that lets per-assignment checkers rely on the
shared APIs.

## Unit tests

C++ unit tests are off by default to keep CI subcomponent times short.
Enable with `-DGRADER_BUILD_TESTS=ON`:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DGRADER_BUILD_TESTS=ON
cmake --build build --target grader_tests -j
ctest --test-dir build --output-on-failure
```

Python-side tests (for `select_assignments.py` and `render_report.py`):

```bash
python3 -m unittest discover -s tests/python -v
```

## Next steps

- New to the codebase? Read [Architecture overview](architecture/overview.md).
- Adding a new assignment? Start with
  [Add a checker](contributing/add-a-checker.md).
- Wiring into a course repo? Jump to
  [CI/CD workflow](workflow/ci-cd.md).
