# HW4 Grader Roadmap

**Authoritative spec:** `context/homeworks/HW4.tex`
**Reference fixture (non-authoritative):** `context/code_solutions/myHW_Yingyord_First/HW4/`
**Implementation:** `src/checks/hw4.cpp` (to create), `include/checks/hw4.h` (to create)

## Spec inventory (HW4.tex)

| Topic              | Behavior                                                                                                                                       |
| ------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| RC servo EPWM8A/B  | 50 Hz carrier (20 ms period). At 100 MHz EPWMCLK with `CLKDIV=4`, `TBPRD=62500`. Duty range: −90°→4% (CMPA=2500), 0°→8% (CMPA=5000), +90°→12% (CMPA=7500). |
| Required functions | `void setEPWM8A_RCServo(float angle)` and `void setEPWM8B_RCServo(float angle)`. Saturate angle to [-90, 90]; map linearly to CMPA/CMPB.        |
| External interrupts| XINT1 on PB3 (GPIO6), XINT2 on PB4 (GPIO7); falling-edge polarity; GPIO qualify period 6 (debounce).                                            |
| ISRs               | `xint1_isr` increments `Xint1Count`; `xint2_isr` increments `Xint2Count`. Spec implies `IFR/PIEACK` cleanup (tested by re-entry).               |
| State machine      | 4 robot states (navigate, obstacle avoid w/ left wall-follow, right wall-follow, pink ball track). LIDAR 228 readings, element 113 = forward. **Mostly demo / not auto-graded.** |

## Stubs needed (`src/ti_stubs.cpp`)

- [ ] All HW2 EPWM stubs (for EPWM8)
- [ ] `GPIO_setInterruptPin(gpio, xintN)` → record `xintPinMap[xintN] = gpio`
- [ ] `GPIO_setInterruptType(xintN, type)` → `XintRegs.XINTnCR.bit.POLARITY`
- [ ] `XINT_enableInterrupt(xintN)` → `XintRegs.XINTnCR.bit.ENABLE = 1`
- [ ] `GPIO_setQualificationMode` → `GpioCtrlRegs.GPxQSEL{1,2}`, `GPxCTRL.QUALPRD`
- [ ] (Older driverlib) `GPIO_SetupXINTnGpio(gpio_num)` shorthand — same effect as the pair
- [x] **Shared (shipped):** `serial_printf` / `UART_printfLine` capture → `g_printfCalls`. Auto-wired in `src/ti_stubs.cpp`; consume via `include/checks/printf_capture.h`.
- [x] **Shared (shipped):** stimulus helpers — `include/checks/stimulus.hpp`. `grader::press_button(int gpio)` simulates the GPIO low; trigger XINT via direct `xint1_isr()` call.
- [x] **Shared (shipped):** synthetic clock + `run_isr_for_us` — `include/checks/synthetic_clock.h`.
- [x] **Shared (shipped):** format parser + `expect_format` / `expect_arg_types` / `expect_print_cadence` — `include/checks/format_parser.h` + `include/checks/expectations.h`. Run `./AutomaticGrader --selftest` to verify the infra after edits.

## Checks to implement (`src/checks/hw4.cpp`)

- [ ] `check_initialization` — HW1 baseline + EPWM8 + XINT register state. Verify `gpiosSetup[40]` and `gpiosSetup[41]` muxes for EPWM8A/B (or whichever pins reference uses). `EPwm8Regs.{TBPRD == 62500, TBCTL.HSPCLKDIV == 4, AQCTLA.ZRO == 2, AQCTLA.CAU == 1, CMPA == 5000, CMPB == 5000}` (8% duty initial = 0°). `xintPinMap[XINT1] == 6`, `xintPinMap[XINT2] == 7`. `XintRegs.XINT1CR.{POLARITY == 0, ENABLE == 1}` and same for XINT2. GPIO6/7 input + pullup + qualify period 6. `PieVectTable.{XINT1_INT == &xint1_isr, XINT2_INT == &xint2_isr}`.
- [ ] `check_setEPWM8A_RCServo` — direct call sweep:
  - `setEPWM8A_RCServo(0.0f)` → `EPwm8Regs.CMPA.bit.CMPA == 5000`
  - `setEPWM8A_RCServo(-90.0f)` → `CMPA == 2500`
  - `setEPWM8A_RCServo(+90.0f)` → `CMPA == 7500`
  - `setEPWM8A_RCServo(+200.0f)` → `CMPA == 7500` (saturated)
  - `setEPWM8A_RCServo(-200.0f)` → `CMPA == 2500` (saturated)
  - `setEPWM8A_RCServo(45.0f)` → `CMPA ≈ 6250` ±1 (linearity)
- [ ] `check_setEPWM8B_RCServo` — same matrix asserting `CMPB.bit.CMPB`.
- [ ] `check_xint1_button` — `Xint1Count = 0`; simulate 5 button presses by calling `xint1_isr()` (with optional `grader::press_button(6)`/`grader::release_button(6)` around each). Assert `Xint1Count == 5`. If reference toggles an LED in the ISR, also verify `GpioDataRegs`.
- [ ] `check_xint2_button` — same for `Xint2Count` and GPIO7.
- [ ] `check_print_cadence` — **[ambiguous]** — HW4 spec doesn't pin a print rate for Ex. 4 button counts. Skip cadence check unless spec wording dictates.
- [ ] `check_print_format` — if Ex. 3/4 prints exist per spec, `expect_arg_types(latest, ...)` for the documented variables (likely button counts as `grader::ArgType::Int32` or `grader::ArgType::Int16`).

## Validation matrix (deep)

- [ ] Reference (`myHW_Yingyord_First/HW4/`) → all pass, zero warnings, exit 0
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
- [ ] Servo CMPA endpoints — spec says 4%–12%; reference may use slightly tighter values for safety. Use *reference's* values if they fall within spec tolerance; document choice.
- [ ] Whether `xint1_isr` is supposed to do something visible besides `Xint1Count++` (e.g. trigger a servo move). If yes, add an assertion.
- [ ] State machine + LIDAR (Ex. 5) — out-of-scope for firmware grader (no LIDAR in stubs); document as "manual demo grading" in HW4.tex review.
- [ ] Print cadence/format — spec is loose on Ex. 3/4 print requirements. **[ambiguous]**: revisit when course staff clarify.
