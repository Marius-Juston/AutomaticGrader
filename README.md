# Automatic Grader — SE 423 (UIUC) Homework & Lab

[![Tests](https://github.com/Marius-Juston/AutomaticGrader/actions/workflows/tests.yml/badge.svg)](https://github.com/Marius-Juston/AutomaticGrader/actions/workflows/tests.yml) [![Docs](https://github.com/Marius-Juston/AutomaticGrader/actions/workflows/docs.yml/badge.svg)](https://github.com/Marius-Juston/AutomaticGrader/actions/workflows/docs.yml)

Automated checker for the SE 423 course at UIUC. Compiles student firmware written for the TI C2000 (TMS320F28379D
LaunchPad) against Linux stubs, drives the student's `main()` cooperatively from a single thread, and grades the
submission by inspecting simulated peripheral register state plus captured `serial_printf` output.

Designed to be dropped into a course CI/CD pipeline so submissions can be graded before they reach Gradescope.

## 📚 Documentation

Full documentation — architecture, contributor tutorial, CI/CD walkthrough, C++ + Python API reference — is published as
a GitHub Pages site:

**<https://marius-juston.github.io/AutomaticGrader/>**

Quick links:

- **[Getting started](https://marius-juston.github.io/AutomaticGrader/getting-started/)** — build and run locally in under five minutes.
- **[Add a new checker](https://marius-juston.github.io/AutomaticGrader/contributing/add-a-checker/)** — the contributor tutorial.
- **[Architecture overview](https://marius-juston.github.io/AutomaticGrader/architecture/overview/)** — three-layer model + cooperative driver + capture pipeline.
- **[CI/CD workflow](https://marius-juston.github.io/AutomaticGrader/workflow/ci-cd/)** — reusable workflow, manifest, slot keying, report traceability.
- **[Course staff guide](https://marius-juston.github.io/AutomaticGrader/course-staff/instructor-guide/)** — how to wire it into a course repo.

The detailed reference content that used to live in this README has moved into the docs site. The sections below are a
condensed quick-start.

## Repository role in the larger course pipeline

This repo is a *subcomponent*. The outer course repo provides student work in a `workspace/` folder with a manifest
mapping assignment IDs to relative paths:

```
HW1  students/alice/hw1
HW2  students/alice/hw2
HW3  students/bob/hw3
...
```

CI iterates the manifest, drops each entry into `student_work/LABstarter_main.c`, builds with `ASSIGNMENT=<id>`, runs
the grader, and aggregates pass/fail per assignment into a markdown report.

## Authoritative sources

The LaTeX HW specs and the Lab `.txt` specs are **absolute** — every checker assertion must trace to them:

- `context/homeworks/HW1.tex`..`HW6.tex` — homework specs.
- `context/lab/Lab1.txt`..`Lab7-2.txt` — lab specs.

Reference student solutions in `context/code_solutions/myHW_Yingyord_First/HW{N}/` and
`context/code_solutions/SE423_Lab_phuriy2_lourdes3_emilyah3/Lab*/` are *non-authoritative*. They serve as one
positive-test fixture and as a structural guide for stub design. Where they deviate from the spec, the spec wins and the
reference is allowed to fail checks (with the failure documented in the corresponding roadmap).

## Repository layout

| Path                                            | Purpose                                                                                               |
|-------------------------------------------------|-------------------------------------------------------------------------------------------------------|
| `src/main.cpp`                                  | Entry point — instantiates `Validator`, runs the assignment's check list.                             |
| `src/checks/hw{N}.cpp`, `src/checks/lab{N}.cpp` | Per-assignment checker (selected by `ASSIGNMENT=` cache var).                                         |
| `src/checks/validator.cpp`                      | `Validator` — owns the `std::jthread` running the student's `temp_main`.                              |
| `src/checks/stat_checker.cpp`                   | `HardwareStateValidator` — tracks register state and runs comparisons.                                |
| `src/checks/generated.cpp`                      | Generated zero-check overloads for every TI register struct (don't hand-edit).                        |
| `src/checks/compare_generated.cpp`              | Generated pairwise-compare overloads (don't hand-edit).                                               |
| `include/checks/stimulus.hpp`                   | Stimulus helpers: `press_button`, `inject_adc_result`, `inject_spi_rx`, `inject_lidar_frame`, etc.    |
| `include/checks/format_check.hpp`               | Format-string verification: `expect_format`, `expect_arg_types`, `expect_print_cadence`.              |
| `src/ti_stubs.cpp`, `include/ti_stubs.h`        | TI driverlib stand-ins + macro neutralizers + `serial_printf` capture (`g_printfCalls`).              |
| `lib/C2000Ware_4_01_00_00/`                     | Vendored TI headers (peripheral register structs, base addresses).                                    |
| `student_work/LABstarter_main.c`                | Student's `main()` — compiled with `-Dmain=temp_main` so it coexists with `src/main.cpp`.             |
| `tools/*.py`                                    | Regex-based codegen for the `generated.*` and `populate_all_zero()` files.                            |
| `context/homeworks/HW{N}.tex`                   | **Authoritative** homework specs.                                                                     |
| `context/lab/Lab{N}.txt`                        | **Authoritative** lab specs.                                                                          |
| `context/code_solutions/`                       | Non-authoritative reference solutions (one fixture per assignment).                                   |
| `progress/HW{N}_ROADMAP.md`, `progress/Lab{N}_ROADMAP.md`         | Per-assignment implementation checklist + deep-validation matrix (mutation tests + cadence + format). |
| `CLAUDE.md`                                     | Architecture and contributor guide.                                                                   |

## Requirements

- Linux (WSL is fine). MinGW/Windows is unsupported because the harness relies on `__attribute__((weak))`.
- CMake ≥ 4.1, a C++20 compiler.
- `libspdlog-dev`.

```bash
sudo apt install libspdlog-dev
```

## Build & run

```bash
ASSIGNMENT=HW1 cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
./cmake-build-debug/AutomaticGrader
```

Valid `ASSIGNMENT` values: `HW1`..`HW6`, `Lab1`..`Lab6`, `Lab7-1`, `Lab7-2`. Legacy `HW=N` env var still works (mapped
to `ASSIGNMENT=HW${N}`). Switch by re-running cmake (`cmake -DASSIGNMENT=Lab3 cmake-build-debug`); the value is `FORCE`d
into the cache, so changing only the env var without rerunning cmake won't take effect.

Successful run:

```
[info] Homework: 1
[info] Succeeded all tests!
```

A failed check logs each diverging register field via `spdlog::warn` and the binary exits non-zero.

## Per-assignment status

The implementation status of each assignment lives in two places:

- A condensed matrix in [CLAUDE.md](CLAUDE.md) (single source of truth for what's wired up).
- Per-assignment roadmaps under [`progress/`](progress/) — each with the spec inventory, stub checklist, and the
  validation matrix that the checker must satisfy.

## License & attribution

Internal grading tool for SE 423 at UIUC. TI C2000Ware headers under `lib/C2000Ware_4_01_00_00/` are vendored under
their original Texas Instruments license.
