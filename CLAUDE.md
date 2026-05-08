# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this project is

Auto-grader for SE 423 (UIUC) homework and lab code targeting the TI C2000 TMS320F28379D LaunchPad. It compiles the student's firmware and runs it on a Linux host: `main()` is renamed to `temp_main`, launched on a background thread, and checker functions inspect simulated peripheral/register state to grade the submission.

Designed to be consumed as a CI/CD subcomponent of a larger course repo. The outer repo provides:
- `workspace/` — student source trees, with a manifest mapping `<assignment ID> <relative path>` per line so the grader knows which folder is HW1, Lab3, etc. (Not present in this repo on its own; created by the parent repo / CI.)
- `context/homeworks/HW{N}.tex` (HW1..HW6) — **authoritative** spec for each homework.
- `context/lab/Lab{N}.txt` (Lab1, Lab2, Lab3, Lab4, Lab5, Lab6, Lab7-1, Lab7-2) — **authoritative** spec for each lab.
- `context/code_solutions/` — *non-authoritative* example student solutions, retained as positive-test fixtures and as a structural reference when designing capturing stubs. The Garcia PDF reference was removed; only `myHW_Yingyord_First/HW{N}/` and `SE423_Lab_phuriy2_lourdes3_emilyah3/Lab*/` remain. **Where a reference deviates from the spec, the spec wins** — the reference is allowed to fail checks (and that failure should be documented in the corresponding roadmap, not silenced by loosening the check).

The grader emits PASS/FAIL feedback to the GitHub Action log and to a markdown report.

## Authority hierarchy

1. The LaTeX HW spec or `.txt` Lab spec is absolute. Every checker assertion must trace to a quoted spec sentence.
2. Where the spec is genuinely ambiguous (e.g. "print every 100 ms" without a precision specifier), tag the assertion **[ambiguous]** in the roadmap and pick the most common interpretation. Course staff resolve later.
3. Reference solutions inform stub design (which TI driverlib functions to capture, which globals must be `extern`-able) and serve as one positive-test fixture. They do not dictate behavior.

## Three new test categories (apply uniformly across HW + Lab)

1. **Stimulus simulation.** Drive the stimuli the student is supposed to react to:
   - `press_button(gpio_num)` / `release_button(gpio_num)` for PB1..PB4 / joystick.
   - `inject_adc_result(module, soc, value)` for photodiodes / joystick / mic / gyro voltages.
   - `inject_spi_rx(module, word)` for MPU-9250 / DAN28027 reads.
   - `inject_encoder_count(module, count)` for EQEP-driven velocity tests.
   - `inject_lidar_frame(distances[228])` for Lab6.
   These helpers live in `include/checks/stimulus.hpp` and operate by writing to the stub-side `*Regs` and FIFO shadows. Always pair a stimulus with the spec'd response assertion (LED toggle, voltage scaling, state transition).

2. **Printf format-string verification.** TI C2000 has 16-bit `int`; the host Linux build has 32-bit `int`. A student writing `%d` for an `int32_t` works on the grader's host but fails on the target — this is the *single most common* bug in SE 423 submissions and rendered-output inspection cannot catch it. Solution: intercept `serial_printf` at the call site in `src/ti_stubs.cpp` and record `{port, fmt_string, parsed_specifier_sequence, rendered_output, synthetic_timestamp_us}` into `g_printfCalls`. Checkers then call `expect_format(call, "Timeint = %ld, Time = %.2f sec, …")` which parses both expected and actual format strings into a sequence of `(specifier, length_modifier, precision)` tuples and compares — tolerating whitespace / static-text differences but flagging any specifier-vs-required-type mismatch (`%d`-for-`int32_t`, `%f`-for-`%.2f`, missing `\r\n`, etc.).

3. **Print cadence.** Each checker call advances a synthetic-clock counter by `CpuTimerN.PeriodInUSec` per ISR invocation. After driving the ISR for a known synthetic duration, assert `g_printfCalls.size()` matches the spec's required rate within ±10%. Decoupled from wall-clock racing.

## Per-assignment status and roadmaps

Each assignment has a checklist at the repo root tracking required stubs, check functions, and the deep-validation matrix (mutation tests, snapshot hygiene, cross-assignment regression). When implementing or modifying a checker, update the matching roadmap as boxes are checked.

| Assignment | Checker file                  | Status                                                                                                |
| ---------- | ----------------------------- | ----------------------------------------------------------------------------------------------------- |
| HW1        | `src/checks/hw1.cpp`          | Init + timer0/1/2 + saturate done. **Pending spec rework**: re-instate Exercise 9 LED10/11 + button-press LED12/13/14/15 toggles; add `check_print_cadence` and `check_print_format`. |
| HW2        | not yet created               | Needs EPWM + ADC SOC capturing stubs.                                                                 |
| HW3        | not yet created               | Needs SPI stubs incl. `spibTxLog[]`/`spibRxLog[]`.                                                    |
| HW4        | not yet created               | Needs XINT + GPIO-qualification stubs.                                                                |
| HW5        | not yet created               | Needs DMA stubs + `RFFT_f32` shim.                                                                    |
| HW6        | out-of-scope (firmware)       | A* + Kalman — route to a separate harness in the parent CI repo.                                      |
| Lab1..Lab7-2 | not yet created             | Roadmaps land first; CMake `ASSIGNMENT=` selector supports them. `.cpp` checkers deferred.            |

The plan that drives this is at `~/.claude/plans/given-the-homework-solutions-delightful-anchor.md`.

## Build selector: `ASSIGNMENT`

`CMakeLists.txt` exposes an `ASSIGNMENT` cache variable. Valid values: `HW1`..`HW6`, `Lab1`..`Lab6`, `Lab7-1`, `Lab7-2`. Legacy `HW=N` env var is mapped to `ASSIGNMENT=HW${N}` for back-compat. Switching assignments requires re-running cmake (the value is `FORCE`d into the cache).

```bash
ASSIGNMENT=HW1   cmake -S . -B cmake-build-debug && cmake --build cmake-build-debug
ASSIGNMENT=Lab3  cmake -S . -B cmake-build-debug && cmake --build cmake-build-debug
HW=1             cmake -S . -B cmake-build-debug && cmake --build cmake-build-debug   # back-compat
```

The selector defines either `ASSIGNMENT_HW` + `HW=N` or `ASSIGNMENT_LAB` + `LAB=N` macros. `src/checks/validator.cpp` uses these to include the right `hw{N}.h` / `lab{N}.h`.

## Adding a new assignment checker — required workflow

1. **Read the spec first** — `context/homeworks/HW{N}.tex` or `context/lab/Lab{N}.txt`. Quote spec sentences in the roadmap as the source-of-truth for each assertion.
2. Read `HW{N}_ROADMAP.md` / `Lab{N}_ROADMAP.md`; it lists the exact stubs and checks derived from step 1.
3. Add capturing stubs to `src/ti_stubs.cpp` (mirror the `GPIO_SetupPinMux` pattern at `src/ti_stubs.cpp:155` — read the register pointer, write the bitfield, return). Prefer extending the existing `*Regs` global over inventing a new shadow array, so existing `check_compare` overloads pick up the state for free. For new test categories: extend `g_printfCalls` capture (already global, all assignments share it) and add stimulus helpers to `include/checks/stimulus.hpp`.
4. Add `src/checks/{hw,lab}{N}.cpp` and `include/checks/{hw,lab}{N}.h` mirroring `hw1.cpp`/`hw1.h`. Define `checker()` returning the `CheckFunctions` list. Use the four-phase pattern below.
5. Walk the validation matrix at the bottom of the roadmap: build, run reference solution(s), run at least 3 mutated copies and confirm each fails on the *exact* register/field, run twice without rebuilding to confirm snapshot hygiene, and re-run all prior HWs to catch regressions.
6. Update the roadmap (check the boxes) as part of the same commit.

## Four-phase check pattern (from hw1.cpp + new test categories)

- **Phase 1 — pre-main zero state.** `HardwareStateValidator v; v.populate_all_zero(); v.validate();`. Done once in the first check; confirms registers start clean.
- **Phase 2 — post-init state.** `val->start_main_thread()` (sleeps 10 ms), then build `expected` structs and call `register_comparison*` / `register_custom*` for `gpiosSetup[]`, timers, IER, PieCtrlRegs, PieVectTable, GpioDataRegs initial set/clear bits. End with `v.validate()`.
- **Phase 3 — ISR-driven dynamics with stimuli.** Snapshot volatile globals (`const GPIO_DATA_REGS initial = GpioDataRegs; uint16_t saveU = UARTPrint;`). Apply stimuli (`press_button(4)`, `inject_adc_result(ADCA, 0, 4095)`, `inject_spi_rx(SPIB, 0x4000)`, `inject_lidar_frame(...)`, etc.) before each ISR loop. Drive `cpu_timerN_isr()` / `xintN_isr()` / `ADCA_isr()` / `DMA_isr()` for a known synthetic duration (each call advances `synthetic_clock_us` by the configured period). Register expectations at predicted tick boundaries (e.g. `expectedTimeStep = 250000.f / CpuTimer2.PeriodInUSec`), validate, **then restore snapshots before returning** so subsequent checks aren't polluted.
- **Phase 4 — print cadence + format.** Reset `g_printfCalls` before driving the ISR loop in Phase 3. After the loop, call `expect_print_cadence(SCIA, expected_count, ±10%)` and `expect_format(latest_call_for_port(SCIA), "fmt with correct %ld/%.2f/%.3f specifiers")`. Cadence uses synthetic timestamps (no wall-clock racing); format check parses both expected and actual format strings into specifier sequences.

`HardwareStateValidator` registration variants:
- `register_comparison(name, actual, expected)` — by reference; safe when `actual` won't change before `validate()`.
- `register_comparison_copy(name, actual, expected)` — captures by value; required for state mutated between registration and validation (e.g. `GpioDataRegs` between ISR ticks).
- `register_custom*` — supply your own predicate (HW1 uses this for `IER` bitmask and `UARTPrint` toggling).
- `mark_as_used(name)` — opt out of `populate_all_zero` for registers the student is expected to mutate.

`PrintfCall` capture API (in `include/ti_stubs.h`, populated by the override of `serial_printf` in `src/ti_stubs.cpp`):
- `g_printfCalls` — append-only `std::vector<PrintfCall>`. Each entry has `{port, fmt, parsed_specifiers, rendered, synthetic_timestamp_us}`.
- `resetPrintfCapture()` — clear the vector and reset `synthetic_clock_us`. Call at the top of every Phase-3 block.
- `expect_format(call, expected_fmt)` — assert specifier sequences match (length-modifier, precision, conversion). Tolerates whitespace and static-text differences.
- `expect_arg_types(call, {ARG_INT32, ARG_FLOAT, ...})` — assert the parsed type list matches what the spec demands.
- `expect_print_cadence(port, expected_count, tolerance_pct)` — assert exactly N calls within tolerance.

`stimulus.hpp` helpers (peripheral state injection):
- `press_button(int gpio)` / `release_button(int gpio)` — write `GPxDAT.bit.GPIO{N}`.
- `inject_adc_result(adc_module, soc_idx, uint16_t value)` — write `Adc{a..d}ResultRegs.ADCRESULT{N}`.
- `inject_spi_rx(spi_module, uint16_t word)` — push into the SPI RX FIFO shadow.
- `inject_encoder_count(eqep_module, int32_t count)` — write `EQep{1,2,3}Regs.QPOSCNT`.
- `inject_lidar_frame(uint16_t distances[228])` — fill the LADAR ping-pong buffer.
- `run_isr_for_us(IsrFn, period_us, total_us)` — calls `IsrFn()` `total_us / period_us` times, advancing the synthetic clock each call.

## Build & run

Linux only — relies on `__attribute__((weak))` (not available under MinGW). Requires `libspdlog-dev` and CMake ≥ 4.1, C++20.

```bash
sudo apt install libspdlog-dev
ASSIGNMENT=HW1 cmake -S . -B cmake-build-debug && cmake --build cmake-build-debug
./cmake-build-debug/AutomaticGrader
```

`cmake-build-debug/` is the existing CLion build dir and already has `compile_commands.json`; prefer reusing it over making a fresh `build/`.

Select an assignment via the `ASSIGNMENT` env var (or `-DASSIGNMENT=...`). Valid values: `HW1`..`HW6`, `Lab1`..`Lab6`, `Lab7-1`, `Lab7-2`. Legacy `HW=N` env var still works (mapped to `ASSIGNMENT=HW${N}`). To switch between builds, re-run cmake — `FORCE` in `CMakeLists.txt` overwrites the cache, but `make` alone won't notice the env var change. The `AUTO_GRADER` macro is also defined globally; student code / stubs can branch on it.

There are no automated tests, lint config, or formatter wired up in this repo. Each assignment's `checker()` *is* the test suite.

The student source under `student_work/LABstarter_main.c` is compiled with `-Dmain=temp_main` so it can coexist with `src/main.cpp`. `-include ti_stubs.h` is force-included on every TU to neutralize TI-specific keywords (`__interrupt`, `EALLOW`, `DATA_SECTION`, etc.).

## Architecture

Three layers:

1. **TI hardware shim** (`include/`, `src/ti_stubs.cpp`, `lib/C2000Ware_4_01_00_00/...`). Provides plain-memory definitions of every C2000 peripheral register struct (`GpioCtrlRegs`, `AdcaRegs`, `EPwm1Regs`, …) plus stubs for `GPIO_SetupPinMux`, `GPIO_SetupPinOptions`, `InitSysCtrl`, serial I/O, etc. The student code links against these instead of the real TI driverlib so it runs as a normal Linux process. `ti_stubs.h` is force-included to map TI-only attributes/types to no-ops or standard types (e.g. `Uint16 → uint16_t`, `__interrupt → ` empty).

2. **State tracking** (`src/checks/generated.cpp`, `src/checks/compare_generated.cpp`, `src/checks/stat_checker.cpp`). Generated C++ that knows how to recursively `check_zero(...)` and `check_compare(..., expected, ...)` every TI register struct. `HardwareStateValidator` (in `state_checker.h`) holds a `tracker_` map from register-name → lambda; `populate_all_zero()` registers a checker for each `extern struct` declared in the TI headers. Both `generated.cpp` and the populate map are *generated* by the Python tools — do not hand-edit them; regenerate instead (see below).

3. **Grading harness** (`src/main.cpp`, `src/checks/validator.cpp`, `src/checks/hw*.cpp`). `Validator` owns a list of `CheckFunction`s and a `std::jthread` running the student's `temp_main`. Each `hw${N}.cpp` defines `checker()` (returning the per-HW list of `CheckFunction`s) and the individual checks: assert pre-`main` state is zero, call `validator.start_main_thread()`, sleep briefly, then compare register state against an `expected[]` array (e.g. `expectedGpioSetup`). `validator()` is declared `__attribute__((weak))` so each HW's strong override wins at link time.

Writing a check typically looks like: build an `expected` array of `GpioSetup` / `AdcSetup` / register values, run the student thread, then call `check_compare(actual, expected, "name")`.

## Code generation tools

`tools/` contains Python regex-based generators that scan TI headers (e.g. `lib/C2000Ware_4_01_00_00/.../f2837xd_gpio.h`) and emit:

- `validation.py::generate_zero_checks` → `src/checks/generated.cpp` + `include/checks/generated.hpp` (zero-check overloads for every register struct).
- `compare_validation.py::generate_zero_checks` → `src/checks/compare_generated.cpp` + `include/checks/compare_generated.hpp` (pairwise compare overloads).
- `hash_map_setter.py::generate_hashmap_initializer` → body of `HardwareStateValidator::populate_all_zero()` in `src/checks/stat_checker.cpp`.

No CLI; invoke from a one-liner, e.g.
`python -c "from tools.validation import generate_zero_checks; print(generate_zero_checks(open('lib/.../f2837xd_gpio.h').read()))"`.
When TI headers change or a new peripheral is added, rerun the relevant generator and replace the corresponding file.

## Useful regex from README

To convert raw TI calls in student code into `expected[]` initializers for a new HW check:

```
GPIO_SetupPinMux\((\d+),(.+)\);\n+\s+GPIO_SetupPinOptions\(\d+,(.+)\);
→  expectedGpioSetup[$1] = {$2, $3};
```
