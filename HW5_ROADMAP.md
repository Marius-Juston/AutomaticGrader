# HW5 Grader Roadmap

**Authoritative spec:** `context/homeworks/HW5.tex`
**Reference fixture (non-authoritative):** `context/code_solutions/myHW_Yingyord_First/HW5/HW5fy_main.c`
**Implementation:** `src/checks/hw5.cpp` (to create), `include/checks/hw5.h` (to create)

## Spec inventory (HW5.tex)

HW5.tex is brief — most content is "extend HW4 state machine" and "personal project". Firmware-gradable portions:

| Topic              | Behavior                                                                                       |
| ------------------ | ---------------------------------------------------------------------------------------------- |
| Microphone ADC     | EPWM-triggered ADC sampling at known rate; result fed to DMA ping-pong buffers (`AdcPingBufRaw[1024]`, `AdcPongBufRaw[1024]`). |
| DMA channel 1      | Source = ADCA result reg; dest = ping/pong buffers; transfer size 1024; trigger ADCA INT1.    |
| `DMA_isr`          | On end-of-block, runs `RFFT_f32` on the just-filled buffer; fills `pwrSpec[513]`; finds peak frequency; toggles `pingpongFFT`. |
| Servo carryover    | EPWM8A/B from HW4 still active.                                                                |
| State machine Pt.3 | Golf ball collection (orange/blue/purple) — substates RobotState 20/22/24/26 (orange) and 30/32/34/36 (blue/purple). **Mostly demo.** |
| Personal project   | One servo + one sensor + working demo. Not auto-graded.                                        |

## Stubs needed (`src/ti_stubs.cpp`)

- [ ] All HW2 ADC stubs and HW4 EPWM stubs
- [ ] `DMA_configAddresses(channel, destAddr, srcAddr)` → `dmaConfig[ch].{src, dst}`
- [ ] `DMA_configBurst(channel, size, srcStep, dstStep)` → `dmaConfig[ch].{burstSize, srcStep, dstStep}`
- [ ] `DMA_configTransfer(channel, count, srcStep, dstStep)` → `dmaConfig[ch].transferSize`
- [ ] `DMA_configWrap` → wrap params (ping-pong wrap of 2)
- [ ] `DMA_setTriggerSource(channel, trigger)` → `dmaConfig[ch].triggerSrc`
- [ ] `DMA_setInterruptMode`, `DMA_enableTrigger`, `DMA_startChannel` → `dmaConfig[ch].running = true`
- [ ] `DMA_initController` — no-op
- [ ] **`RFFT_f32` shim** — no-op by default; for `check_peak_detection` the test pre-populates `pwrSpec[]` so the shim can stay no-op.
- [x] **Shared (shipped):** `serial_printf` / `UART_printfLine` capture → `g_printfCalls`. Auto-wired in `src/ti_stubs.cpp`; consume via `include/checks/printf_capture.h`.
- [x] **Shared (shipped):** stimulus helpers — `include/checks/stimulus.hpp` (`press_button`, `inject_adc_result`, `inject_spi_rx`, `inject_encoder_count`, `inject_lidar_*`).
- [x] **Shared (shipped):** synthetic clock + `run_isr_for_us` — `include/checks/synthetic_clock.h`.
- [x] **Shared (shipped):** format parser + `expect_format` / `expect_arg_types` / `expect_print_cadence` — `include/checks/format_parser.h` + `include/checks/expectations.h`. Run `./AutomaticGrader --selftest` to verify the infra after edits.

## Checks to implement (`src/checks/hw5.cpp`)

- [ ] `check_initialization` — HW1, HW2 ADC, HW4 EPWM8 baselines + DMA register state.
  - `dmaConfig[CH1].src == &AdcaResultRegs.ADCRESULT0` (or whichever microphone result reg)
  - Both `AdcPingBufRaw` and `AdcPongBufRaw` referenced as ping/pong destinations (verify via wrap config)
  - `dmaConfig[CH1].transferSize == 1024`, `burstSize == 1`
  - `dmaConfig[CH1].triggerSrc == DMA_TRIGGER_ADCAINT1`
  - `dmaConfig[CH1].running == true`
  - `PieVectTable.DMA_CH1_INT == &DMA_isr`
- [ ] `check_pingpong_swap` — preset `pingFFT = 1; pongFFT = 0; pingpongFFT = 0`. Call `DMA_isr()` twice. Assert `pingpongFFT` toggled twice; `pingFFT`/`pongFFT` flags drive correct buffer per call.
- [ ] `check_peak_detection` — zero `pwrSpec[]`; set `pwrSpec[100] = 1000.0f`. Either invoke the student's peak-finding helper directly (if exposed) or call `DMA_isr()` after preloading. Assert reported peak frequency = `100 * SAMPLE_RATE / 1024` ±1 Hz tolerance. (Sample rate from spec; **[ambiguous]** — likely 10 kHz from EPWM4 trigger.)
- [ ] `check_dma_isr_advance` — `DMAcount = 0`; call `DMA_isr()` 5×; assert `DMAcount == 5`.
- [ ] `check_servo_carryover` — same `setEPWM8A_RCServo` / `setEPWM8B_RCServo` parameter sweep as HW4 (HW5 inherits servo).
- [ ] `check_print_cadence` — drive ISR for 1000 ms synthetic; assert N prints per spec'd cadence (likely 100 ms or 200 ms — **[ambiguous]**).
- [ ] `check_print_format` — `expect_arg_types(latest, {grader::ArgType::Float, ...})` for peak-frequency print. Float for frequency.

## Validation matrix (deep)

- [ ] Reference (`myHW_Yingyord_First/HW5/`) → all pass, zero warnings, exit 0
- [ ] Mutation A: DMA transfer size 1024→512 → fails on `dmaConfig[CH1].transferSize`
- [ ] Mutation B: bind DMA src to `AdcbResultRegs` → fails on `dmaConfig[CH1].src`
- [ ] Mutation C: break ping-pong toggle (always process ping) → `check_pingpong_swap` fails
- [ ] Mutation D: peak detection returns index 0 always → `check_peak_detection` fails on frequency
- [ ] Mutation E: format string mismatch → `check_print_format` fails
- [ ] Run twice → identical
- [ ] Cross-regression: HW1..HW4 still pass

## Open questions / [ambiguous] from spec

- [ ] Sample rate (EPWM4/EPWM7 trigger period for the mic). HW5.tex uses prior assignment's setup; pin via reference.
- [ ] HW5 print format/cadence — spec is sparse. Treat as "if a print exists in the reference, assert format/cadence; otherwise skip".
- [ ] State machine + golf ball logic (Ex. 5 personal project) — demo-only. Not auto-graded.
- [ ] `RFFT_f32` shim should compute real magnitudes? Recommend pure no-op + direct `pwrSpec[]` injection for determinism.
