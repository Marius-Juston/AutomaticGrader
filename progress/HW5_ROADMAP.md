# HW5 Grader Roadmap

**Authoritative spec:** `context/homeworks/HW5.tex`
**Reference fixture (non-authoritative):** `context/code_solutions/myHW_Yingyord_First/HW5/HW5fy_main.c`
**Implementation:** `src/checks/hw5.cpp` (to create), `include/checks/hw5.h` (to create)

## Spec inventory (HW5.tex)

HW5.tex is brief — most content is "extend HW4 state machine" and "personal project". Firmware-gradable portions:

| Topic              | Behavior                                                                                                                              |
|--------------------|---------------------------------------------------------------------------------------------------------------------------------------|
| Microphone ADC     | EPWM-triggered ADC sampling at known rate; result fed to DMA ping-pong buffers (`AdcPingBufRaw[1024]`, `AdcPongBufRaw[1024]`).        |
| DMA channel 1      | Source = ADCA result reg; dest = ping/pong buffers; transfer size 1024; trigger ADCA INT1.                                            |
| `DMA_isr`          | On end-of-block, runs `RFFT_f32` on the just-filled buffer; fills `pwrSpec[513]`; finds peak frequency; toggles `pingpongFFT`.        |
| Servo carryover    | EPWM8A/B from HW4 still active.                                                                                                       |
| State machine Pt.3 | Golf ball collection (orange/blue/purple) — substates RobotState 20/22/24/26 (orange) and 30/32/34/36 (blue/purple). **Mostly demo.** |
| Personal project   | One servo + one sensor + working demo. Not auto-graded.                                                                               |

## Stubs needed (`src/ti_stubs.cpp`)

- [x] All HW2 ADC stubs and HW4 EPWM stubs — already shipped.
- [x] DMA via direct `DmaRegs.CH1.*` writes (the reference path) — `DmaRegs` global already exists.
- [x] `DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH1` — already exists.
- [x] **`RFFT_f32` shim** — no-op already shipped in `include/fpu32/fpu_rfft.h`.
- [ ] Driverlib
  `DMA_configAddresses/Burst/Transfer/Wrap/setTriggerSource/setInterruptMode/enableTrigger/startChannel/initController` —
  not used by FY reference. Defer until a student fixture needs them.
- [x] **Shared (shipped):** `serial_printf` / `UART_printfLine` capture → `g_printfCalls`. Auto-wired in
  `src/ti_stubs.cpp`; consume via `include/checks/printf_capture.h`.
- [x] **Shared (shipped):** stimulus helpers — `include/checks/stimulus.hpp` (`press_button`, `inject_adc_result`,
  `inject_spi_rx`, `inject_encoder_count`, `inject_lidar_*`).
- [x] **Shared (shipped):** synthetic clock + `run_isr_for_us` — `include/checks/synthetic_clock.h`.
- [x] **Shared (shipped):** format parser + `expect_format` / `expect_arg_types` / `expect_print_cadence` —
  `include/checks/format_parser.h` + `include/checks/expectations.h`. Run `./AutomaticGrader --selftest` to verify the
  infra after edits.

## Checks to implement (`src/checks/hw5.cpp`)

**Status:** four checks implemented and passing against `myHW_Yingyord_First/HW5/HW5fy_main.c`.

- [x] `check_initialization` — HW1/HW2 ADC + HW4 EPWM8 baselines + DMA register state:
    - `DmaRegs.CH1.SRC_ADDR_SHADOW` matches the low-32-bit truncation of `&AdcaResultRegs.ADCRESULT0` *or*
      `&AdcbResultRegs.ADCRESULT0` (reference uses ADCB; spec is loose).
    - `DmaRegs.CH1.DST_ADDR_SHADOW != 0` (ping/pong dest configured; symbol-resolution of the buffer name is deferred —
      slice 4).
    - `DmaRegs.CH1.TRANSFER_SIZE ∈ {1023, 1024}`.
    - `DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH1 ∈ {1 (ADCAINT1), 6 (ADCBINT1)}`.
    - `DmaRegs.CH1.MODE.all & 0x8901 == 0x8901` (CHINTE | PERINTE | CONTINUOUS | CH1).
    - `DmaRegs.CH1.CONTROL.RUN == 1`.
    - `PieVectTable.DMA_CH1_INT == &DMA_isr` (when DMA_isr is defined).
    - `PieCtrlRegs.PIEIER7.INTx1 == 1`.
    - EPWM7 `TBPRD > 0` and `ETSEL.SOCAEN == 1` (mic ADC trigger).
- [x] `check_servo_carryover` — same six-case sweep (-90/0/+90/±200 saturation/+45 linearity) on `setEPWM8A_RCServo` and
  `setEPWM8B_RCServo`.
- [x] `check_pingpong_swap` — zero `DMAcount`/`pingFFT`/`pongFFT`/`iPingPong`, drive `DMA_isr()` 3×, assert
  `DMAcount == 3` and at least one of `{pingFFT, pongFFT}` was set.
- [x] `check_print_format` — drives Timer2 ISRs for 8 s synthetic time, accepts either two-float (Power, Freq) or
  four-float arg-type signature; emits a warning (not failure) if no print is captured (spec is sparse).
- [ ] `check_peak_detection` — deferred. Reference's `RFFT_f32` is a no-op shim, so we can't drive `pwrSpec[]`
  realistically without a working FFT. Slice ?: pre-populate `pwrSpec[]` and call the student's peak-finder helper
  directly when exposed.
- [ ] `check_dma_isr_advance` — folded into `check_pingpong_swap` (the `DMAcount == 3` assertion).
- [ ] `check_print_cadence` — **[ambiguous]** — HW5 spec doesn't pin a cadence; deliberately not implemented.

## Validation matrix (deep)

- [x] Reference (`myHW_Yingyord_First/HW5/HW5fy_main.c`) → all 4 active checks pass, "Succeeded all tests!" (one warn
  for missing print).
- [ ] Mutation A: DMA transfer size 1024→512 → fails on `dmaConfig[CH1].transferSize`
- [ ] Mutation B: bind DMA src to `AdcbResultRegs` → fails on `dmaConfig[CH1].src`
- [ ] Mutation C: break ping-pong toggle (always process ping) → `check_pingpong_swap` fails
- [ ] Mutation D: peak detection returns index 0 always → `check_peak_detection` fails on frequency
- [ ] Mutation E: format string mismatch → `check_print_format` fails
- [ ] Run twice → identical
- [ ] Cross-regression: HW1..HW4 still pass

## Open questions / [ambiguous] from spec

- [ ] Sample rate (EPWM4/EPWM7 trigger period for the mic). HW5.tex uses prior assignment's setup; pin via reference.
- [ ] HW5 print format/cadence — spec is sparse. Treat as "if a print exists in the reference, assert format/cadence;
  otherwise skip".
- [ ] State machine + golf ball logic (Ex. 5 personal project) — demo-only. Not auto-graded.
- [ ] `RFFT_f32` shim should compute real magnitudes? Recommend pure no-op + direct `pwrSpec[]` injection for
  determinism.
