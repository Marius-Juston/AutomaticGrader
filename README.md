# Automatic Grader — SE 423 (UIUC) Homework & Lab

Automated checker for the SE 423 course at UIUC. Compiles student firmware written for the TI C2000 (TMS320F28379D
LaunchPad) against Linux stubs, runs the student's `main()` on a background thread, and grades the submission by
inspecting simulated peripheral register state.

Designed to be dropped into a course CI/CD pipeline so submissions can be graded before they reach Gradescope.

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
| `HW{N}_ROADMAP.md`, `Lab{N}_ROADMAP.md`         | Per-assignment implementation checklist + deep-validation matrix (mutation tests + cadence + format). |
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

| Assignment | Status                                                                                                                                  | Roadmap                                |
|------------|-----------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------|
| HW1        | Implemented (init + timer0/1/2 + saturate). Pending spec rework: Exercise 9 LED10/11 + button-press LED toggles, format/cadence checks. | [HW1_ROADMAP.md](HW1_ROADMAP.md)       |
| HW2        | Not started — needs EPWM/ADC capturing stubs first.                                                                                     | [HW2_ROADMAP.md](HW2_ROADMAP.md)       |
| HW3        | Not started — needs SPI stubs with TX/RX buffer capture.                                                                                | [HW3_ROADMAP.md](HW3_ROADMAP.md)       |
| HW4        | Not started — needs XINT + GPIO-qualification stubs.                                                                                    | [HW4_ROADMAP.md](HW4_ROADMAP.md)       |
| HW5        | Not started — needs DMA stubs and an `RFFT_f32` shim.                                                                                   | [HW5_ROADMAP.md](HW5_ROADMAP.md)       |
| HW6        | Out of scope for the firmware grader (A* + Kalman). Route to a separate harness.                                                        | [HW6_ROADMAP.md](HW6_ROADMAP.md)       |
| Lab1       | Roadmap only.                                                                                                                           | [Lab1_ROADMAP.md](Lab1_ROADMAP.md)     |
| Lab2       | Roadmap only.                                                                                                                           | [Lab2_ROADMAP.md](Lab2_ROADMAP.md)     |
| Lab3       | Roadmap only — needs EQEP stubs.                                                                                                        | [Lab3_ROADMAP.md](Lab3_ROADMAP.md)     |
| Lab4       | Roadmap only — needs ADCB/ADCC/ADCD + DAC stubs.                                                                                        | [Lab4_ROADMAP.md](Lab4_ROADMAP.md)     |
| Lab5       | Roadmap only.                                                                                                                           | [Lab5_ROADMAP.md](Lab5_ROADMAP.md)     |
| Lab6       | Roadmap only — needs LADAR + SCID + SWI stubs.                                                                                          | [Lab6_ROADMAP.md](Lab6_ROADMAP.md)     |
| Lab7-1     | Roadmap only.                                                                                                                           | [Lab7-1_ROADMAP.md](Lab7-1_ROADMAP.md) |
| Lab7-2     | Roadmap only — needs OpenMV blob + OptiTrack injection stubs.                                                                           | [Lab7-2_ROADMAP.md](Lab7-2_ROADMAP.md) |

## How a check is structured

Each `hw{N}.cpp` / `lab{N}.cpp` defines `checker()` returning a list of `CheckFunction`s. The pattern is four phases (
extending the original three with a print-cadence/format phase):

1. **Phase 1 — pre-main zero state.** Confirm peripheral registers start clean.
2. **Phase 2 — post-init.** Call `validator.start_main_thread()`, sleep 10 ms, build `expected` structs, register
   comparisons against `gpiosSetup[]`, timers, IER, PieCtrlRegs, PieVectTable, GpioDataRegs, peripheral-specific
   configs.
3. **Phase 3 — ISR-driven dynamics + stimuli.** Snapshot volatile globals, apply stimuli (`press_button`,
   `inject_adc_result`, `inject_spi_rx`, etc.), drive ISRs synthetically (synthetic clock advances `synthetic_clock_us`
   by the configured period each call), assert state changes at predicted tick boundaries, restore snapshots before
   returning.
4. **Phase 4 — print cadence and format.** `resetPrintfCapture()` before Phase 3; afterwards call
   `expect_print_cadence(SCIA, expected_count, ±10%)` and
   `expect_format(latest, "spec'd format string with correct %ld/%.2f/%.3f specifiers")`. Catches `%d`-for-`int32_t` (
   the most common student bug — invisible in rendered output on 32-bit-int Linux but breaks on the C2000 target).

See `CLAUDE.md` for the full contributor guide and the per-assignment `*_ROADMAP.md` files for what each checker must
cover.

## Authoring tip — convert TI calls into expected[] entries

`tools/` contains a regex you can run in your editor to lift `GPIO_SetupPinMux`/`GPIO_SetupPinOptions` pairs out of a
reference solution and turn them into `expected[]` initializers:

Find:

```regex
GPIO_SetupPinMux\((\d+),(.+)\);\n+\s+GPIO_SetupPinOptions\(\d+,(.+)\);
```

Replace:

```
expectedGpioSetup[$1] = {$2, $3};
```

## Codegen

When a TI header changes (or a new peripheral struct is added), regenerate the supporting files:

```bash
python -c "from tools.validation         import generate_zero_checks;        print(generate_zero_checks(open('lib/.../f2837xd_gpio.h').read()))"
python -c "from tools.compare_validation import generate_zero_checks;        print(generate_zero_checks(open('lib/.../f2837xd_gpio.h').read()))"
python -c "from tools.hash_map_setter    import generate_hashmap_initializer; print(generate_hashmap_initializer(open('lib/.../f2837xd_gpio.h').read()))"
```

Replace the corresponding `generated.cpp`, `compare_generated.cpp`, or the body of `populate_all_zero()` in
`stat_checker.cpp`.

## Validating a new checker

For each new assignment, walk the **Validation matrix** in `HW{N}_ROADMAP.md` / `Lab{N}_ROADMAP.md`:

1. Reference solution from `context/code_solutions/.../{HW,Lab}{N}/` → all checks pass, zero `spdlog::warn` lines, exit 0. (For HW1 specifically, the reference deviates from spec in Exercise 9 and is expected to fail those checks —
that's documented in the roadmap.)
2. At least 5 hand-authored mutations of the reference, covering: numeric constants (period, TBPRD, threshold),
   structural (drop a setup line), format-string (`%ld` → `%d`), cadence (modulus change), stimulus (drop a button
   branch). Each must fail on the *exact* field name that diverged.
3. Run twice without rebuilding → identical pass result (verifies snapshot/restore in Phase 3).
4. Re-run prior assignments after adding new stubs → no regressions.

## License & attribution

Internal grading tool for SE 423 at UIUC. TI C2000Ware headers under `lib/C2000Ware_4_01_00_00/` are vendored under
their original Texas Instruments license.
