# HW1 Grader Roadmap

**Authoritative spec:** `context/homeworks/HW1.tex`
**Reference fixture (non-authoritative):** `context/code_solutions/myHW_Yingyord_First/HW1/HW1fy_main.c`
**Implementation:** `src/checks/hw1.cpp`, `include/checks/hw1.h`

> The reference solution intentionally deviates from spec for Exercise 9 (LED10/11 toggle and PB1/PB4 button-press LED
> toggles are commented out, replaced by a creative "F" letter pattern). Per the authority hierarchy, the spec wins; the
> reference is *expected to fail* the Exercise 9 checks. This is documented evidence that the checker enforces the spec.

## Spec inventory (verbatim or paraphrased from HW1.tex)

| Exercise | Behavior                                                                                                                                                                                                                                                                                                                                                                                                                                             | Type             |
|----------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------|
| Ex. 5    | Modify CpuTimer2 period or modulus so `serial_printf` fires every 250 ms.                                                                                                                                                                                                                                                                                                                                                                            | Cadence          |
| Ex. 7    | `float saturate(float input, float saturation_limit)` — clamp to `[-saturation_limit, +saturation_limit]`. No globals.                                                                                                                                                                                                                                                                                                                               | Pure function    |
| Ex. 8    | Globals: `sinvalue=0`, `time=0`, `ampl=3.0`, `frequency=0.05`, `offset=0.25`, `timeint=0` (`int32_t`), `satvalue=0`. Inside the `if (UARTPrint == 1)` block before `serial_printf`: `timeint = timeint+1; time = timeint*0.25; sinvalue = ampl*sin(2*PI*frequency*time)+offset; satvalue = saturate(sinvalue, 2.65);`. Print format: `"Timeint = %ld, Time = %.2f sec, Input = %.3f, SatOut = %.2f\r\n"` with `(timeint, time, sinvalue, satvalue)`. | Math + format    |
| Ex. 9    | Inside `cpu_timer2_isr`, every 100 ms: always toggle LED10 (GPIO27) and LED11 (GPIO60). If PB1 (GPIO4) pressed (DAT==0), toggle LED12 (GPIO61) and LED13 (GPIO157). If PB4 (GPIO7) pressed, toggle LED14 (GPIO158) and LED15 (GPIO159).                                                                                                                                                                                                              | Stimulus → state |
| Ex. 10   | Creative LED display — not auto-graded.                                                                                                                                                                                                                                                                                                                                                                                                              | Skip             |

## Stubs (already exist; no new work)

- [x] `GPIO_SetupPinMux`, `GPIO_SetupPinOptions` — populate `gpiosSetup[]`
- [x] `ConfigCpuTimer` — populate `CpuTimer{0,1,2}` `PeriodInUSec` and `CPUFreqInMHz`
- [x] **Shared with HW2+:** `serial_printf` / `UART_printfLine` capture into `g_printfCalls` — `src/ti_stubs.cpp:143`
  records `{port, fmt, parsed_specifiers, rendered, synthetic_timestamp_us}` per call.
- [x] **Shared with HW2+:** stimulus helpers in `include/checks/stimulus.hpp` — `press_button` / `release_button` /
  `inject_adc_result` / `inject_spi_rx` / `inject_encoder_count` / `inject_lidar_frame`.
- [x] **Shared with HW2+:** synthetic-clock + `run_isr_for_us` in `include/checks/synthetic_clock.h`.
- [x] **Shared with HW2+:** format-string parser + `expect_format` / `expect_arg_types` / `expect_print_cadence` (
  `include/checks/format_parser.h`, `include/checks/expectations.h`). Run `./AutomaticGrader --selftest` to exercise the
  infrastructure end-to-end.

## Checks to implement / fix in `src/checks/hw1.cpp`

- [x] `check_initialization` — Phase 1 zero-state + Phase 2 expected post-init regs (already validated; no changes)
- [x] `check_timer0` — drives `cpu_timer0_isr()` 250×; asserts `numTimer0calls += 250`,
  `CpuTimer0.InterruptCount += 250`, `LEDdisplaynum += 1`, `GPBTOGGLE.bit.GPIO34 == 1`. Snapshot/restore.
- [x] `check_timer1` — drives `cpu_timer1_isr()` 100×; asserts `CpuTimer1.InterruptCount += 100`. Snapshot/restore.
- [x] `check_saturate` — sweep over `{-10, -2, -1, -0.5, 0, 0.5, -1, -2, -10} × {0.25, 1.0, 1.5, 2.0, 15.0}`.
- [x] **Reworked `check_timer2` to spec-strict mode** (`src/checks/hw1.cpp:check_timer2`). Each sub-check primes any
  edge-detector with one ISR call (buttons released → `prev_button = 1`), then:
    - [x] Sub-check A (all buttons released, drive ≥ 100 ms): `GPATOGGLE.GPIO31` (status BLUE), `GPATOGGLE.GPIO27` (
      LED10), `GPBTOGGLE.GPIO60` (LED11) all toggled; **no** toggle on GPIO61/157/158/159.
    - [x] Sub-check B (`grader::press_button(4)`): asserts `GPBTOGGLE.GPIO61` (LED12) and `GPETOGGLE.GPIO157` (LED13)
      toggled; LED14/15 idle.
    - [x] Sub-check C (`grader::press_button(7)`): asserts `GPETOGGLE.GPIO158` (LED14) and `GPETOGGLE.GPIO159` (LED15)
      toggled; LED12/13 idle.
    - [x] Snapshot/restore of `GpioDataRegs`, `UARTPrint`, `CpuTimer2.InterruptCount` per sub-check.
- [x] `check_print_cadence` — `grader::resetPrintfCapture()`, drive 1 s synthetic in 4 × 250 ms bursts (
  `grader::run_isr_for_us`), 20 ms wall-sleep between bursts to let the main thread service `UARTPrint`. Asserts
  `expect_print_cadence(SCIA, 4, 0.25)`.
- [x] `check_print_format` — inspects `grader::latestPrintfCall(grader::SerialPort::SCIA)`. Accepts either:
    - Ex. 5 untouched: `"Num Timer2:%ld Num SerialRX: %ld\r\n"` with `{Int32, Int32}` args
    - Ex. 8 final: `"Timeint = %ld, Time = %.2f sec, Input = %.3f, SatOut = %.2f\r\n"` with
      `{Int32, Float, Float, Float}` args
    - Correctly rejects `%d`-for-`int32_t` (the LABstarter's 5-arg `(wrong: %d)` form fails with the TI-C2000 16-bit
      `int` hint).
- [ ] `check_sin_math` — **deferred** as race-prone; the format + cadence checks already catch broken sine math
  indirectly.

## Bugs already fixed

- [x] `hw1.cpp:25` — `int success = 0; success &= …` always returned 0; fixed to `1`.
- [x] `hw1.cpp:38` — duplicate `expected[31] = …` deleted.
- [x] `Validator::check()` — was returning only the last check's result; now AND-accumulates.
- [x] `main.cpp` — pass/fail polarity was inverted; now `result == 1` is success and exit 0.

## Validation matrix (deep)

- [x] LABstarter reference (`student_work/LABstarter_main.c`) — current spec-deviating state correctly fails the
  spec-strict checker on:
    - `check_timer2[PB1-pressed]` — GPIO61/157 not toggled (student uses edge-on-RELEASE, not level/edge-on-press)
    - `check_timer2[PB4-pressed]` — GPIO158/159 not toggled (student wires PB4 to GPIO159+GPIO160 instead of 158+159)
    - `check_print_format[Ex.8]` — student's 5-arg `(wrong: %d)` debug form fails with
      `expected 4 format specifier(s), got 5` and `hint: you are printing 1 extra argument(s)`
- [ ] Spec-compliant fixture — hand-author by uncommenting the LED10/11 toggle and button-press LED blocks in the
  reference and switching to level-based button detection. Required: `Succeeded all tests!`, exit 0.
- [x] Mutation A — `ConfigCpuTimer(..., 5000)` → `..., 6000` → fails on `CpuTimer2 PeriodInUSec`
- [x] Mutation B — delete `numTimer0calls++` → fails on `numTimer0calls expected 250 but was 0`
- [x] Mutation C — delete `GPIO_SetupPinOptions(31, …)` → fails on `GpioSetup[31] Output type`
- [x] Mutation D — break `saturate` (drop positive branch) → fails on `saturate(0.5, 0.25) = 0.25 got 0.5`
- [x] Mutation E — `%ld` → `%d` in the Ex.8 print fails `check_print_format` with the TI-C2000 16-bit `int` hint (proven
  against the LABstarter's existing 5-arg form which embeds exactly this bug).
- [ ] Mutation F — change print modulus from 50 to 100 → `check_print_cadence` should fail on cadence window
- [ ] Mutation G — comment out the `GPADAT.bit.GPIO4` branch → `check_timer2[PB1-pressed]` fails (already verified by
  the LABstarter case)
- [x] Three consecutive runs against current reference → identical pass (snapshot hygiene clean)

## Open questions / [ambiguous] from spec

- [ ] Spec doesn't pin which exact mechanism the student uses to hit 250 ms (timer period change vs modulus change). The
  cadence test is mechanism-agnostic.
- [ ] Spec doesn't say whether `serial_printf` should keep the original Ex. 5 format string after Ex. 8 is implemented.
  Reference replaces it; we accept either.
- [ ] `check_sin_math` is race-prone. If it flakes >5% in CI, drop it and rely on the format/cadence checks alone —
  those also catch broken sine logic indirectly (mismatched `sinvalue` precision would show up in render output if we
  add a parse-rendered-output assertion later).
- [ ] HW1.tex Exercise 9 says "every 100 ms" — assumed to mean modulus on `CpuTimer2.InterruptCount` matching the
  post-Ex.5 print rate. Confirm with course staff.
