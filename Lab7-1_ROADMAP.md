# Lab7-1 Grader Roadmap (RC Servo)

**Authoritative spec:** `context/lab/Lab7-1.txt`
**Reference fixture:** `context/code_solutions/SE423_Lab_phuriy2_lourdes3_emilyah3/Lab7-1/`
**Implementation:** `src/checks/lab7-1.cpp` (deferred), `include/checks/lab7-1.h` (deferred)

## Spec inventory (Lab7-1.txt)

| Topic              | Behavior                                                                                                                                                                                                         |
|--------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| RC servo PWM       | EPWM3A, EPWM3B, EPWM5A, EPWM5B, EPWM6A. Standard 50 Hz, 1–2 ms pulse for ±90°. CMPA mapping same as HW4 (4–12% duty).                                                                                            |
| Required functions | 5 functions: `setEPWM3A_RCServo(float angle)`, `setEPWM3B_RCServo(float angle)`, `setEPWM5A_RCServo(float angle)`, `setEPWM5B_RCServo(float angle)`, `setEPWM6A_RCServo(float angle)`. Each saturates [-90, 90]. |
| Timer2 ISR         | 40 ms; posts SWI3 (lowest priority); SWI3 calls all 5 servo functions with the current `RCangle`.                                                                                                                |
| `RCangle` source   | `readEncWheel()` from Lab3; range [−90, 90].                                                                                                                                                                     |
| Print cadence      | `RCangle` printed every 100 ms.                                                                                                                                                                                  |
| Print format       | Float `%.2f`. **[ambiguous]** — confirm precision against spec.                                                                                                                                                  |

## Stubs needed

- [ ] All HW2 EPWM stubs (EPWM3, 5, 6 use the same calls as 4, 8, 12).
- [ ] EQEP3 from Lab3 (carryover).
- [ ] SWI post mechanism from Lab6.
- [x] **Shared (shipped):** `serial_printf` / `UART_printfLine` capture → `g_printfCalls`. Auto-wired in
  `src/ti_stubs.cpp`; consume via `include/checks/printf_capture.h`.
- [x] **Shared (shipped):** stimulus helpers — `include/checks/stimulus.hpp` (
  `grader::inject_encoder_count(grader::EqepModule::Eqep3, count)` for the command knob).
- [x] **Shared (shipped):** synthetic clock + `run_isr_for_us` — `include/checks/synthetic_clock.h`.
- [x] **Shared (shipped):** format parser + `expect_format` / `expect_arg_types` / `expect_print_cadence` —
  `include/checks/format_parser.h` + `include/checks/expectations.h`. Run `./AutomaticGrader --selftest` to verify the
  infra after edits.
- [ ] **Deferred (carried from Lab6):** SWI post capture (`PostSWI3`) — drive the SWI ISR by direct call until that
  ships.

## Checks to implement

- [ ] `check_initialization` — `gpiosSetup[]` for EPWM3A/B, EPWM5A/B, EPWM6A pins.
  `EPwm{3,5,6}Regs.{TBPRD, AQCTLA, CMPA initial}` matches spec.
- [ ] `check_setEPWM3A_RCServo` — sweep `angle ∈ {-200, -90, -45, 0, 45, 90, 200}`; assert `EPwm3Regs.CMPA.bit.CMPA`
  matches expected per the same 4–12% duty mapping as HW4.
- [ ] `check_setEPWM3B_RCServo` — same matrix, `CMPB`.
- [ ] `check_setEPWM5A_RCServo` — same matrix, `EPwm5Regs.CMPA`.
- [ ] `check_setEPWM5B_RCServo` — same matrix, `EPwm5Regs.CMPB`.
- [ ] `check_setEPWM6A_RCServo` — same matrix, `EPwm6Regs.CMPA`.
- [ ] `check_swi3_posts_servos` — drive `cpu_timer2_isr()` once; assert `PostSWI3` was called. Drive
  `SWI3_LowestPriority()`; assert all 5 `EPwm*Regs.CMPA/CMPB` updated to reflect the current `RCangle`.
- [ ] `check_RCangle_from_encoder` — `grader::inject_encoder_count(grader::EqepModule::Eqep3, count)`; drive ISR; assert
  `RCangle == readEncWheel()` derived value, clamped to [-90, 90].
- [ ] `check_print_cadence` — 100 ms.
- [ ] `check_print_format` — `expect_arg_types(latest, {grader::ArgType::Float})`.

## Validation matrix (deep)

- [ ] Reference → all pass
- [ ] Mutation A: drop saturation in `setEPWM3A_RCServo` → CMPA out-of-range → fails
- [ ] Mutation B: swap two CMPA values → wrong direction → fails
- [ ] Mutation C: missing servo function → linker error or runtime fail
- [ ] Mutation D: SWI3 posting wrong priority → `check_swi3_posts_servos` may still pass; verify priority via PIE
  inspection
- [ ] Mutation E: `%d` for RCangle → `check_print_format` fails
- [ ] Run twice → identical
- [ ] Cross-regression

## Skip — demo only

- Physical servo mechanical movement
- Servo torque verification

## Open questions / [ambiguous] from spec

- [ ] EPWM3/5/6 GPIO pin numbers — confirm against reference (likely 4/5, 8/9, 10).
- [ ] `RCangle` clamp semantics — saturate at the function level (each `setEPWM*_RCServo`) vs at the assignment to
  `RCangle`.
- [ ] Print precision `%.2f` — [ambiguous].
