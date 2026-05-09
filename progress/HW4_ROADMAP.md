# HW4 Grader Roadmap

**Authoritative spec:** `context/homeworks/HW4.tex`
**Reference fixture (non-authoritative):** `context/code_solutions/myHW_Yingyord_First/HW4/`
**Implementation:** `src/checks/hw4.cpp` (to create), `include/checks/hw4.h` (to create)

## Spec inventory (HW4.tex)

| Topic               | Behavior                                                                                                                                                                         |
|---------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| RC servo EPWM8A/B   | 50 Hz carrier (20 ms period). At 100 MHz EPWMCLK with `CLKDIV=4`, `TBPRD=62500`. Duty range: −90°→4% (CMPA=2500), 0°→8% (CMPA=5000), +90°→12% (CMPA=7500).                       |
| Required functions  | `void setEPWM8A_RCServo(float angle)` and `void setEPWM8B_RCServo(float angle)`. Saturate angle to [-90, 90]; map linearly to CMPA/CMPB.                                         |
| External interrupts | XINT1 on PB3 (GPIO6), XINT2 on PB4 (GPIO7); falling-edge polarity; GPIO qualify period 6 (debounce).                                                                             |
| ISRs                | `xint1_isr` increments `Xint1Count`; `xint2_isr` increments `Xint2Count`. Spec implies `IFR/PIEACK` cleanup (tested by re-entry).                                                |
| State machine       | 4 robot states (navigate, obstacle avoid w/ left wall-follow, right wall-follow, pink ball track). LIDAR 228 readings, element 113 = forward. **Mostly demo / not auto-graded.** |

## Stubs needed (`src/ti_stubs.cpp`)

- [x] All HW2 EPWM stubs (for EPWM8) — already shipped via `EPwm8Regs` global.
- [x] `GPIO_SetupXINT{1..5}Gpio(gpio_num)` — records `xintPinMap[N] = gpio_num`. Defined in `src/ti_stubs.cpp` alongside
  the new `xintPinMap[8]` global; declared in `include/ti_stubs.h`.
- [x] `XintRegs.XINT{1,2}CR.bit.{POLARITY,ENABLE}` — student writes the field directly; covered by the existing
  `XintRegs` global.
- [x] `GpioCtrlRegs.GPAQSEL1.bit.GPIO{6,7}` + `GPACTRL.QUALPRD0` — student writes directly; covered.
- [ ] (Newer driverlib) `GPIO_setInterruptPin/Type`, `XINT_enableInterrupt`, `GPIO_setQualificationMode` — not used by
  the FY reference; defer until a student fixture needs them.
- [x] **Shared (shipped):** `serial_printf` / `UART_printfLine` capture → `g_printfCalls`. Auto-wired in
  `src/ti_stubs.cpp`; consume via `include/checks/printf_capture.h`.
- [x] **Shared (shipped):** stimulus helpers — `include/checks/stimulus.hpp`. `grader::press_button(int gpio)` simulates
  the GPIO low; trigger XINT via direct `xint1_isr()` call.
- [x] **Shared (shipped):** synthetic clock + `run_isr_for_us` — `include/checks/synthetic_clock.h`.
- [x] **Shared (shipped):** cooperative main-loop driver — `grader::run_student_init()`,
  `grader::step_main_loop()`, `grader::drive_isr_with_main_pump()` in
  `include/checks/main_loop_driver.h`. Use `drive_isr_with_main_pump` for any check
  whose print path is gated by `UARTPrint` (or any flag set in an ISR and consumed in
  main’s while-loop body). The student source’s `while (1)` loop is patched at
  build time by `tools/patch_student_source.py`. See CLAUDE.md "Cooperative main-loop
  driver".
- [x] **Shared (shipped):** format parser + `expect_format` / `expect_arg_types` / `expect_print_cadence` —
  `include/checks/format_parser.h` + `include/checks/expectations.h`. Run `./AutomaticGrader --selftest` to verify the
  infra after edits.

## Checks to implement (`src/checks/hw4.cpp`)

**Status:** all six checks implemented and passing against `myHW_Yingyord_First/HW4/HW4fy_main.c`.

- [x] `check_initialization` — HW1 baseline + EPWM8 + XINT register state. Verify `gpiosSetup[40]` and `gpiosSetup[41]`
  muxes for EPWM8A/B (or whichever pins reference uses).
  `EPwm8Regs.{TBPRD == 62500, TBCTL.HSPCLKDIV == 4, AQCTLA.ZRO == 2, AQCTLA.CAU == 1, CMPA == 5000, CMPB == 5000}` (8%
  duty initial = 0°). `xintPinMap[XINT1] == 6`, `xintPinMap[XINT2] == 7`.
  `XintRegs.XINT1CR.{POLARITY == 0, ENABLE == 1}` and same for XINT2. GPIO6/7 input + pullup + qualify period 6.
  `PieVectTable.{XINT1_INT == &xint1_isr, XINT2_INT == &xint2_isr}`.
- [x] `check_setEPWM8A_RCServo` — direct call sweep:
    - `setEPWM8A_RCServo(0.0f)` → `EPwm8Regs.CMPA.bit.CMPA == 5000`
    - `setEPWM8A_RCServo(-90.0f)` → `CMPA == 2500`
    - `setEPWM8A_RCServo(+90.0f)` → `CMPA == 7500`
    - `setEPWM8A_RCServo(+200.0f)` → `CMPA == 7500` (saturated)
    - `setEPWM8A_RCServo(-200.0f)` → `CMPA == 2500` (saturated)
    - `setEPWM8A_RCServo(45.0f)` → `CMPA ≈ 6250` ±1 (linearity)
- [x] `check_setEPWM8B_RCServo` — same matrix asserting `CMPB.bit.CMPB`.
- [x] `check_xint1_button` — drives 5 `grader::press_button(6)` / `xint1_isr()` / `release_button(6)` cycles and asserts
  `Xint1Count` advanced by 5.
- [x] `check_xint2_button` — same for `Xint2Count` and GPIO7.
- [ ] `check_print_cadence` — **[ambiguous]** — HW4 spec doesn't pin a print rate for Ex. 4 button counts. Deliberately
  not implemented (would create false negatives for spec-compliant students).
- [x] `check_print_format` — `expect_arg_types(latest, {Int32, Int32})` (matches the reference's
  `"NumXInt1=%ld,NumXInt2=%ld\r\n"`). Returns 1 if no print is emitted (spec is loose).

## Validation matrix (deep)

- [x] Reference (`myHW_Yingyord_First/HW4/HW4fy_main.c`) → all 6 checks pass, "Succeeded all tests!"
- [ ] Mutation A: `TBPRD` 62500→50000 → fails on `EPwm8Regs TBPRD`
- [ ] Mutation B: drop `setEPWM8A_RCServo` saturation (let `+200` pass) → `CMPA` overflow → check fails
- [ ] Mutation C: flip XINT1 polarity 0→1 → fails on `XintRegs XINT1CR.POLARITY`
- [ ] Mutation D: drop GPIO qualify config → fails on `GpioCtrlRegs.GPACTRL.QUALPRD`
- [ ] Mutation E: bind XINT1 to GPIO5 instead of GPIO6 → fails on `xintPinMap[XINT1]`
- [ ] Mutation F: `xint1_isr` doesn't increment `Xint1Count` → `check_xint1_button` fails
- [ ] Run twice → identical
- [ ] Cross-regression: HW1, HW2, HW3 still pass

## Open questions / [ambiguous] from spec

- [ ] EPWM8A/B GPIO pin numbers — confirm against reference (40/41 vs 14/15 from one spec read).
- [ ] Servo CMPA endpoints — spec says 4%–12%; reference may use slightly tighter values for safety. Use *reference's*
  values if they fall within spec tolerance; document choice.
- [ ] Whether `xint1_isr` is supposed to do something visible besides `Xint1Count++` (e.g. trigger a servo move). If
  yes, add an assertion.
- [ ] State machine + LIDAR (Ex. 5) — out-of-scope for firmware grader (no LIDAR in stubs); document as "manual demo
  grading" in HW4.tex review.
- [ ] Print cadence/format — spec is loose on Ex. 3/4 print requirements. **[ambiguous]**: revisit when course staff
  clarify.
