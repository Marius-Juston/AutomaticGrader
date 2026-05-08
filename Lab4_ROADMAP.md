# Lab4 Grader Roadmap

**Authoritative spec:** `context/lab/Lab4.txt`
**Reference fixture:** `context/code_solutions/SE423_Lab_phuriy2_lourdes3_emilyah3/Lab4/`
**Implementation:** `src/checks/lab4.cpp` (deferred), `include/checks/lab4.h` (deferred)

## Spec inventory (Lab4.txt)

| Topic                  | Behavior                                                                                                                  |
| ---------------------- | ------------------------------------------------------------------------------------------------------------------------- |
| ADCD config            | EPWM4 trigger at 1 ms; SOC0=ADCIND0, SOC1=ADCIND1; 12-bit; interrupt on SOC1.                                            |
| ADC scaling            | `voltage = result * 3.0/4095.0`.                                                                                          |
| DAC functions          | `void setDACA(float volt)` and `void setDACB(float volt)` — saturate `[0, 3]`, write to `DacaRegs.DACVALS.bit.DACVALS` (and DACB). |
| `ADCD_ISR`             | Read ADC results, convert, echo to DAC; set `UARTPrint = 1` every 100 ms.                                                  |
| FIR filter             | 5-tap averaging; 4th-order Butterworth — coefficient arrays `b[]`, tap state arrays.                                       |
| Gyro ADCC              | ADCC with 4 SOCs (ADCINC2/3/4/5) at 1 ms.                                                                                  |
| Gyro zero-cal SM       | 0–2 s: ignore. 2–4 s: sum 2000 samples. After 4 s: subtract zero offset; convert to deg/s. Zero offsets: `zeroADCC2..5` floats. |
| Mic ADCB               | EPWM7 trigger at 0.1 ms (10 kHz); single SOC = ADCINB4; echo to DAC.                                                      |
| FFT                    | DMA + TI FFT lib; ping/pong 1024-sample blocks; find peak.                                                                |
| Print cadence/format   | Voltage `%.2f` or `%.3f` every 100 ms.                                                                                     |

## Stubs needed

- [ ] All HW2 ADC + EPWM stubs.
- [ ] ADCD/ADCC/ADCB SOC config (extension of HW2's stubs to other ADC modules).
- [ ] DAC stubs: writes to `Dac{a,b,c}Regs.DACVALS.bit.DACVALS`. (`Dac*Regs` already in TI headers.)
- [ ] DMA stubs (HW5 set, reused).
- [ ] `RFFT_f32` shim from HW5.
- [ ] **Shared:** `serial_printf` capture; `inject_adc_result`; clock-step helpers.

## Checks to implement

- [ ] `check_initialization` — `gpiosSetup[]` for any DAC output pin if relevant. ADC{B,C,D} SOC config matches spec channels. `DacaRegs.DACOUTEN == 1`. `PieVectTable.{ADCD1_INT, ADCC1_INT, ADCB1_INT}` registered.
- [ ] `check_setDACA` — sweep `volt ∈ {-1, 0, 1.5, 3, 5}`; assert `DacaRegs.DACVALS.bit.DACVALS == clamp(volt, 0, 3) * 4095/3` ±1.
- [ ] `check_setDACB` — same matrix for DACB.
- [ ] `check_ADCD_ISR_echo` — `inject_adc_result(ADCD, 0, 2048)`, call `ADCD_ISR()`; assert `DacaRegs.DACVALS` reflects 1.5 V (mid-scale). Same for ADCD SOC1 → DACB.
- [ ] `check_FIR_5tap_avg` — feed a known sequence to the filter; assert output equals `(s[0]+s[1]+s[2]+s[3]+s[4])/5`.
- [ ] `check_FIR_butterworth_impulse` — impulse input → student's filter output should match the spec'd `b[]` coefficients.
- [ ] `check_gyro_zero_cal_sm` — drive ADCC ISR for 4 s synthetic with constant injected voltage `V`; assert `zeroADCC2 ≈ V` after the 2–4 s sum. Drive past 4 s; assert subsequent samples have `(V - zeroADCC2) ≈ 0`.
- [ ] `check_print_cadence_voltage` — 100 ms; ±10%.
- [ ] `check_print_format_voltage` — `expect_arg_types(latest, {ARG_FLOAT, ARG_FLOAT})`.
- [ ] `check_FFT_peak` — preload `pwrSpec[]` peak at known bin; assert reported frequency.

## Validation matrix (deep)

- [ ] Reference → all pass, zero warnings
- [ ] Mutation A: ADC scaling 4095→4096 → fails on `voltage` mismatch
- [ ] Mutation B: drop DAC saturation → out-of-range → fails
- [ ] Mutation C: FIR coefficient swap → `check_FIR_*` fails
- [ ] Mutation D: gyro zero-cal end-time wrong (3 s instead of 4 s) → `check_gyro_zero_cal_sm` fails
- [ ] Mutation E: `%d` for voltage → `check_print_format` fails
- [ ] Run twice → identical
- [ ] Cross-regression: prior labs/HWs pass

## Skip — demo only

- Oscilloscope alias demonstration (visual)
- Manual gyro zero-tuning at rest (requires physical sensor)
- Live audio tone detection
- FFT visual validation

## Open questions / [ambiguous] from spec

- [ ] Print precision `%.2f` vs `%.3f` for voltage — confirm against reference.
- [ ] EPWM4 vs EPWM7 prescaler exact `TBPRD` for the 1 ms / 0.1 ms triggers.
- [ ] FIR Butterworth coefficient values — student-derived; test against the published table or against impulse-response.
- [ ] Gyro voltage→deg/s conversion factor — confirm against LPR510 datasheet snippet in spec.
