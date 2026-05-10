# Build organisation

The CMake build is split into five object libraries that compile in
parallel, so a per-assignment checker tweak doesn't recompile the
heavy auto-generated state tracker.

```mermaid
flowchart LR
    subgraph Parallel[" "]
        direction TB
        Infra[grader_infra<br/>format_parser, synthetic_clock,<br/>printf_capture, stimulus,<br/>expectations, selftest]
        State[grader_state_check<br/>generated.cpp, compare_generated.cpp,<br/>stat_checker.cpp, validator.cpp]
        Stubs[grader_stubs<br/>ti_stubs.cpp, lidar.cpp,<br/>OptiTrack.c, LEDPatterns.c,<br/>main_loop_driver.cpp]
        Assign[grader_assignment<br/>src/checks/hw{N}.cpp<br/>OR lab{N}.cpp<br/>(selected by ASSIGNMENT=)]
        Student[grader_student<br/>patched temp_main]
    end
    Patcher[patch_student_source.py] -.generates.-> Student
    Main[src/main.cpp] --> Bin[AutomaticGrader executable]
    Infra --> Bin
    State --> Bin
    Stubs --> Bin
    Assign --> Bin
    Student --> Bin
    Ccache[ccache] -.launcher.-> Parallel

    classDef obj fill:#fef3c7,stroke:#d97706
    classDef bin fill:#dcfce7,stroke:#16a34a
    classDef tool fill:#dbeafe,stroke:#2563eb
    class Infra,State,Stubs,Assign,Student obj
    class Bin bin
    class Patcher,Ccache,Main tool
```

| Library | What it contains | Why it's split |
|---|---|---|
| `grader_infra` | Shared APIs (format parser, capture, clock, stimulus, expectations, selftest). | Stable; every checker depends on it; recompiled only when slice-1 changes. |
| `grader_state_check` | Auto-generated zero-check + compare overloads, `HardwareStateValidator`, `Validator`. | Heavy translation units; would dominate every incremental rebuild if not isolated. |
| `grader_stubs` | TI driverlib stand-ins, lidar/OptiTrack/LED helper sources, cooperative driver. | Recompiled only on stub changes. |
| `grader_assignment` | The single `src/checks/hw{N}.cpp` or `lab{N}.cpp` selected by `ASSIGNMENT=`. | Recompiled on every checker tweak; cheap because everything else is cached. |
| `grader_student` | The student firmware, *patched* — see [Cooperative driver](cooperative-driver.md). | Recompiled when `STUDENT_SRC=` changes or the source under it changes. |

## ccache

`CMakeLists.txt` auto-detects ccache via `find_program(CCACHE_PROGRAM ccache)`
and wires it as `CMAKE_C_COMPILER_LAUNCHER` / `CMAKE_CXX_COMPILER_LAUNCHER`.
On a 4-core machine, a clean rebuild is ~24 s; single-file incrementals are
~2 s.

The CI workflow extends this with `actions/cache@v5` keyed on
`hashFiles(workspace/<folder>/**/*.c)`, so PRs that don't touch student
source enjoy a warm cache.

## Why the patched student source

The original student `while(1)` body is rewritten at build time so the
harness can step it one iteration at a time. See
[Cooperative driver](cooperative-driver.md) for the contract; the
mechanism is just a Python regex over the C source:

- `tools/patch_student_source.py` runs as a CMake custom command, in:
  `${BUILD}/student_patched/student.c`. Inspect it when debugging a
  checker mismatch.
- The patched file gets `COMPILE_DEFINITIONS "main=temp_main"` so it
  coexists with `src/main.cpp`.

## Tests target

C++ unit tests live under `tests/` and are off by default. Build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DGRADER_BUILD_TESTS=ON
cmake --build build --target grader_tests -j
ctest --test-dir build --output-on-failure
```

Python tests are under `tests/python/` and run with `unittest`:

```bash
python3 -m unittest discover -s tests/python -v
```

Both are invoked by `.github/workflows/tests.yml` (see
[CI/CD workflow](../workflow/ci-cd.md) for the production pipeline).

## Macros wired by the assignment selector

The selector also defines compile-time macros so `validator.cpp` knows
which `hw{N}.h` / `lab{N}.h` to include and so student code can
`#ifdef AUTO_GRADER` for grader-specific behaviour:

| Macro | When defined |
|---|---|
| `AUTO_GRADER` | Always when built by this repo. |
| `HW=N` | When `ASSIGNMENT=HW<N>` (also set to 1 for Lab builds as a legacy fallback). |
| `LAB=N` | When `ASSIGNMENT=Lab<N>`. |
| `ASSIGNMENT_HW` | When `ASSIGNMENT=HW…`. |
| `ASSIGNMENT_LAB` | When `ASSIGNMENT=Lab…`. |
| `GRADER_MAIN_INIT_GUARD` / `GRADER_MAIN_LOOP` | Defined in `include/ti_stubs.h` under `AUTO_GRADER`; consumed by the patched student source. |
