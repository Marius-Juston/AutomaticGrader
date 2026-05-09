# Lab3 Grader Roadmap

**Authoritative spec:** `context/lab/Lab3.txt`
**Reference fixture:** `context/code_solutions/SE423_Lab_phuriy2_lourdes3_emilyah3/Lab3/`
**Implementation:** `src/checks/lab3.cpp` (deferred), `include/checks/lab3.h` (deferred)

## Spec inventory (Lab3.txt)

| Topic                 | Behavior                                                                                                                                                                                          |
|-----------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| EPWM motor PWM        | EPWM1A on GPIO0, EPWM2A on GPIO2, EPWM12A on GPIO22. `TBPRD == 50` (50 µs = 20 kHz). `TBCTL` up-count, free-soft. `AQCTLA` actions on compare.                                                    |
| EPWM pull-up disable  | `GpioCtrlRegs.GP*PUD.bit.GPIO*` set for PWM pins.                                                                                                                                                 |
| Required functions    | `void setEPWM1A(float controleffort)`, `void setEPWM2A(float controleffort)` — saturate `controleffort` to `[-10, 10]`, map to 0–100% duty. Negative effort handled per direction-pin convention. |
| EQEP setup            | EQEP1 (motor left), EQEP2 (motor right), EQEP3 (command knob). `QEPCTL` configured for 4× quad. `QPOSCNT` is the live count.                                                                      |
| Encoder readers       | `float readEncLeft()`, `float readEncRight()`, `float readEncWheel()` — convert raw counts to wheel radians via `2000 counts/rev × 1/20 gear ratio`.                                              |
| Velocity calculation  | Backwards-difference: `v = (p_current - p_old) / 0.001` (ft/s). 1 ms timer.                                                                                                                       |
| Friction compensation | Globals: `Vpos, Vneg, Cpos, Cneg`. `u = u + Vpos*vel + Cpos` (positive vel) or `u + Vneg*vel + Cneg` (negative vel).                                                                              |
| Print cadence         | Velocity printed every 100 ms (`%.2f` ft/s — limited precision for 20-char LCD).                                                                                                                  |

## Stubs needed

- [ ] All HW2 EPWM stubs (covers EPWM1, EPWM2, EPWM12 via the same calls).
- [ ] EQEP stubs: `EQEP_setEmulationMode`, `EQEP_setQEPSource`, `EQEP_enableModule`, `EQEP_setUnitPeriod`,
  `EQEP_setPositionCounterMode`, `EQEP_setPositionInitMode` — write `EQepXRegs.{QDECCTL, QEPCTL, QUPRD, QPOSMAX}`.
- [x] **Shared (shipped):** `serial_printf` / `UART_printfLine` capture → `g_printfCalls`. Auto-wired in
  `src/ti_stubs.cpp`; consume via `include/checks/printf_capture.h`.
- [x] **Shared (shipped):** stimulus helpers — `include/checks/stimulus.hpp`.
  `grader::inject_encoder_count(grader::EqepModule::Eqep1..3, int32_t)` writes `EQepXRegs.QPOSCNT`.
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

## Checks to implement

- [ ] `check_initialization` — `gpiosSetup[0]` mux for EPWM1A; same for EPWM2A and EPWM12A. `EPwm1Regs.TBPRD == 50`,
  `AQCTLA.bit.{ZRO, CAU}` configured. `EQep{1,2,3}Regs.QDECCTL` for 4×. Pull-ups disabled on PWM pins. Encoders enabled.
- [ ] `check_setEPWM1A` — sweep `controleffort ∈ {-20, -10, -5, 0, 5, 10, 20}`; assert `EPwm1Regs.CMPA.bit.CMPA` matches
  expected duty cycle. ±20 saturates to ±10. Linearity check at ±5.
- [ ] `check_setEPWM2A` — same matrix.
- [ ] `check_readEnc_scaling` — `grader::inject_encoder_count(grader::EqepModule::Eqep1, 2000)`; assert `readEncLeft()`
  returns ~2π × 1/20 rad (one motor revolution → 1/20 wheel rev). Verify with multiple values.
- [ ] `check_velocity_calc` — at 1 ms tick, inject encoder counts that change linearly; verify computed velocity matches
  expected ft/s.
- [ ] `check_friction_compensation` — set `Vpos = 1.0, Cpos = 2.0`, simulate positive velocity, assert
  `u_out == u_in + 1.0*vel + 2.0`. Same for negative branch.
- [ ] `check_print_cadence` — drive ISR for 1 s synthetic; assert 10 prints (100 ms).
- [ ] `check_print_format` — `expect_arg_types(latest, {grader::ArgType::Float, grader::ArgType::Float, ...})` with
  `%.2f` precision per spec.

## Validation matrix (deep)

- [ ] Reference → all pass, zero warnings, exit 0
- [ ] Mutation A: `TBPRD` 50→100 → fails on `EPwm1Regs.TBPRD`
- [ ] Mutation B: drop saturation in `setEPWM1A` → CMPA out-of-range → fails
- [ ] Mutation C: encoder gear ratio wrong (1/10 instead of 1/20) → `check_readEnc_scaling` fails
- [ ] Mutation D: friction compensation off-by-sign → `check_friction_compensation` fails
- [ ] Mutation E: `%d` instead of `%.2f` for velocity → `check_print_format` fails
- [ ] Run twice → identical
- [ ] Cross-regression: HW1, HW2 still pass

## Skip — demo only

- Oscilloscope PWM waveform verification
- Friction curve fitting in MATLAB (deliverable, not code)
- Motor mechanical spin direction verification

## Open questions / [ambiguous] from spec

- [ ] Friction coefficients `Vpos/Vneg/Cpos/Cneg` — student-tuned. Test asserts presence + linearity, not specific
  values.
- [ ] Exact velocity formula at zero crossing (which branch executes when vel == 0). Spec ambiguous; accept either.
- [ ] EQEP3 (command knob) source — confirm GPIO pin for command-knob input from spec.
