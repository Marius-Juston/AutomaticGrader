# CMake options

`CMakeLists.txt` exposes a few cache variables and options that gate
the build. All are also documented inline in the CMake file itself.

## Cache variables

### `ASSIGNMENT`

Selects which per-assignment checker to compile.

| Value | Source file |
|---|---|
| `HW1`..`HW6` | `src/checks/hw{N}.cpp` |
| `Lab1`..`Lab6` | `src/checks/lab{N}.cpp` |
| `Lab7-1` | `src/checks/lab7_1.cpp` |
| `Lab7-2` | `src/checks/lab7_2.cpp` |

Sources:

- Default: `HW1`.
- Environment variable: `ASSIGNMENT=...` overrides the default.
- Legacy: `HW=N` env var maps to `ASSIGNMENT=HW${N}`.
- CMake flag: `-DASSIGNMENT=...`.

The value is `FORCE`d into the cache, so switching assignments
requires re-running `cmake` — a plain `cmake --build` won't pick up
a changed env var.

If the requested checker source doesn't exist yet (e.g. `Lab3` while
the roadmap is still being authored), CMake emits a WARNING and falls
back to `src/checks/hw1.cpp`. Build and link still succeed but the
grader validates HW1 instead. Handy for testing the harness while a
new checker is being written.

### `STUDENT_SRC`

Path to the student firmware source. Default:
`student_work/LABstarter_main.c`. Common reference fixtures live
under `context/code_solutions/`.

Both relative and absolute paths are accepted. The path is validated
at configure time; a missing file fails fast with a clear error:

```
STUDENT_SRC='foo/bar.c' does not exist (resolved: /abs/path/foo/bar.c).
```

```bash
cmake -S . -B build -DASSIGNMENT=HW2 \
    -DSTUDENT_SRC=context/code_solutions/myHW_Yingyord_First/HW2/HW2fy_main.c
```

The file at `STUDENT_SRC` is run through `tools/patch_student_source.py`
at build time to convert its `while(1)` into the cooperative-driver
loop ([details](../architecture/cooperative-driver.md)). The patched
copy lands at `${BUILD}/student_patched/student.c` — inspect it when
debugging.

### `CMAKE_BUILD_TYPE`

Defaults to `RelWithDebInfo`. Common alternatives: `Debug`, `Release`.

| Config | Compile flags appended |
|---|---|
| `Debug` | `-O0 -g` |
| `RelWithDebInfo` | `-O2 -g -DNDEBUG` |
| `Release` | `-O2 -DNDEBUG` |

## Options

### `GRADER_BUILD_TESTS`

Default `OFF`. When `ON`, adds the `tests/` subdirectory which builds
the GTest/GMock-based unit tests:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DGRADER_BUILD_TESTS=ON
cmake --build build --target grader_tests -j
ctest --test-dir build --output-on-failure
```

## Automatic detections

- **ccache** — `find_program(CCACHE_PROGRAM ccache)`. When found,
  wired as `CMAKE_C_COMPILER_LAUNCHER` /
  `CMAKE_CXX_COMPILER_LAUNCHER`. No flag needed.
- **Python 3** — `find_package(Python3 REQUIRED COMPONENTS Interpreter)`
  for the student-source patcher. CMake fails fast if missing.
- **spdlog** — `find_package(spdlog REQUIRED)`. Install
  `libspdlog-dev` on Debian/Ubuntu.

## Compile-time macros

Defined globally and consumed by `validator.cpp`, `ti_stubs.h`, and
optionally by student code (`#ifdef AUTO_GRADER`):

| Macro | When defined |
|---|---|
| `AUTO_GRADER` | Always (any time the source is built by this repo). |
| `HW=N` | `ASSIGNMENT=HW<N>`; also `HW=1` for Lab builds as a legacy fallback. |
| `LAB=N` | `ASSIGNMENT=Lab<N>`. |
| `ASSIGNMENT_HW` | `ASSIGNMENT=HW…`. |
| `ASSIGNMENT_LAB` | `ASSIGNMENT=Lab…`. |

`include/ti_stubs.h` consumes these to wire the patched
`while(1)` → `GRADER_MAIN_LOOP` macros under `AUTO_GRADER`.
