# HW2 Grader Roadmap

**Authoritative spec:** `context/homeworks/HW2.tex`
**Reference fixture (non-authoritative):** `context/code_solutions/myHW_Yingyord_First/HW2/`
**Implementation:** `src/checks/hw2.cpp` (to create), `include/checks/hw2.h` (to create)

## Spec inventory (HW2.tex)

| Topic                | Behavior                                                                                                  |
| -------------------- | --------------------------------------------------------------------------------------------------------- |
| EPWM12A on GPIO22    | 5 kHz carrier; `TBPRD` derived from 50 MHz EPWMCLK; `AQCTLA.ZRO=2` (set high), `AQCTLA.CAU=1` (set low). 50% duty initial. |
| Brightness ramp      | Inside `cpu_timer2_isr` (1 ms): inc/dec `CMPA` by 1 each tick; `updown` flag toggles at 0 and `TBPRD` boundaries. |
| EPWM4 SOCA           | Triggers ADCA SOC0/1/2 at 1 ms; `TBPRD` per 50 MHz; `ETSEL.SOCAEN=1`, `ETPS.SOCAPRD=1`.                   |
| ADCA channels        | SOC0=ADCINA4 (photodiode), SOC1=ADCINA2 (joystick X), SOC2=ADCINA3 (joystick Y); 12-bit; voltage = `result * 3.0/4096.0`. |
| `ADCA_ISR`           | Reads `Adca4result`, `JoystickXresult`, `JoystickYresult`; converts to voltage; increments `AdcaInterruptCount`. |
| Photodiode LED bar   | 5 MSBs of `Adca4result` light LEDs 6/11, 2/7/12, 3/8/13, 4/9/14, 5/10/15 in that order.                  |
| Joystick LED         | Center → LED8 only. Y-up (toward buzzer) → LED6. Y-down → LED10. X-up → LED3. X-down → LED13.            |
| Print cadence        | Photodiode + joystick voltages every 100 ms.                                                              |
| Print format         | Three floats. Spec wording uses `%f`; reference uses `%.2f`. **[ambiguous]** — accept either if the type is `grader::ArgType::Float`. |

## Stubs needed (`src/ti_stubs.cpp`)

- [ ] `EPWM_setClockPrescaler` → `EPwmXRegs.TBCTL.bit.{HSPCLKDIV, CLKDIV}`
- [ ] `EPWM_setTimeBasePeriod` → `EPwmXRegs.TBPRD`
- [ ] `EPWM_setCounterCompareValue` → `CMPA.bit.CMPA` / `CMPB.bit.CMPB`
- [ ] `EPWM_setActionQualifierAction` → `AQCTLA`/`AQCTLB` bitfields
- [ ] `EPWM_setCounterCompareShadowLoadMode` → `CMPCTL`
- [ ] `EPWM_setTimeBaseCounterMode`, `EPWM_setPhaseShift`, `EPWM_disablePhaseShiftLoad` → `TBCTL` fields
- [ ] `EPWM_enableADCTrigger` / `EPWM_setADCTriggerSource` / `EPWM_setADCTriggerEventPrescale` → `ETSEL.bit.{SOCAEN, SOCASEL}`, `ETPS.bit.SOCAPRD`
- [ ] `ADC_setupSOC(base, soc, trigger, channel, sampleWindow)` → `AdcXRegs.ADCSOCnCTL.bit.{CHSEL, TRIGSEL, ACQPS}`
- [ ] `ADC_setVREF`, `ADC_setMode`, `ADC_enableConverter`, `ADC_setSOCPriority` → `ADCCTL{1,2}` fields
- [ ] `ADC_setInterruptSource`, `ADC_setInterruptPulseMode`, `ADC_enableInterrupt` → `ADCINTSEL1N2`, `ADCCTL1.INTPULSEPOS`
- [x] **Shared (shipped):** `serial_printf` / `UART_printfLine` capture → `g_printfCalls`. Auto-wired in `src/ti_stubs.cpp`; consume via `include/checks/printf_capture.h`.
- [x] **Shared (shipped):** stimulus helpers — `include/checks/stimulus.hpp` (`press_button`, `inject_adc_result`, `inject_spi_rx`, `inject_encoder_count`, `inject_lidar_*`).
- [x] **Shared (shipped):** synthetic clock + `run_isr_for_us` — `include/checks/synthetic_clock.h`.
- [x] **Shared (shipped):** format parser + `expect_format` / `expect_arg_types` / `expect_print_cadence` — `include/checks/format_parser.h` + `include/checks/expectations.h`. Run `./AutomaticGrader --selftest` to verify the infra after edits.
- [ ] **Deferred:** EPWM-triggered ADC SOC plumbing (the *trigger path* through `EPWM4 → SOCA → ADCA1`). `inject_adc_result` ships the result-side; the trigger wiring still needs ETSEL/ETPS capture + an EPWM tick-to-SOC bridge.

## Checks to implement (`src/checks/hw2.cpp`)

- [ ] `check_initialization` — HW1 baseline + HW2 EPWM12A/EPWM4/ADCA register state. Verify `gpiosSetup[22]` mux for EPWM12A, `EPwm12Regs.TBPRD == 5000` (or whatever 5 kHz gives at the configured prescaler — accept the value the reference uses), `AQCTLA.ZRO == 2`, `AQCTLA.CAU == 1`, `CMPA == TBPRD/2`. EPWM4 `TBPRD` for 1 kHz, `ETSEL.SOCAEN == 1`. ADCA SOC0/1/2 channels correct. `PieVectTable.ADCA1_INT == &ADCA_ISR`.
- [ ] `check_pwm_ramp` — drive `cpu_timer2_isr()` `2*TBPRD` synthetic ticks. Sample `CMPA` mid-ramp and at boundaries; assert it reaches `TBPRD`, then ramps to 0, and `updown` flips at each boundary.
- [ ] `check_adc_isr_scaling` — `grader::inject_adc_result(grader::AdcModule::A, 0, 4096)`, `inject_adc_result(AdcModule::A, 1, 2048)`, `inject_adc_result(AdcModule::A, 2, 0)`. Call `ADCA_ISR()`. Assert: `Adca4result == 4096`, `Adca4voltage ≈ 3.0` ±1e-3; `JoystickXresult == 2048`, `JoystickXvoltage ≈ 1.5`; `JoystickYresult == 0`, `JoystickYvoltage ≈ 0.0`. `AdcaInterruptCount` incremented.
- [ ] `check_photodiode_led_bar` — sweep `Adca4result` ∈ {0, 1024, 2048, 3072, 4095}, call the student's LED-bar update function (or trigger via `ADCA_ISR()` then snapshot `GpioDataRegs`), assert the documented LED bits per 5-MSB-bucket spec.
- [ ] `check_joystick_leds` — for canonical positions (center, X-up, X-down, Y-up, Y-down), inject the corresponding ADCB results, drive ISR, snapshot `GpioDataRegs`, assert spec'd LED state.
- [ ] `check_print_cadence` — `grader::run_isr_for_us(cpu_timer2_isr, CpuTimer2.PeriodInUSec, 1'000'000)`; `grader::expect_print_cadence(grader::SerialPort::SCIA, 10, 0.10)`.
- [ ] `check_print_format` — `expect_format(latest, "<spec'd format>")`, `expect_arg_types(latest, {grader::ArgType::Float, grader::ArgType::Float, grader::ArgType::Float})`. **[ambiguous]**: spec doesn't pin precision; accept any precision in [0,3].

## Validation matrix (deep)

- [ ] Reference (`myHW_Yingyord_First/HW2/`) → all pass, zero warnings, exit 0
- [ ] Mutation A (numeric): `EPwm12Regs.TBPRD` 5000→6000 → fails on `EPwm12Regs TBPRD`
- [ ] Mutation B (structural): drop `EPWM_enableADCTrigger` → fails on `EPwm4Regs ETSEL.SOCAEN`
- [ ] Mutation C (channel swap): `ADCSOC0CTL.CHSEL` 4→2 (use joystick instead of photodiode) → fails on `AdcaRegs ADCSOC0CTL.CHSEL`
- [ ] Mutation D (scaling): change `3.0/4096.0` → `3.3/4096.0` → fails on `Adca4voltage expected 3.0 got 3.3`
- [ ] Mutation E (format): `%.2f` → `%d` for one of the floats → `check_print_format` fails on `arg N type mismatch`
- [ ] Mutation F (cadence): change `cpu_timer2_isr` modulus from 100 to 200 → `check_print_cadence` fails
- [ ] Run twice without rebuild → identical outcome
- [ ] Cross-regression: `ASSIGNMENT=HW1` still passes after HW2 stubs land

## Open questions / [ambiguous] from spec

- [ ] Joystick voltage thresholds for LED state mapping — spec uses qualitative wording. Pin down to numeric thresholds when implementing.
- [ ] Print format precision — spec says `%f`, reference uses `%.2f`. Accept any precision; type must be `grader::ArgType::Float`.
- [ ] Photodiode LED-bar order (which 5-MSB bucket lights which LED first) — confirm against reference.
