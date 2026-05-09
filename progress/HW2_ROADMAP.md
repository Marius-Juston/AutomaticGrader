# HW2 Grader Roadmap

**Authoritative spec:** `context/homeworks/HW2.tex`
**Reference fixture (non-authoritative):** `context/code_solutions/myHW_Yingyord_First/HW2/`
**Implementation:** `src/checks/hw2.cpp` (to create), `include/checks/hw2.h` (to create)

## Spec inventory (HW2.tex)

| Topic              | Behavior                                                                                                                              |
|--------------------|---------------------------------------------------------------------------------------------------------------------------------------|
| EPWM12A on GPIO22  | 5 kHz carrier; `TBPRD` derived from 50 MHz EPWMCLK; `AQCTLA.ZRO=2` (set high), `AQCTLA.CAU=1` (set low). 50% duty initial.            |
| Brightness ramp    | Inside `cpu_timer2_isr` (1 ms): inc/dec `CMPA` by 1 each tick; `updown` flag toggles at 0 and `TBPRD` boundaries.                     |
| EPWM4 SOCA         | Triggers ADCA SOC0/1/2 at 1 ms; `TBPRD` per 50 MHz; `ETSEL.SOCAEN=1`, `ETPS.SOCAPRD=1`.                                               |
| ADCA channels      | SOC0=ADCINA4 (photodiode), SOC1=ADCINA2 (joystick X), SOC2=ADCINA3 (joystick Y); 12-bit; voltage = `result * 3.0/4096.0`.             |
| `ADCA_ISR`         | Reads `Adca4result`, `JoystickXresult`, `JoystickYresult`; converts to voltage; increments `AdcaInterruptCount`.                      |
| Photodiode LED bar | 5 MSBs of `Adca4result` light LEDs 6/11, 2/7/12, 3/8/13, 4/9/14, 5/10/15 in that order.                                               |
| Joystick LED       | Center → LED8 only. Y-up (toward buzzer) → LED6. Y-down → LED10. X-up → LED3. X-down → LED13.                                         |
| Print cadence      | Photodiode + joystick voltages every 100 ms.                                                                                          |
| Print format       | Three floats. Spec wording uses `%f`; reference uses `%.2f`. **[ambiguous]** — accept either if the type is `grader::ArgType::Float`. |

## Stubs needed (`src/ti_stubs.cpp`)

- [ ] `EPWM_setClockPrescaler` → `EPwmXRegs.TBCTL.bit.{HSPCLKDIV, CLKDIV}`
- [ ] `EPWM_setTimeBasePeriod` → `EPwmXRegs.TBPRD`
- [ ] `EPWM_setCounterCompareValue` → `CMPA.bit.CMPA` / `CMPB.bit.CMPB`
- [ ] `EPWM_setActionQualifierAction` → `AQCTLA`/`AQCTLB` bitfields
- [ ] `EPWM_setCounterCompareShadowLoadMode` → `CMPCTL`
- [ ] `EPWM_setTimeBaseCounterMode`, `EPWM_setPhaseShift`, `EPWM_disablePhaseShiftLoad` → `TBCTL` fields
- [ ] `EPWM_enableADCTrigger` / `EPWM_setADCTriggerSource` / `EPWM_setADCTriggerEventPrescale` →
  `ETSEL.bit.{SOCAEN, SOCASEL}`, `ETPS.bit.SOCAPRD`
- [ ] `ADC_setupSOC(base, soc, trigger, channel, sampleWindow)` → `AdcXRegs.ADCSOCnCTL.bit.{CHSEL, TRIGSEL, ACQPS}`
- [ ] `ADC_setVREF`, `ADC_setMode`, `ADC_enableConverter`, `ADC_setSOCPriority` → `ADCCTL{1,2}` fields
- [ ] `ADC_setInterruptSource`, `ADC_setInterruptPulseMode`, `ADC_enableInterrupt` → `ADCINTSEL1N2`,
  `ADCCTL1.INTPULSEPOS`
- [x] **Shared (shipped):** `serial_printf` / `UART_printfLine` capture → `g_printfCalls`. Auto-wired in
  `src/ti_stubs.cpp`; consume via `include/checks/printf_capture.h`.
- [x] **Shared (shipped):** stimulus helpers — `include/checks/stimulus.hpp` (`press_button`, `inject_adc_result`,
  `inject_spi_rx`, `inject_encoder_count`, `inject_lidar_*`).
- [x] **Shared (shipped):** synthetic clock + `run_isr_for_us` — `include/checks/synthetic_clock.h`.
- [x] **Shared (shipped):** format parser + `expect_format` / `expect_arg_types` / `expect_print_cadence` —
  `include/checks/format_parser.h` + `include/checks/expectations.h`. Run `./AutomaticGrader --selftest` to verify the
  infra after edits.
- [ ] **Deferred:** EPWM-triggered ADC SOC plumbing (the *trigger path* through `EPWM4 → SOCA → ADCA1`).
  `inject_adc_result` ships the result-side; the trigger wiring still needs ETSEL/ETPS capture + an EPWM tick-to-SOC
  bridge.

## Checks to implement (`src/checks/hw2.cpp`)

- [x] `check_initialization` — HW1 GPIO baseline + HW2-specific:
    - GPIO22 mux=5 for EPWM12A; LED + button banks per HW1.
    - CpuTimer0=10 ms, CpuTimer1=20 ms, CpuTimer2=1 ms (matches reference); `*Regs.TCR.all == 0x4000`.
    - IER bitmask = `M_INT1 | M_INT8 | M_INT9 | M_INT12 | M_INT13 | M_INT14`; `PieCtrlRegs.PIEIER1.{INTx7, INTx1}` and
      `PIEIER12.INTx9` set.
    - `PieVectTable.ADCA1_INT == &ADCA_ISR` plus the HW1 timer/SCI vectors.
    - `EPwm12Regs.TBPRD ≈ 10000` (50 MHz / 5 kHz), `AQCTLA.{CAU, ZRO} = {1, 2}`, `CTRMODE = 0`, `CLKDIV = 0`.
    - `EPwm4Regs.TBPRD ≈ 50000` (1 ms), `ETSEL.{SOCAEN, SOCASEL} = {1, 2}`, `ETPS.SOCAPRD = 1`.
    - ADCA SOC0/1/2 `CHSEL = {4, 2, 3}`, all `TRIGSEL = 0x0B` (EPWM4 SOCA), `ADCINTSEL1N2.INT1SEL = 2`, `INT1E = 1`,
      `ADCCTL1.ADCPWDNZ = 1`.
    - Skipped registers (intentional, written by student): `AdcaRegs`, `AdcaResultRegs`, `EPwm4Regs`, `EPwm12Regs`,
      `GpioCtrlRegs`, `GpioDataRegs`, `DacaRegs` (DAC trim — host artefact).
- [ ] `check_pwm_ramp` — **deferred**; depends on the EPWM-SOC trigger plumbing (still on the slice deferred list) and
  the brightness-ramp logic in `cpu_timer2_isr` which the reference has commented out.
- [x] `check_adc_isr_scaling` — three injection cases (`max/half/zero`, `rotated`, `uniform-quarter`); for each calls
  `ADCA_ISR()` and asserts `result` mirrors injection, `voltage = result * 3.0/4096`, `AdcaInterruptCount` advances by
  1, and `ADCINTFLGCLR.ADCINT1` is set. Snapshot/restore of all 7 globals.
- [ ] `check_photodiode_led_bar` — **deferred**; the Yingyord reference has `setLightLEDs(Adca4result)` commented out
  inside `ADCA_ISR`, so the LED bar isn't observable through the existing pipeline. Re-enable when a spec-compliant
  fixture lands.
- [ ] `check_joystick_leds` — **deferred**; the reference's `setJoystickLEDs` thresholds aren't pinned to numeric values
  in the spec. Pin them when spec is clarified.
- [x] `check_print_cadence` — drives `ADCA_ISR()` 1000× in 10 bursts of 100 (each burst sets `UARTPrint=1` once); 10 ms
  wall-sleep between bursts to let the main thread service prints. Asserts `expect_print_cadence(SCIA, 10, 0.30)`.
- [x] `check_print_format` — `expect_arg_types(latest, {Float, Float, Float})`. Precision intentionally unconstrained.

## Validation matrix (deep)

- [x] Reference (`myHW_Yingyord_First/HW2/HW2fy_main.c`) → `Succeeded all tests!`, exit 0 (4 active checks + 3
  documented as deferred above).
- [x] Negative test: `STUDENT_SRC=student_work/LABstarter_main.c` (HW1 starter, no HW2 logic) → all 4 checks correctly
  fail or skip with explicit `missing student symbols` errors (`ADCA_ISR`, `Adca4result`, etc.).
- [ ] Mutation A: `EPwm12Regs.TBPRD` 10000→6000 → `check_initialization` fails on `TBPRD not ~10000`
- [ ] Mutation B: drop `EPwm4Regs.ETSEL.bit.SOCAEN = 1` → fails on `EPwm4Regs.ETSEL.SOCAEN != 1`
- [ ] Mutation C: `ADCSOC0CTL.CHSEL` 4→2 → fails on `ADCSOC0CTL.CHSEL != 4`
- [ ] Mutation D: change `3.0/4096.0` → `3.3/4096.0` → `check_adc_isr_scaling` fails on `voltage scaling off`
- [ ] Mutation E: `%f` → `%d` for one float → `check_print_format` fails on `argument #N: expected float, got int`
- [ ] Mutation F: change `(AdcaInterruptCount % 100) == 0` modulus to 200 → `check_print_cadence` fails on cadence
  window
- [x] Cross-regression: `ASSIGNMENT=HW1` still passes after HW2 changes (HW2 only added new files and per-assignment
  `mark_as_used` calls; no shared-infra modification).

## Open questions / [ambiguous] from spec

- [ ] Joystick voltage thresholds for LED state mapping — spec uses qualitative wording. Pin down to numeric thresholds
  when implementing.
- [ ] Print format precision — spec says `%f`, reference uses `%.2f`. Accept any precision; type must be
  `grader::ArgType::Float`.
- [ ] Photodiode LED-bar order (which 5-MSB bucket lights which LED first) — confirm against reference.
