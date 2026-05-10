# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

> **Human-facing documentation** lives on the docs site at
> <https://marius-juston.github.io/AutomaticGrader/> (source under `docs/`,
> built with MkDocs Material). When the human-facing material diverges from
> what's in this file, prefer the docs site — this file is intentionally
> Claude-focused operational guidance.

## What this project is

Auto-grader for SE 423 (UIUC) homework and lab code targeting the TI C2000 TMS320F28379D LaunchPad. It compiles the
student's firmware and runs it on a Linux host: `main()` is renamed to `temp_main`, launched on a background thread, and
checker functions inspect simulated peripheral/register state to grade the submission.

Designed to be consumed as a CI/CD subcomponent of a larger course repo. The outer repo provides:

- `workspace/` — student source trees, with a manifest mapping `<assignment ID> <relative path>` per line so the grader
  knows which folder is HW1, Lab3, etc. (Not present in this repo on its own; created by the parent repo / CI.)
- `context/homeworks/HW{N}.tex` (HW1..HW6) — **authoritative** spec for each homework.
- `context/lab/Lab{N}.txt` (Lab1, Lab2, Lab3, Lab4, Lab5, Lab6, Lab7-1, Lab7-2) — **authoritative** spec for each lab.
- `context/code_solutions/` — *non-authoritative* example student solutions, retained as positive-test fixtures and as a
  structural reference when designing capturing stubs. The Garcia PDF reference was removed; only
  `myHW_Yingyord_First/HW{N}/` and `SE423_Lab_phuriy2_lourdes3_emilyah3/Lab*/` remain. **Where a reference deviates from
  the spec, the spec wins** — the reference is allowed to fail checks (and that failure should be documented in the
  corresponding roadmap, not silenced by loosening the check).

The grader emits PASS/FAIL feedback to the GitHub Action log and to a markdown report.

## Authority hierarchy

1. The LaTeX HW spec or `.txt` Lab spec is absolute. Every checker assertion must trace to a quoted spec sentence.
2. Where the spec is genuinely ambiguous (e.g. "print every 100 ms" without a precision specifier), tag the assertion *
   *[ambiguous]** in the roadmap and pick the most common interpretation. Course staff resolve later.
3. Reference solutions inform stub design (which TI driverlib functions to capture, which globals must be `extern`-able)
   and serve as one positive-test fixture. They do not dictate behavior.

## Cooperative main-loop driver (single-threaded execution model)

**Important**: the grader does **not** run the student's `temp_main` on a detached thread. At
build time, `tools/patch_student_source.py` rewrites the student source so that:

1. Inside `main()`'s opening brace, a `GRADER_MAIN_INIT_GUARD` macro is injected: a static
   flag plus `goto _grader_loop_start;` that skips all init code on every re-entry after
   the first.
2. `while (1)` is replaced with `_grader_loop_start: ; GRADER_MAIN_LOOP`, where
   `GRADER_MAIN_LOOP` expands to `for (int _i = 0; _i < grader_main_loop_iterations(); ++_i)`.

The driver lives in `src/checks/main_loop_driver.cpp` (header
`include/checks/main_loop_driver.h`) and exposes:

| API                                                             | What it does                                                                                                                                                                                |
|-----------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `grader::run_student_init()`                                    | Calls `temp_main()` once with iterations = 0. Init runs, the patched loop body executes 0 times, main returns. **`Validator::start_main_thread()` is now a thin wrapper around this call.** |
| `grader::step_main_loop(n)`                                     | Re-enters `temp_main()` with iterations = n. The init-guard goto skips init, so only the body runs n times.                                                                                 |
| `grader::drive_isr_with_main_pump(isr, period_us, total_ticks)` | Lockstep driver: for `total_ticks` iterations, advance the synthetic clock, fire one ISR, then run one main-loop iteration. **Use this for every print/cadence check.**                     |
| `grader::reset_student_init()`                                  | Test-only; forces the next `run_student_init()` to re-run init.                                                                                                                             |

**Why this matters**:

- No detached thread means no `volatile`-qualifier issues on student-side polled flags
  (`UARTPrint`, etc.) and no need for real-time sleeps to "let the main thread catch up".
- Cadence is exact: each ISR is paired with exactly one main-loop iteration, so a print
  gated by `if (Counter % 50 == 0) UARTPrint = 1;` produces deterministic output.
- Format checks see a populated capture log every time — no "no SCIA prints captured"
  flake.
- The init-guard handles even student code with **busy-waits in init** (the HW3
  reference's `setupSpib()` polls `SpibRegs.SPIFFRX.bit.RXFFST`); the busy-wait runs only
  on first call. For the *first* init run, HW3's `check_initialization` still spawns a
  parallel `std::jthread` watchdog (`hw3_unblock_setup_spib`) that feeds the FIFO state
  register the values the busy-wait expects.

**When writing a new print check**: do NOT call `grader::run_isr_for_us` followed by
`std::this_thread::sleep_for`. Use `grader::drive_isr_with_main_pump(...)` and assert with
`grader::expect_print_cadence(..., tolerance=0.10)`. Tighten the tolerance — the loose
±25–30% slop in the old code only existed to mask races that no longer exist.

**Constraints the rewrite imposes on student code**:

- Student must have exactly one `while (1)` (or any whitespace variant matching
  `while\s*\(\s*1\s*\)`) inside `main`. `for (;;)` and `while (true)` are *not* currently
  rewritten — extend `tools/patch_student_source.py` if a fixture needs them.
- Init code must be idempotent if a check legitimately needs to run init twice
  (e.g. cross-fixture tests); we currently don't, but `reset_student_init()` is the
  escape hatch.
- The patched student source lives at `${BUILD}/student_patched/student.c`; inspect it
  when debugging a fixture mismatch.

## Three new test categories (apply uniformly across HW + Lab)

1. **Stimulus simulation.** Drive the stimuli the student is supposed to react to:
    - `press_button(gpio_num)` / `release_button(gpio_num)` for PB1..PB4 / joystick.
    - `inject_adc_result(module, soc, value)` for photodiodes / joystick / mic / gyro voltages.
    - `inject_spi_rx(module, word)` for MPU-9250 / DAN28027 reads.
    - `inject_encoder_count(module, count)` for EQEP-driven velocity tests.
    - `inject_lidar_frame(distances)` (`std::span<const float, 228>`) for Lab6.
      These helpers live in `include/checks/stimulus.hpp` and operate by writing to the stub-side `*Regs`. Always pair a
      stimulus with the spec'd response assertion (LED toggle, voltage scaling, state transition). *Shipped in slice 1 —
      see `include/checks/stimulus.hpp` + `src/checks/stimulus.cpp`.*

2. **Printf format-string verification.** TI C2000 has 16-bit `int`; the host Linux build has 32-bit `int`. A student
   writing `%d` for an `int32_t` works on the grader's host but fails on the target — this is the *single most common*
   bug in SE 423 submissions and rendered-output inspection cannot catch it. Solution: intercept `serial_printf` at the
   call site in `src/ti_stubs.cpp` and record
   `{port, fmt_string, parsed_specifier_sequence, rendered_output, synthetic_timestamp_us}` into `g_printfCalls`.
   Checkers then call `expect_format(call, "Timeint = %ld, Time = %.2f sec, …")` which parses both expected and actual
   format strings into a sequence of `(specifier, length_modifier, precision)` tuples and compares — tolerating
   whitespace / static-text differences but flagging any specifier-vs-required-type mismatch (`%d`-for-`int32_t`, `%f`
   -for-`%.2f`, missing `\r\n`, etc.). *Shipped in slice 1 —
   see `include/checks/format_parser.h`, `include/checks/printf_capture.h`, `include/checks/expectations.h`.*

3. **Print cadence.** Each checker call advances a synthetic-clock counter by `CpuTimerN.PeriodInUSec` per ISR
   invocation. After driving the ISR for a known synthetic duration, assert `g_printfCalls.size()` matches the spec's
   required rate within ±10%. Decoupled from wall-clock racing. *Shipped in slice 1 —
   see `include/checks/synthetic_clock.h` (`run_isr_for_us`) and `expect_print_cadence` in `expectations.h`.*

## Per-assignment status and roadmaps

Each assignment has a checklist at the repo root tracking required stubs, check functions, and the deep-validation
matrix (mutation tests, snapshot hygiene, cross-assignment regression). When implementing or modifying a checker, update
the matching roadmap as boxes are checked.

| Assignment   | Checker file            | Status                                                                                                                                                                                                                                                                                                                                                                                 |
|--------------|-------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| HW1          | `src/checks/hw1.cpp`    | **Done.** Init + timer0/1/2 + saturate + spec-strict `check_timer2` (PB1/PB4 stimulus + LED10–15 toggles) + `check_print_cadence` + `check_print_format`. `check_sin_math` deferred (race-prone). LABstarter correctly fails 5-arg format + edge-on-release LED bugs.                                                                                                                  |
| HW2          | `src/checks/hw2.cpp`    | **Done (4 of 7 checks active).** Init (EPWM12/EPWM4/ADCA register state) + `check_adc_isr_scaling` + cadence + format all pass against `myHW_Yingyord_First/HW2`. `check_pwm_ramp` deferred (needs EPWM-SOC trigger plumbing); LED-bar/joystick checks deferred (reference comments those out).                                                                                        |
| HW3          | `src/checks/hw3.cpp`    | **Done (4 of 6 checks active).** Init (SPIB / EPWM9 / pin muxes) + `check_spib_isr_scaling` + cadence + format pass against `myHW_Yingyord_First/HW3`. `check_spib_request_format` and `check_imu_full_init` SKIPPED until SPI TX-side word log lands in slice 4. Reference busy-waits on `SPIFFRX.RXFFST`; `hw3_unblock_setup_spib()` walks the value sequence as a slice-3 watchdog. |
| HW4          | `src/checks/hw4.cpp`    | **Done (6 of 6 checks active).** Init (EPWM8 servo carrier, GPIO14/15 mux, XINT1/XINT2 polarity+enable, GPIO6/7 qual=2, `xintPinMap[1..2]`) + servo-sweep × 2 + xint count × 2 + format pass against `myHW_Yingyord_First/HW4`. New stub: `GPIO_SetupXINT{1..5}Gpio` records `xintPinMap[]`.                                                                                           |
| HW5          | `src/checks/hw5.cpp`    | **Done (4 of 7 checks active).** Init (EPWM7 SOC + DMA CH1 src/dest/size/mode/run + PIE) + servo carryover + ping-pong swap + print format pass against `myHW_Yingyord_First/HW5`. `check_peak_detection` deferred (RFFT shim is no-op); `check_print_cadence` skipped (spec ambiguous).                                                                                               |
| HW6          | out-of-scope (firmware) | A* + Kalman — route to a separate harness in the parent CI repo.                                                                                                                                                                                                                                                                                                                       |
| Lab1..Lab7-2 | not yet created         | Roadmaps land first; CMake `ASSIGNMENT=` selector supports them. `.cpp` checkers deferred.                                                                                                                                                                                                                                                                                             |

The plan that drives this is at `~/.claude/plans/given-the-homework-solutions-delightful-anchor.md`.

## Build selector: `ASSIGNMENT`

`CMakeLists.txt` exposes an `ASSIGNMENT` cache variable. Valid values: `HW1`..`HW6`, `Lab1`..`Lab6`, `Lab7-1`, `Lab7-2`.
Legacy `HW=N` env var is mapped to `ASSIGNMENT=HW${N}` for back-compat. Switching assignments requires re-running
cmake (the value is `FORCE`d into the cache).

```bash
ASSIGNMENT=HW1   cmake -S . -B cmake-build-debug && cmake --build cmake-build-debug
ASSIGNMENT=Lab3  cmake -S . -B cmake-build-debug && cmake --build cmake-build-debug
HW=1             cmake -S . -B cmake-build-debug && cmake --build cmake-build-debug   # back-compat
```

The selector defines either `ASSIGNMENT_HW` + `HW=N` or `ASSIGNMENT_LAB` + `LAB=N` macros. `src/checks/validator.cpp`
uses these to include the right `hw{N}.h` / `lab{N}.h`.

## Adding a new assignment checker — required workflow

1. **Read the spec first** — `context/homeworks/HW{N}.tex` or `context/lab/Lab{N}.txt`. Quote spec sentences in the
   roadmap as the source-of-truth for each assertion.
2. Read `progress/HW{N}_ROADMAP.md` / `progress/Lab{N}_ROADMAP.md`; it lists the exact stubs and checks derived from
   step 1.
3. Add capturing stubs to `src/ti_stubs.cpp` (mirror the `GPIO_SetupPinMux` pattern at `src/ti_stubs.cpp:155` — read the
   register pointer, write the bitfield, return). Prefer extending the existing `*Regs` global over inventing a new
   shadow array, so existing `check_compare` overloads pick up the state for free. For the *shared* test categories (
   printf capture, stimulus, synthetic clock, expectations, cooperative main-loop driver), do not re-implement —
   consume the APIs from `include/checks/printf_capture.h`, `include/checks/stimulus.hpp`,
   `include/checks/synthetic_clock.h`, `include/checks/expectations.h`, and `include/checks/main_loop_driver.h`.
   Capture is automatic: every `serial_printf` / `UART_printfLine` call already lands in `g_printfCalls` with the SCI
   port classified — you only need to extend if a *new* SCI port (beyond SCIA/B/C/D/UART_LCD) is added. Use
   `grader::drive_isr_with_main_pump(...)` for any check whose effect is observed via a print or any flag the main
   loop body consumes; `run_isr_for_us` remains correct only for ISR-internal state checks.
4. Add `src/checks/{hw,lab}{N}.cpp` and `include/checks/{hw,lab}{N}.h` mirroring `hw1.cpp`/`hw1.h`. Define `checker()`
   returning the `CheckFunctions` list. Use the four-phase pattern below.
5. Walk the validation matrix at the bottom of the roadmap: build, run reference solution(s), run at least 3 mutated
   copies and confirm each fails on the *exact* register/field, run twice without rebuilding to confirm snapshot
   hygiene, and re-run all prior HWs to catch regressions.
6. Update the roadmap (check the boxes) as part of the same commit.

## Four-phase check pattern (from hw1.cpp + new test categories)

- **Phase 1 — pre-main zero state.** `HardwareStateValidator v; v.populate_all_zero(); v.validate();`. Done once in the
  first check; confirms registers start clean.
- **Phase 2 — post-init state.** `val->start_main_thread()` (sleeps 10 ms), then build `expected` structs and call
  `register_comparison*` / `register_custom*` for `gpiosSetup[]`, timers, IER, PieCtrlRegs, PieVectTable, GpioDataRegs
  initial set/clear bits. End with `v.validate()`.
- **Phase 3 — ISR-driven dynamics with stimuli.** Snapshot volatile globals (
  `const GPIO_DATA_REGS initial = GpioDataRegs; uint16_t saveU = UARTPrint;`). Apply stimuli (`grader::press_button(4)`,
  `grader::inject_adc_result(grader::AdcModule::A, 0, 4095)`, `grader::inject_spi_rx(grader::SpiModule::B, 0x4000)`,
  `grader::inject_lidar_frame(...)`, etc.) before each ISR loop. For ISR-only state (LED toggles, register snapshots
  at boundaries) use `grader::run_isr_for_us(isr, period_us, total_us)` directly. **For any check whose effect is
  observed via a print** (or any other state set in main's while-loop body), use
  `grader::drive_isr_with_main_pump(isr, period_us, total_ticks)` — it pairs each ISR call with exactly one main-loop
  iteration via the cooperative driver, so flag-gated prints fire deterministically. Register expectations at
  predicted tick boundaries (e.g. `expectedTimeStep = 250000.f / CpuTimer2.PeriodInUSec`), validate, **then restore
  snapshots before returning** so subsequent checks aren't polluted.
- **Phase 4 — print cadence + format.** Call `grader::resetPrintfCapture()` before driving the ISR loop in Phase 3 (this
  also resets the synthetic clock). Use `grader::drive_isr_with_main_pump(...)` (NOT `run_isr_for_us` + sleeps) — the
  former is single-threaded and deterministic. After the loop, call
  `grader::expect_print_cadence(grader::SerialPort::SCIA, expected_count, 0.10)` and
  `grader::expect_format(grader::latestPrintfCall(grader::SerialPort::SCIA), "fmt with correct %ld/%.2f/%.3f specifiers")`.
  Cadence uses synthetic timestamps (no wall-clock racing); format check parses both expected and actual format strings
  into specifier sequences. **Tolerance ≤ ±10%** — the old slack of ±25–30% was masking the race that the cooperative
  driver eliminates.

`HardwareStateValidator` registration variants:

- `register_comparison(name, actual, expected)` — by reference; safe when `actual` won't change before `validate()`.
- `register_comparison_copy(name, actual, expected)` — captures by value; required for state mutated between
  registration and validation (e.g. `GpioDataRegs` between ISR ticks).
- `register_custom*` — supply your own predicate (HW1 uses this for `IER` bitmask and `UARTPrint` toggling).
- `mark_as_used(name)` — opt out of `populate_all_zero` for registers the student is expected to mutate.

`PrintfCall` capture API (declared in `include/checks/printf_capture.h`; population point is `serial_printf` /
`UART_printfLine` in `src/ti_stubs.cpp`):

- `g_printfCalls` — append-only `std::vector<grader::PrintfCall>` in the **global** namespace. Each entry has
  `{port, fmt, parsed, rendered, synthetic_timestamp_us, sequence_index, lcd_line}`.
- `grader::resetPrintfCapture()` — clear the vector *and* reset the synthetic clock. Call at the top of every Phase-3
  block.
- `grader::expect_format(call, expected_fmt, name)` — assert specifier sequences match (length-modifier, precision,
  conversion). Tolerates whitespace and static-text differences.
- `grader::expect_arg_types(call, {grader::ArgType::Int32, grader::ArgType::Float, ...}, name)` — assert the parsed type
  list matches what the spec demands.
- `grader::expect_print_cadence(port, expected_count, tolerance_pct, name)` — assert exactly N calls within tolerance.
- Convenience accessors: `grader::latestPrintfCall(port)`, `grader::getPrintfCallsForPort(port)`,
  `grader::printfCallCount(port)`, `grader::dumpPrintfCalls()`.

`stimulus.hpp` helpers (peripheral state injection — all in the `grader::` namespace):

- `press_button(int gpio)` / `release_button(int gpio)` / `set_gpio_input(int gpio, bool high)` /
  `read_gpio_input(int gpio)` — manipulate `GPxDAT.bit.GPIO{N}`.
- `inject_adc_result(AdcModule, soc_idx, uint16_t value)` — write `Adc{a..d}ResultRegs.ADCRESULT{N}`. Modules:
  `AdcModule::A..D`.
- `inject_spi_rx(SpiModule, uint16_t word)` and `inject_spi_rx(SpiModule, std::span<const uint16_t>)` — push into
  `SPIRXBUF` and bump `SPIFFRX.RXFFST`. Modules: `SpiModule::A..C`.
- `inject_encoder_count(EqepModule, int32_t count)` — write `EQep{1,2,3}Regs.QPOSCNT`. Modules:
  `EqepModule::Eqep1..Eqep3`.
- `inject_lidar_ping(distances)` / `inject_lidar_pong(distances)` / `inject_lidar_frame(distances)` — `distances` is
  `std::span<const float, 228>`; `_frame` writes both ping and pong.
- `clear_adc_results(module)` / `clear_spi_state(module)` / `clear_lidar_frame()` / `reset_all_stimulus()` — tear-down
  helpers.
- `grader::run_isr_for_us(IsrFn, period_us, total_us)` (in `synthetic_clock.h`) — calls `IsrFn()` `total_us / period_us`
  times, advancing the synthetic clock each call.

## Shared check infrastructure (slice 1 — shipped)

All five headers below are part of the `grader_infra` object library and are already wired into every assignment build.
Future checkers consume them directly; do not duplicate the functionality.

| Header                              | Purpose                                                               | Key symbols                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
|-------------------------------------|-----------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `include/checks/format_parser.h`    | C printf format-string parser.                                        | `grader::FormatSpec`, `grader::ParsedFormat`, `grader::ArgType`, `parse_format()`, `expected_arg_type()`, `format_specs_equivalent()`, `format_spec_canonical()`.                                                                                                                                                                                                                                                                                                                        |
| `include/checks/synthetic_clock.h`  | Tick-based clock decoupled from wall time.                            | `grader::synthetic_clock_now_us()`, `synthetic_clock_advance(us)`, `synthetic_clock_reset()`, `run_isr_for_us(isr, period_us, total_us)`.                                                                                                                                                                                                                                                                                                                                                |
| `include/checks/printf_capture.h`   | Captures every `serial_printf` / `UART_printfLine` call.              | `grader::PrintfCall`, `grader::SerialPort` (`SCIA`/`SCIB`/`SCIC`/`SCID`/`UART_LCD`), `resetPrintfCapture()`, `clearPrintfCalls()`, `getPrintfCalls()`, `getPrintfCallsForPort(port)`, `latestPrintfCall(port)`, `printfCallCount(port)`, `dumpPrintfCalls()`, the global `std::vector<grader::PrintfCall> g_printfCalls`.                                                                                                                                                                |
| `include/checks/stimulus.hpp`       | Drive simulated peripheral state.                                     | `grader::press_button(int)`, `release_button(int)`, `set_gpio_input(int,bool)`, `read_gpio_input(int)`, `inject_adc_result(AdcModule, soc, value)`, `inject_spi_rx(SpiModule, word \| std::span)`, `spi_rx_fifo_size`, `clear_spi_state`, `inject_encoder_count(EqepModule, int32_t)`, `inject_lidar_ping/pong/frame(std::span<const float, 228>)`, `clear_lidar_frame`, `reset_all_stimulus`. Modules: `AdcModule::{A,B,C,D}`, `SpiModule::{A,B,C}`, `EqepModule::{Eqep1,Eqep2,Eqep3}`. |
| `include/checks/expectations.h`     | Pointed assertion APIs over the capture log.                          | `grader::expect_format(call, expected_fmt, name)`, `expect_arg_types(call, {…}, name)`, `expect_print_cadence(port, count, tol_pct, name)`, `expect_print_cadence_window(port, min, max, name)`, `expect_min_print_calls(port, min, name)`, `expect_format_any(port, expected_fmt, name)`. Each emits a multi-line `spdlog::error` with student-debug hints (most importantly the TI-C2000 16-bit `int` warning when `%d` is used where `%ld` is required).                              |
| `include/checks/main_loop_driver.h` | Cooperative single-threaded driver for the student's main while-loop. | `grader::run_student_init()` (called by `Validator::start_main_thread()`), `grader::step_main_loop(n)`, `grader::drive_isr_with_main_pump(isr, period_us, total_ticks)`, `grader::reset_student_init()`. Backed by `tools/patch_student_source.py` (build-time rewrite of `while(1)` + injection of `GRADER_MAIN_INIT_GUARD`).                                                                                                                                                           |

`g_printfCalls` is declared in the **global** namespace (its element type is `grader::PrintfCall`); every other slice-1
symbol lives in the `grader::` namespace.

The capture pipeline records `{port, fmt, parsed, rendered, synthetic_timestamp_us, sequence_index}` for each call.
Format mismatches and cadence misses are reported with the offending argument index, the canonical spec form, and a hint
string.

**Slice 1 deferred** (will land in later slices, do **not** assume they exist yet): SCI RX simulation (
`inject_serial_rx`), SPI TX-side word log (`spibTxLog`), SWI post (`PostSWI*`), camera-blob injection (`inject_blob` /
`inject_optitrack`), EPWM-triggered ADC SOC plumbing (separate from `inject_adc_result`).

## Build & run

Linux only — relies on `__attribute__((weak))` (not available under MinGW). Requires `libspdlog-dev` and CMake ≥ 3.21,
C++20.

```bash
sudo apt install libspdlog-dev
ASSIGNMENT=HW1 cmake -S . -B cmake-build-debug && cmake --build cmake-build-debug
./cmake-build-debug/AutomaticGrader
```

`cmake-build-debug/` is the existing CLion build dir and already has `compile_commands.json`; prefer reusing it over
making a fresh `build/`.

Select an assignment via the `ASSIGNMENT` env var (or `-DASSIGNMENT=...`). Valid values: `HW1`..`HW6`, `Lab1`..`Lab6`,
`Lab7-1`, `Lab7-2`. Legacy `HW=N` env var still works (mapped to `ASSIGNMENT=HW${N}`). To switch between builds, re-run
cmake — `FORCE` in `CMakeLists.txt` overwrites the cache, but `make` alone won't notice the env var change. The
`AUTO_GRADER` macro is also defined globally; student code / stubs can branch on it.

If the requested checker source (`src/checks/lab{N}.cpp` etc.) does not yet exist, CMake emits a WARNING and falls back
to `src/checks/hw1.cpp`. Build and link still succeed but the grader will validate HW1 instead — handy for testing the
harness while the assignment-specific checker is being written.

The student firmware path is selectable via `STUDENT_SRC` (env var or `-DSTUDENT_SRC=…`). Default is
`student_work/LABstarter_main.c`. Common reference fixtures live under `context/code_solutions/`, e.g.:

```bash
ASSIGNMENT=HW2 STUDENT_SRC=context/code_solutions/myHW_Yingyord_First/HW2/HW2fy_main.c \
    cmake -S . -B cmake-build-debug && cmake --build cmake-build-debug
```

The path is validated at configure time; missing files fail-fast with a clear error.

There are no automated tests, lint config, or formatter wired up in this repo. Each assignment's `checker()` *is* the
test suite.

## Self-test mode

Run `./cmake-build-debug/AutomaticGrader --selftest` (or set `GRADER_SELFTEST=1`) to exercise the slice-1 infrastructure
end-to-end: format parser, synthetic clock, printf capture pipeline, expectation APIs, stimulus helpers. The self-test
reports a failure count and exits non-zero on any regression. **Run this before any non-trivial change to `grader_infra`
** — a green selftest is the contract that lets per-assignment checkers rely on the shared APIs without re-validating
them.

## Build organisation

`CMakeLists.txt` splits the build into five object libraries that compile in parallel:

- `grader_infra` — slice-1 shared APIs (format parser, capture, clock, stimulus, expectations, selftest).
- `grader_state_check` — heavy auto-generated state validators (`generated.cpp`, `compare_generated.cpp`,
  `stat_checker.cpp`, `validator.cpp`).
- `grader_stubs` — `ti_stubs.cpp` + the `.c` support files (`lidar.cpp`, `OptiTrack.c`, `LEDPatterns.c`, …).
- `grader_assignment` — the single per-assignment checker source picked by `ASSIGNMENT=…`.
- `grader_student` — `student_work/LABstarter_main.c` compiled with `-Dmain=temp_main`.

`ccache` is auto-detected via `find_program(CCACHE_PROGRAM ccache)` and wired in as `CMAKE_CXX_COMPILER_LAUNCHER`. Clean
rebuild on a 4-core machine is ~24 s; single-file incremental is ~2 s.

The student source under `student_work/LABstarter_main.c` is compiled with `-Dmain=temp_main` so it can coexist with
`src/main.cpp`. `-include ti_stubs.h` is force-included on every TU to neutralize TI-specific keywords (`__interrupt`,
`EALLOW`, `DATA_SECTION`, etc.).

## Architecture

Three layers:

1. **TI hardware shim** (`include/`, `src/ti_stubs.cpp`, `lib/C2000Ware_4_01_00_00/...`). Provides plain-memory
   definitions of every C2000 peripheral register struct (`GpioCtrlRegs`, `AdcaRegs`, `EPwm1Regs`, …) plus stubs for
   `GPIO_SetupPinMux`, `GPIO_SetupPinOptions`, `InitSysCtrl`, serial I/O, etc. The student code links against these
   instead of the real TI driverlib so it runs as a normal Linux process. `ti_stubs.h` is force-included to map TI-only
   attributes/types to no-ops or standard types (e.g. `Uint16 → uint16_t`, `__interrupt → ` empty).

2. **State tracking** (`src/checks/generated.cpp`, `src/checks/compare_generated.cpp`, `src/checks/stat_checker.cpp`).
   Generated C++ that knows how to recursively `check_zero(...)` and `check_compare(..., expected, ...)` every TI
   register struct. `HardwareStateValidator` (in `state_checker.h`) holds a `tracker_` map from register-name → lambda;
   `populate_all_zero()` registers a checker for each `extern struct` declared in the TI headers. Both `generated.cpp`
   and the populate map are *generated* by the Python tools — do not hand-edit them; regenerate instead (see below).

3. **Grading harness** (`src/main.cpp`, `src/checks/validator.cpp`, `src/checks/hw*.cpp`). `Validator` owns a list of
   `CheckFunction`s and a `std::jthread` running the student's `temp_main`. Each `hw${N}.cpp` defines `checker()` (
   returning the per-HW list of `CheckFunction`s) and the individual checks: assert pre-`main` state is zero, call
   `validator.start_main_thread()`, sleep briefly, then compare register state against an `expected[]` array (e.g.
   `expectedGpioSetup`). `validator()` is declared `__attribute__((weak))` so each HW's strong override wins at link
   time.

Writing a check typically looks like: build an `expected` array of `GpioSetup` / `AdcSetup` / register values, run the
student thread, then call `check_compare(actual, expected, "name")`.

## Code generation tools

`tools/` contains Python regex-based generators that scan TI headers (e.g.
`lib/C2000Ware_4_01_00_00/.../f2837xd_gpio.h`) and emit:

- `validation.py::generate_zero_checks` → `src/checks/generated.cpp` + `include/checks/generated.hpp` (zero-check
  overloads for every register struct).
- `compare_validation.py::generate_zero_checks` → `src/checks/compare_generated.cpp` +
  `include/checks/compare_generated.hpp` (pairwise compare overloads).
- `hash_map_setter.py::generate_hashmap_initializer` → body of `HardwareStateValidator::populate_all_zero()` in
  `src/checks/stat_checker.cpp`.

No CLI; invoke from a one-liner, e.g.
`python -c "from tools.validation import generate_zero_checks; print(generate_zero_checks(open('lib/.../f2837xd_gpio.h').read()))"`.
When TI headers change or a new peripheral is added, rerun the relevant generator and replace the corresponding file.

## Useful regex from README

To convert raw TI calls in student code into `expected[]` initializers for a new HW check:

```
GPIO_SetupPinMux\((\d+),(.+)\);\n+\s+GPIO_SetupPinOptions\(\d+,(.+)\);
→  expectedGpioSetup[$1] = {$2, $3};
```
