# HW1 Grader Roadmap

**Authoritative spec:** `context/homeworks/HW1.tex`
**Reference fixture (non-authoritative):** `context/code_solutions/myHW_Yingyord_First/HW1/HW1fy_main.c`
**Implementation:** `src/checks/hw1.cpp`, `include/checks/hw1.h`

> The reference solution intentionally deviates from spec for Exercise 9 (LED10/11 toggle and PB1/PB4 button-press LED toggles are commented out, replaced by a creative "F" letter pattern). Per the authority hierarchy, the spec wins; the reference is *expected to fail* the Exercise 9 checks. This is documented evidence that the checker enforces the spec.

## Spec inventory (verbatim or paraphrased from HW1.tex)

| Exercise | Behavior                                                                                       | Type            |
| -------- | ---------------------------------------------------------------------------------------------- | --------------- |
| Ex. 5    | Modify CpuTimer2 period or modulus so `serial_printf` fires every 250 ms.                       | Cadence         |
| Ex. 7    | `float saturate(float input, float saturation_limit)` — clamp to `[-saturation_limit, +saturation_limit]`. No globals. | Pure function   |
| Ex. 8    | Globals: `sinvalue=0`, `time=0`, `ampl=3.0`, `frequency=0.05`, `offset=0.25`, `timeint=0` (`int32_t`), `satvalue=0`. Inside the `if (UARTPrint == 1)` block before `serial_printf`: `timeint = timeint+1; time = timeint*0.25; sinvalue = ampl*sin(2*PI*frequency*time)+offset; satvalue = saturate(sinvalue, 2.65);`. Print format: `"Timeint = %ld, Time = %.2f sec, Input = %.3f, SatOut = %.2f\r\n"` with `(timeint, time, sinvalue, satvalue)`. | Math + format   |
| Ex. 9    | Inside `cpu_timer2_isr`, every 100 ms: always toggle LED10 (GPIO27) and LED11 (GPIO60). If PB1 (GPIO4) pressed (DAT==0), toggle LED12 (GPIO61) and LED13 (GPIO157). If PB4 (GPIO7) pressed, toggle LED14 (GPIO158) and LED15 (GPIO159). | Stimulus → state |
| Ex. 10   | Creative LED display — not auto-graded.                                                         | Skip            |

## Stubs (already exist; no new work)

- [x] `GPIO_SetupPinMux`, `GPIO_SetupPinOptions` — populate `gpiosSetup[]`
- [x] `ConfigCpuTimer` — populate `CpuTimer{0,1,2}` `PeriodInUSec` and `CPUFreqInMHz`
- [x] **Shared with HW2+:** `serial_printf` / `UART_printfLine` capture into `g_printfCalls` — `src/ti_stubs.cpp:143` records `{port, fmt, parsed_specifiers, rendered, synthetic_timestamp_us}` per call.
- [x] **Shared with HW2+:** stimulus helpers in `include/checks/stimulus.hpp` — `press_button` / `release_button` / `inject_adc_result` / `inject_spi_rx` / `inject_encoder_count` / `inject_lidar_frame`.
- [x] **Shared with HW2+:** synthetic-clock + `run_isr_for_us` in `include/checks/synthetic_clock.h`.
- [x] **Shared with HW2+:** format-string parser + `expect_format` / `expect_arg_types` / `expect_print_cadence` (`include/checks/format_parser.h`, `include/checks/expectations.h`). Run `./AutomaticGrader --selftest` to exercise the infrastructure end-to-end.

## Checks to implement / fix in `src/checks/hw1.cpp`

- [x] `check_initialization` — Phase 1 zero-state + Phase 2 expected post-init regs (already validated; no changes)
- [x] `check_timer0` — drives `cpu_timer0_isr()` 250×; asserts `numTimer0calls += 250`, `CpuTimer0.InterruptCount += 250`, `LEDdisplaynum += 1`, `GPBTOGGLE.bit.GPIO34 == 1`. Snapshot/restore.
- [x] `check_timer1` — drives `cpu_timer1_isr()` 100×; asserts `CpuTimer1.InterruptCount += 100`. Snapshot/restore.
- [x] `check_saturate` — sweep over `{-10, -2, -1, -0.5, 0, 0.5, -1, -2, -10} × {0.25, 1.0, 1.5, 2.0, 15.0}`.
- [ ] **Rework `check_timer2` to spec-strict mode**:
  - [ ] Always: after 1 ISR call, `GPATOGGLE.bit.GPIO31 = 1` (blue LED). After N=`100ms/period` ticks, `GPATOGGLE.bit.GPIO27 = 1` and `GPBTOGGLE.bit.GPIO60 = 1` (LED10/11).
  - [ ] With `release_button(4)`, after the 100 ms boundary: `GPBTOGGLE.bit.GPIO61 = 0` and `GPETOGGLE.bit.GPIO157 = 0` (no toggle).
  - [ ] With `press_button(4)`, after the 100 ms boundary: `GPBTOGGLE.bit.GPIO61 = 1` and `GPETOGGLE.bit.GPIO157 = 1`.
  - [ ] With `release_button(7)`: no GPIO158/159 toggle.
  - [ ] With `press_button(7)`: `GPETOGGLE.bit.GPIO158 = 1` and `GPETOGGLE.bit.GPIO159 = 1`.
  - [ ] After 250 ms of synthetic time, `CpuTimer2.InterruptCount` advanced by `250000 / period_us`.
  - [ ] After each block, snapshot/restore `GpioDataRegs`, `UARTPrint`, `CpuTimer2.InterruptCount`.
- [ ] `check_print_cadence` (new) — `grader::resetPrintfCapture()`, `grader::run_isr_for_us(cpu_timer2_isr, CpuTimer2.PeriodInUSec, 1'000'000)` (1 s synthetic), then `grader::expect_print_cadence(grader::SerialPort::SCIA, 4, 0.0)` (cadence is integer; tolerance 0).
- [ ] `check_print_format` (new) — inspect `grader::latestPrintfCall(grader::SerialPort::SCIA)`. Accept either:
  - Ex. 5 untouched: `expect_format(call, "Num Timer2:%ld Num SerialRX: %ld\r\n")` and `expect_arg_types(call, {grader::ArgType::Int32, grader::ArgType::Int32})`
  - Ex. 8 final: `expect_format(call, "Timeint = %ld, Time = %.2f sec, Input = %.3f, SatOut = %.2f\r\n")` and `expect_arg_types(call, {grader::ArgType::Int32, grader::ArgType::Float, grader::ArgType::Float, grader::ArgType::Float})`
  - **Reject** any `%d` where `%ld` is required (the most common student bug).
  - **Reject** `%.2f` where `%.3f` is required for `sinvalue`.
- [ ] `check_sin_math` (new, **[racy]**) — set `UARTPrint = 1`, sleep 50 ms to let the main thread's while-loop process one cycle, then assert: `timeint == previous + 1`, `time == timeint * 0.25` (exact), `sinvalue ≈ 3.0 * sin(2π · 0.05 · time) + 0.25` ± 1e-3, `satvalue == saturate(sinvalue, 2.65)` (exact). If race-driven flakiness shows up in CI, downgrade to format-only verification.

## Bugs already fixed

- [x] `hw1.cpp:25` — `int success = 0; success &= …` always returned 0; fixed to `1`.
- [x] `hw1.cpp:38` — duplicate `expected[31] = …` deleted.
- [x] `Validator::check()` — was returning only the last check's result; now AND-accumulates.
- [x] `main.cpp` — pass/fail polarity was inverted; now `result == 1` is success and exit 0.

## Validation matrix (deep)

- [x] Reference (`HW1fy_main.c`) — current state passes the *loose* checker. **After the spec-strict rework**: reference will fail Exercise 9 LED10/11 + button checks. Document the failing field names here when reworked.
- [ ] Spec-compliant fixture — hand-author by uncommenting the LED10/11 toggle and button-press LED blocks in the reference and fixing `(CpuTimer2.InterruptCount % 20) == 0` to drive the 100 ms boundary correctly. Required: `Succeeded all tests!`, exit 0, zero warnings.
- [x] Mutation A — `ConfigCpuTimer(..., 5000)` → `..., 6000` → fails on `CpuTimer2 PeriodInUSec`
- [x] Mutation B — delete `numTimer0calls++` → fails on `numTimer0calls expected 250 but was 0`
- [x] Mutation C — delete `GPIO_SetupPinOptions(31, …)` → fails on `GpioSetup[31] Output type`
- [x] Mutation D — break `saturate` (drop positive branch) → fails on `saturate(0.5, 0.25) = 0.25 got 0.5`
- [ ] Mutation E (post-rework) — `%ld` → `%d` in the Ex. 8 print → `check_print_format` fails on `argument #1: expected %ld (ArgType::Int32), got %d (ArgType::Int16)` plus the 16-bit-`int` hint from `expectations.cpp`.
- [ ] Mutation F (post-rework) — change print modulus from 50 to 100 → `check_print_cadence` fails on `expected 4 prints in 1000 ms, got 2`
- [ ] Mutation G (post-rework) — comment out `if (GPADAT.bit.GPIO4 == 0)` branch → `check_timer2[PB1]` fails on missing GPIO61/157 toggle
- [x] Three consecutive runs against current reference → identical pass (snapshot hygiene clean)

## Open questions / [ambiguous] from spec

- [ ] Spec doesn't pin which exact mechanism the student uses to hit 250 ms (timer period change vs modulus change). The cadence test is mechanism-agnostic.
- [ ] Spec doesn't say whether `serial_printf` should keep the original Ex. 5 format string after Ex. 8 is implemented. Reference replaces it; we accept either.
- [ ] `check_sin_math` is race-prone. If it flakes >5% in CI, drop it and rely on the format/cadence checks alone — those also catch broken sine logic indirectly (mismatched `sinvalue` precision would show up in render output if we add a parse-rendered-output assertion later).
- [ ] HW1.tex Exercise 9 says "every 100 ms" — assumed to mean modulus on `CpuTimer2.InterruptCount` matching the post-Ex.5 print rate. Confirm with course staff.
